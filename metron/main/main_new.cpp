#include "main.h"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/CInstance.hpp"
#include "metron/CParser.hpp"
#include "metron/CSourceFile.hpp"
#include "metron/CSourceRepo.hpp"
#include "metron/Cursor.hpp"
#include "metron/Emitter.hpp"
#include "metron/Dumper.hpp"
#include "metron/MtUtils.h"
#include "metron/Tracer.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeAssignment.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeSwitch.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"
#include <filesystem>

using namespace matcheroni;
namespace fs = std::filesystem;

CNodeField* resolve_field(CNodeClass* node_class, CNode* node_name);
void sanity_check_parse_tree(CSourceRepo& repo);
Err build_call_graph(CNodeClass* node, CSourceRepo* repo);
bool has_non_terminal_return(CNodeCompound* node_compound);
bool check_switch_breaks(CNodeSwitch* node);

//------------------------------------------------------------------------------

bool assign_method_type(CNodeFunction* func) {
  if (func->method_type != MT_UNKNOWN) return true;

  bool any_callers = !func->internal_callers.empty();
  bool any_callees = !func->internal_callees.empty();
  bool ext_callees = !func->external_callees.empty();
  bool has_return = func->has_return();
  bool has_params = !func->params.empty();
  bool self_writes_reg = false;
  bool self_writes_sig = false;
  bool tree_writes_reg = false;
  bool tree_writes_sig = false;
  bool calls_tick = false;
  bool calls_tock = false;
  bool calls_task = false;
  bool calls_func = false;

  for (auto w : func->self_writes) {
    if      (w->is_reg()) self_writes_reg = true;
    else if (w->is_sig()) self_writes_sig = true;
    else                  assert(false);
  }

  for (auto w : func->tree_writes) {
    if      (w->is_reg()) tree_writes_reg = true;
    else if (w->is_sig()) tree_writes_sig = true;
    else                  assert(false);
  }

  if (self_writes_reg && self_writes_sig) {
    LOG_R("Function %s writes both registers and signals\n", func->name.c_str());
    exit(-1);
  }

  if ((self_writes_reg || self_writes_sig) && has_return) {
    LOG_R("Function %s writes something and has a return value\n", func->name.c_str());
    exit(-1);
  }

  if ((tree_writes_reg || tree_writes_sig) && has_return) {
    LOG_R("Function %s indirectly writes something and has a return value\n", func->name.c_str());
    exit(-1);
  }

  for (auto c : func->internal_callees) {
    assert(c->method_type != MT_ALWAYS_COMB);
    assert(c->method_type != MT_ALWAYS_FF);
    if (c->method_type == MT_TASK_FF)   calls_tick = true;
    if (c->method_type == MT_TASK_COMB) calls_tock = true;
    if (c->method_type == MT_FUNC)      calls_func = true;
  }

  //----------------------------------------
  // Top-level functions can only be ALWAYS_FF or ALWAYS_COMB

  if (!any_callers && func->is_public) {
    if (self_writes_sig || tree_writes_sig || ext_callees) {
      func->set_type(MT_ALWAYS_COMB);
      return true;
    }

    if (!self_writes_reg && !self_writes_sig && !tree_writes_reg && !tree_writes_sig) {
      func->set_type(MT_ALWAYS_COMB);
      return true;
    }

    if (self_writes_reg || (tree_writes_reg && !tree_writes_sig)) {
      func->set_type(MT_ALWAYS_FF);
      return true;
    }

    assert(false);
  }

  //----------------------------------------
  // Non-top-level functions

  // FIXME we really should splice in output params to tasks so that tasks
  // can have returns

  bool all_callers_are_comb = true;
  bool any_callers_are_unknown = false;
  for (auto c : func->internal_callers) {
    all_callers_are_comb &= (c->method_type == MT_ALWAYS_COMB) || (c->method_type == MT_TASK_COMB);
    any_callers_are_unknown = c->method_type == MT_UNKNOWN;
  }

  if (self_writes_reg || tree_writes_reg) {
    if (any_callers_are_unknown) return false;

    // If this function lies on the ff side of the comb->ff boundary, it must
    // be always_ff.
    func->set_type(all_callers_are_comb ? MT_ALWAYS_FF : MT_TASK_FF);
    return true;
  }

  if (self_writes_sig || tree_writes_sig) {
    func->set_type(MT_TASK_COMB);
    return true;
  }

  func->set_type(MT_FUNC);
  return true;
}

//------------------------------------------------------------------------------

void assign_method_types(CNodeClass* node_class) {
  if (global_options.verbose) LOG("Assigning method types for %s\n", node_class->name.c_str());
  LOG_INDENT_SCOPE();

  std::set<CNodeClass*> submod_classes;
  for (auto f : node_class->all_fields) {
    if (auto submod_class = f->get_type_class()) {
      submod_classes.insert(submod_class);
    }
  }
  for (auto c : submod_classes) assign_method_types(c);


  auto& funcs = node_class->all_functions;

  for (auto f : funcs) assert(f->method_type == MT_UNKNOWN);
  if (node_class->constructor) assert(node_class->constructor->method_type == MT_UNKNOWN);

  //----------

  if (node_class->constructor) node_class->constructor->method_type = MT_INIT;

  for (int i = 0; i < 100; i++) {
    bool done = true;

    // We should get the same result regardless of assignment order.
    // How can we test that?

    for (int j = 0; j < funcs.size(); j++) {
      done &= assign_method_type(funcs[j]);
    }

    /*
    for (int j = funcs.size() - 1; j >= 0; j--) {
      done &= assign_method_type(funcs[j]);
    }
    */

    if (done) break;
  }

  //----------

  bool bad = false;
  for (auto f : funcs) {
    if (f->internal_callers.empty() && f->is_public) {
      if (f->method_type != MT_ALWAYS_COMB && f->method_type != MT_ALWAYS_FF) {
        LOG_R("Top-level function %s is not always_*\n", f->name.c_str());
        exit(-1);
      }
    }

    if (f->method_type == MT_UNKNOWN) {
      LOG_R("Couldn't categorize %s\n", f->name.c_str());
      assign_method_type(f);
      bad = true;
    }

    // All external callees must be always_comb or always_ff
    if (!f->external_callees.empty()) {
      for (auto c : f->external_callees) {
        assert(c->method_type == MT_ALWAYS_COMB || c->method_type == MT_ALWAYS_FF);
      }
    }
  }

  assert(!bad);
  if (global_options.verbose) LOG("Assigning method types for %s done\n", node_class->name.c_str());
}

//------------------------------------------------------------------------------

Err log_action2(CInstance* inst, CNode* node, TraceAction action) {
  Err err;

  //----------------------------------------
  // An action on a whole union applies to all parts of the union.

  if (auto inst_union = inst->as<CInstUnion>()) {
    for (auto child : inst_union->parts) {
      err << log_action2(child, node, action);
    }
    return err;
  }

  //----------------------------------------
  // An action on a whole struct applies to all parts of the struct.

  if (auto inst_struct = inst->as<CInstStruct>()) {
    for (auto child : inst_struct->parts) {
      err << log_action2(child, node, action);
    }
    return err;
  }

  //----------------------------------------
  // An action on a primitive gets logged

  if (auto inst_prim = inst->as<CInstPrim>()) {
    assert(action == ACT_READ || action == ACT_WRITE);

    auto old_state = inst_prim->state_stack.back();
    auto new_state = merge_action(old_state, action);

    inst_prim->state_stack.back() = new_state;

    if (new_state == TS_INVALID) {
      dump_parse_tree(node);
      LOG_R("Trace error: state went from %s to %s\n", to_string(old_state),
            to_string(new_state));
      err << ERR("Invalid context state\n");
    }

    return err;
  }

  assert(false);
  return err;
}

Err log_action(CInstance* inst, CNode* node, TraceAction action) {

  // If the instance is part of a union, apply the action to the whole union.

  std::function<CInstUnion*(CInstance*)> walk_up_unions;

  walk_up_unions = [&](CInstance* inst) -> CInstUnion* {
    if (inst->inst_parent) {
      if (auto parent_union = walk_up_unions(inst->inst_parent)) {
        return parent_union;
      }
    }

    if (auto inst_union = inst->as<CInstUnion>()) {
      return inst_union;
    }

    return nullptr;
  };

  if (auto inst_union = walk_up_unions(inst))
    return log_action2(inst_union, node, action);
  else
    return log_action2(inst, node, action);
}






















//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

int main_new() {
  Err err;

  CSourceRepo repo;

  repo.search_paths.insert(".");
  for (auto& path : global_options.inc_paths) {
    repo.search_paths.insert(path);
  }

  CSourceFile* root_file = nullptr;
  err << repo.load_source(global_options.src_name, &root_file);

  //exit(-1);

  if (!root_file) {
    LOG_R("Could not load %s\n", global_options.src_name.c_str());
    return -1;
  }

  for (auto file : repo.source_files) {
    file->link();
    file->update_transitive_deps();
  }

  sanity_check_parse_tree(repo);
  //dump_parse_tree(root_file->context.top_head);

  //----------------------------------------

  if (global_options.verbose) LOG_B("Count module instances so we can find top modules\n");

  for (auto file : repo.source_files) {
    for (auto c : file->all_classes) {
      for (auto f : c->all_fields) {
        auto type_class = f->node_decl->get_class();
        if (type_class) type_class->refcount++;
      }
    }
  }

  std::vector<CNodeClass*> tops;

  for (auto file : repo.source_files) {
    for (auto c : file->all_classes) {
      if (c->refcount == 0) {
        tops.push_back(c);
      }
    }
  }

  for (auto top : tops) {
    if (global_options.verbose) LOG_Y("Top module is %s\n", top->name.c_str());
  }

  //----------------------------------------

  if (global_options.verbose) LOG_B("Build call graph\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      err << build_call_graph(node_class, &repo);
    }
  }

  //----------------------------------------

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      if (global_options.verbose) LOG_S("Instantiating %s\n", node_class->name.c_str());
      LOG_INDENT_SCOPE();

      node_class->instance = instantiate_class(&repo, node_class->name, nullptr, nullptr, node_class, 1000);
    }
  }

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      if (global_options.verbose) LOG_S("Tracing %s\n", node_class->name.c_str());
      LOG_INDENT_SCOPE();

      Tracer tracer(&repo, node_class->instance);
      CNodeFunction* node_func = nullptr;

      Tracer::trace_cb callback = [&](CInstance* inst, CNode* node, TraceAction action) {
        if (inst->as<CInstFunc>()) return Err();

        inst = inst->get_root();

        if (action == ACT_READ) {
          assert(!inst->as<CInstClass>());
          node_class->self_reads.insert(inst);
          node_func->self_reads.insert(inst);
        } else if (action == ACT_WRITE) {
          node_class->self_writes.insert(inst);
          node_func->self_writes.insert(inst);
        }

        return Err();
      };

      if (node_class->constructor) {
        node_func = node_class->constructor;
        auto func_name = node_func->name;
        auto inst_func = node_class->instance->resolve(func_name);
        if (global_options.verbose) LOG_S("Tracing %s\n", func_name.c_str());
        LOG_INDENT_SCOPE();
        err << tracer.start_trace(inst_func, node_func, callback, /*deep_trace*/ false);
      }

      for (auto f : node_class->all_functions) {
        node_func = f;
        auto func_name = node_func->name;
        auto inst_func = node_class->instance->resolve(func_name);
        if (global_options.verbose) LOG_S("Tracing %s.%s\n", node_class->name.c_str(), func_name.c_str());
        LOG_INDENT_SCOPE();
        err << tracer.start_trace(inst_func, node_func, callback, /*deep_trace*/ false);
      }

      for (auto f : node_class->all_functions) {
        f->propagate_rw();
      }

      // RO
      for (auto inst : node_class->self_reads) {
        if (inst->get_owner() != node_class) continue;
        if (node_class->self_writes.contains(inst)) continue;
        if (!inst->is_public()) continue;

        if (inst->is_reg() && !inst->is_array()) {
          LOG_Y("Register %s is only read, never written!\n", inst->path.c_str());
          //exit(-1);
        }

        if (inst->is_sig()) {
          if (auto f = inst->get_root()->get_field()) node_class->input_sigs.insert(f);
        }
      }

      // RW
      for (auto inst : node_class->self_reads) {
        if (inst->get_owner() != node_class) continue;
        if (!node_class->self_writes.contains(inst)) continue;
        if (!inst->is_public()) continue;

        if (inst->is_sig()) {
          if (auto f = inst->get_root()->get_field()) node_class->output_sigs.insert(f);
        }
        if (inst->is_reg() && !inst->is_array()) {
          if (auto f = inst->get_root()->get_field()) node_class->output_regs.insert(f);
        }
      }

      // WO
      for (auto inst : node_class->self_writes) {
        if (inst->get_owner() != node_class) continue;
        if (node_class->self_reads.contains(inst)) continue;
        if (!inst->is_public()) continue;

        if (inst->is_sig()) {
          if (auto f = inst->get_root()->get_field()) node_class->output_sigs.insert(f);
        }

        if (inst->is_reg() && !inst->is_array()) {
          if (auto f = inst->get_root()->get_field()) node_class->output_regs.insert(f);
        }
      }
    }
  }

  //----------------------------------------

  for (auto top : tops) {
    if (global_options.verbose) LOG_B("Assign method types to %s\n", top->name.c_str());
    LOG_INDENT_SCOPE();
    assign_method_types(top);
  }

  //----------------------------------------

  if (global_options.verbose) LOG_B("Check for multiple returns\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto node_func : node_class->all_functions) {
        if (node_func->node_body && has_non_terminal_return(node_func->node_body)) {
          LOG_R("Function %s has non-terminal return\n", node_func->name.c_str());
          exit(-1);
        }
      }
    }
  }

  //----------------------------------------

  if (global_options.verbose) LOG_B("Check for mid-block break\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {

      node_visitor visit = [&](CNode* node) {
        if (auto node_switch = node->as<CNodeSwitch>()) {
          if (!check_switch_breaks(node_switch)) {
            dump_parse_tree(node_switch);
            LOG_R("Switch has cases that do not end with break\n");
            check_switch_breaks(node_switch);
            exit(-1);
          }
        }
      };

      visit_children(node_class, visit);
    }
  }

  //----------------------------------------

  if (global_options.verbose) LOG_B("Sort top methods for each module\n");

  LOG_INDENT();
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      auto& af = node_class->all_functions;
      auto& sf = node_class->sorted_functions;

      std::vector<CNodeFunction*> top_funcs;

      for (auto f : af) if (f->internal_callers.empty()) top_funcs.push_back(f);

      while (top_funcs.size()) {
        int head;
        for (int i = 0; i < top_funcs.size(); i++) {
          head = i;
          for (int j = 0; j < top_funcs.size(); j++) {
            if (i == j) continue;
            auto a = top_funcs[i];
            auto b = top_funcs[j];
            auto a_before_b = a->must_call_before(b);
            auto b_before_a = b->must_call_before(a);
            if (b_before_a) {
              head = -1;
              break;
            }
          }
          if (head != -1) {
            auto f = top_funcs[head];
            sf.push_back(f);
            top_funcs.erase(top_funcs.begin() + head);
            break;
          }
        }
        if (head == -1) {
          LOG_R("Top functions are unorderable\n");
          exit(-1);
        }
      }

      if (global_options.verbose) {
        LOG_S("Call order for %s:\n", node_class->name.c_str());
        LOG_INDENT_SCOPE();
        for (int i = 0; i < sf.size(); i++) {
          LOG_S("%02d: %s\n", i, sf[i]->name.c_str());
        }
      }
    }
  }
  LOG_DEDENT();

  //----------------------------------------

  if (global_options.verbose) LOG_B("Deep-tracing modules\n");
  LOG_INDENT();
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      if (global_options.verbose) LOG_B("Deep-tracing %s in sorted order\n", node_class->name.c_str());
      LOG_INDENT_SCOPE();

      Tracer tracer(&repo, node_class->instance);

      Tracer::trace_cb callback = [&](CInstance* inst, CNode* node, TraceAction action) {
        return log_action(inst, node, action);
      };

      for (auto node_func : node_class->sorted_functions) {
        auto func_name = node_func->name;
        auto inst_func = node_class->instance->resolve(func_name);
        if (global_options.verbose) LOG_S("Deep-tracing %s.%s\n", node_class->name.c_str(), func_name.c_str());
        LOG_INDENT_SCOPE();
        err << tracer.start_trace(inst_func, node_func, callback, /*deep_trace*/ true);
      }
    }
  }

  if (err) exit(-1);

  LOG_DEDENT();

  //----------------------------------------
  // Dump

  if (global_options.dump) {
    LOG_B("\n");
    LOG_B("========================================\n");
    LOG_B("Dump repo\n\n");

    dump_repo(&repo);

    LOG_B("\n");
    LOG_B("========================================\n");
    LOG_B("Dump instance tree\n\n");

    for (auto file : repo.source_files) {
      for (auto node_class : file->all_classes) {
        LOG_B("Instance tree for %s\n", node_class->name.c_str());
        LOG_INDENT();
        dump_inst_tree(node_class->instance);
        LOG_DEDENT();
        LOG_B("\n");
      }
    }

    LOG_B("\n");
    LOG_B("========================================\n");
    LOG_B("Dump call graphs\n\n");

    for (auto file : repo.source_files) {
      for (auto node_class : file->all_classes) {
        LOG_B("Call graph for %s\n", node_class->name.c_str());
        LOG_INDENT();
        dump_call_graph(node_class);
        LOG_DEDENT();
        LOG("\n");
      }
    }

    LOG("\n");
  }

  //----------------------------------------
  // Prep done, time to emit!

  LOG_B("========================================\n");
  LOG_B("Converting %s to SystemVerilog\n\n", global_options.src_name.c_str());

  std::string out_string;
  Cursor cursor(root_file, &out_string);
  cursor.echo = global_options.echo && !global_options.quiet;

  Emitter emitter(&repo, cursor);
  err << emitter.emit_everything();

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    return -1;
  }

  // Save translated source to output directory, if there is one.
  if (global_options.dst_name.size()) {
    auto dst_path = split_path(global_options.dst_name);
    dst_path.pop_back();
    mkdir_all(dst_path);

    FILE* out_file = fopen(global_options.dst_name.c_str(), "wb");
    if (!out_file) {
      LOG_R("ERROR Could not open %s for output\n", global_options.dst_name.c_str());
    } else {
      // Copy the BOM over if needed.
      if (root_file->use_utf8_bom) {
        uint8_t bom[3] = {239, 187, 191};
        fwrite(bom, 1, 3, out_file);
      }

      fwrite(out_string.data(), 1, out_string.size(), out_file);
      fclose(out_file);
    }

    {
      // Write deps file
      auto dst_dep_name = global_options.dst_name + ".d";
      FILE* out_file = fopen(dst_dep_name.c_str(), "wb");
      if (!out_file) {
      }
      else {
        fprintf(out_file, "%s: ", global_options.dst_name.c_str());
        fprintf(out_file, "%s", global_options.src_name.c_str());
        for (auto dep : root_file->deps) {
          fprintf(out_file, " \\\n %s", dep->filepath.c_str());
        }
        fprintf(out_file, "\n");
        fclose(out_file);
      }
    }
  }

  if (global_options.verbose) {
    LOG("\n");
    LOG_B("========================================\n");
    LOG_B("Output:\n\n");
    LOG_G("%s\n", out_string.c_str());
    LOG_B("========================================\n");
    LOG("\n");
  }

  LOG("Done!\n");

  LOG("");

  return 0;
}

//------------------------------------------------------------------------------

Err build_call_graph(CNodeClass* node, CSourceRepo* repo) {
  Err err;

  node_visitor link_callers = [&](CNode* child) {
    auto call = child->as<CNodeCall>();
    if (!call) return;

    auto src_method = call->ancestor<CNodeFunction>();

    auto func_name = call->child("func_name");

    if (auto submod_path = func_name->as<CNodeFieldExpression>()) {
      auto submod_field =
          node->get_field(submod_path->child("field_path")->get_text());
      auto submod_class = repo->get_class(submod_field->node_decl->node_type->name);
      auto submod_func = submod_class->get_function(
          submod_path->child("identifier")->get_text());

      src_method->external_callees.insert(submod_func);
      submod_func->external_callers.insert(src_method);
    } else if (auto func_id = func_name->as<CNodeIdentifier>()) {
      auto dst_method = node->get_function(func_id->get_text());
      if (dst_method) {
        src_method->internal_callees.insert(dst_method);
        dst_method->internal_callers.insert(src_method);
      }
    }
    else if(auto func_kw = func_name->as<CNodeKeyword>()) {
      // builtin like sizeof(), nothing to do here
    }
    else {
      dump_parse_tree(func_name);
      assert(false);
    }
  };

  if (node->constructor) {
    visit_children(node->constructor, link_callers);
  }

  for (auto src_method : node->all_functions) {
    visit_children(src_method, link_callers);
  }

  return err;
}

//------------------------------------------------------------------------------

void sanity_check_parse_tree(CSourceRepo& repo) {
  for (auto file : repo.source_files) {

    // Sanity check that all node spans in the repo tightly contain their
    // children
    node_visitor check_span = [](CNode* n) {
      if (n->child_head) {
        auto text = n->get_text();
        if (n->span.begin != n->child_head->span.begin) {
          LOG("%.*s\n", text.size(), text.data());
          assert(false);
        }
        if (n->span.end != n->child_tail->span.end) {
          LOG("%.*s\n", text.size(), text.data());
          assert(false);
        }
      }
    };

    visit_children(file->translation_unit, check_span);
  }
  if (global_options.verbose) LOG_G("Parse tree is sane\n");
}

//------------------------------------------------------------------------------
