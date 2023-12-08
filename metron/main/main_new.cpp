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

#include "tests/metron_good/bitfields.h"

using namespace matcheroni;
namespace fs = std::filesystem;

CNodeField* resolve_field(CNodeClass* node_class, CNode* node_name);
void sanity_check_parse_tree(CSourceRepo& repo);
Err build_call_graph(CNodeClass* node, CSourceRepo* repo);


//------------------------------------------------------------------------------

MethodType get_method_type(CNodeFunction* node) {
  if (node->called_by_init()) return MT_INIT;
  if (node->name.starts_with("tick")) return MT_TICK;
  if (node->name.starts_with("tock")) return MT_TOCK;

  auto result = MT_UNKNOWN;

  /*
  if (node->all_writes.empty()) {
    return MT_FUNC;
  }
  */

  for (auto c : node->internal_callers) {
    auto new_result = get_method_type(c);
    if (new_result == MT_UNKNOWN) {
      LOG_R("%s: get_method_type returned MT_UNKNOWN\n", node->name.c_str());
      assert(false);
    }

    if (result == MT_UNKNOWN) {
      result = new_result;
    }
    else if (result != new_result) {
      LOG_R("%s: get_method_type inconsistent\n", node->name.c_str());
      assert(false);
    }
  }

  if (result != MT_UNKNOWN) return result;

  // Function was not a tick or tock, and was not called by a tick or tock.

  if (node->self_writes.size()) {
    LOG_R("Function %s writes stuff but is not tick or tock\n", node->name.c_str());
    assert(false);
  }

  return result;
}


//------------------------------------------------------------------------------

int main_new(Options opts) {
  LOG_G("New parser!\n");

  Err err;

  CSourceRepo repo;

  repo.search_paths.insert(fs::canonical("."));
  if (!opts.inc_path.empty()) {
    repo.search_paths.insert(fs::canonical(opts.inc_path));
  }

  CSourceFile* root_file = nullptr;
  err << repo.load_source(opts.src_name, &root_file);

  if (!root_file) {
    LOG_R("Could not load %s\n", opts.src_name.c_str());
    return -1;
  }

  for (auto file : repo.source_files) {
    file->link();
  }

  sanity_check_parse_tree(repo);
  //dump_parse_tree(root_file->context.top_head);

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  // Count module instances so we can find top modules.

  for (auto c : root_file->all_classes) {
    for (auto f : c->all_fields) {
      auto type_class = f->node_decl->get_class();
      if (type_class) type_class->refcount++;
    }
  }

  CNodeClass* top = nullptr;
  for (auto c : root_file->all_classes) {
    if (c->refcount == 0) {
      assert(top == nullptr);
      top = c;
    }
  }

  //----------------------------------------

  LOG_B("Instantiating modules\n");
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      auto instance = instantiate_class(&repo, node_class->name, nullptr,
                                        nullptr, node_class, 1000);
      file->all_instances.push_back(instance);
    }
  }

  //----------------------------------------

  LOG_B("Building call graph\n");
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      err << build_call_graph(node_class, &repo);
    }
  }

  /*
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto node_func : node_class->all_functions) {
        if (node_func->method_type != MT_FUNC && node_func->is_public &&
            node_func->internal_callers.size()) {
          LOG_R("Function %s is public and has internal callers\n",
                node_func->name.c_str());
          assert(false);
        }
        // err << node_class->build_call_graph(&repo);
      }
    }
  }
  */

  //----------------------------------------
  // Trace

  LOG_B("Tracing classes\n");
  LOG_INDENT();

  Tracer tracer;
  tracer.repo = &repo;

  for (auto file : repo.source_files) {
    for (auto inst_class : file->all_instances) {
      tracer.root_inst = inst_class;
      auto node_class = inst_class->node_class;
      auto name = node_class->name;
      LOG_B("Tracing public methods in %.*s\n", int(name.size()), name.data());

      // Trace constructors first
      if (auto node_func = node_class->constructor) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->name;
        // if (!node_func->is_public) continue;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = inst_class->resolve(func_name);

        err << tracer.start_trace(inst_func, node_func);
      }

      for (auto node_func : node_class->all_functions) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->name;
        if (!node_func->is_public) continue;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = inst_class->resolve(func_name);

        err << tracer.start_trace(inst_func, node_func);
      }
    }
  }

  if (err.has_err()) {
    LOG_R("Error during tracing\n");
    return -1;
  }

  LOG_DEDENT();




  //----------------------------------------
  // Assign method types

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto node_func : node_class->all_functions) {
        /*
        LOG_G("func %s\n", node_func->name.c_str());

        if (node_func->all_writes.empty()) {
          node_func->set_type(MT_FUNC);
        }
        else {
          for (auto write : node_func->all_writes) {
            if (write->name.ends_with("_")) {
              node_func->set_type(MT_TICK);
              LOG_G("write state %s\n", write->name.c_str());
            }
            else {
              node_func->set_type(MT_TOCK);
              LOG_G("write signal %s\n", write->name.c_str());
            }
          }
        }
        */
      }
    }
  }





  //----------------------------------------
  // Dump

  LOG_B("\n");
  LOG_B("========================================\n");
  LOG_B("Repo dump\n\n");

  dump_repo(&repo);

  LOG_B("\n");
  LOG_B("========================================\n");
  LOG_B("Instance tree\n\n");

  for (auto file : repo.source_files) {
    for (auto inst_class : file->all_instances) {
      LOG_B("Instance tree for %s\n",
            inst_class->node_class->name.c_str());
      LOG_INDENT();
      dump_inst_tree(inst_class);
      LOG_DEDENT();
      LOG_B("\n");
    }
  }

  LOG_B("\n");
  LOG_B("========================================\n");
  LOG_B("Call graphs\n\n");

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

#if 0

  // assert(top);

  //----------------------------------------
  // Top methods that are not ticks or tocks must not write anything

  //----------------------------------------
  // Check for ticks with return values

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto node_func : node_class->all_functions) {
        auto func_name = node_func->name;
        if (func_name.starts_with("tick")) {
          if (node_func->has_return()) {
            LOG_R("Tick %s has a return value and it shouldn't\n",
                  func_name.c_str());
            exit(-1);
          }
        }
      }
    }
  }

  //----------------------------------------

  LOG_B("Tracing top module\n");
  LOG_INDENT();

  CInstClass* top_inst = nullptr;

  if (top) {
    top_inst =
        instantiate_class(&repo, top->name, nullptr, nullptr, top, 1000);
    LOG_INDENT();
    LOG_G("Top instance is %s\n", top->name.c_str());
    LOG_DEDENT();
  }

  tracer.root_inst = top_inst;
  tracer.deep_trace = true;

  if (top_inst) {
    // Trace constructors first
    if (auto node_func = top->constructor) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->name;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);

      err << tracer.start_trace(inst_func, node_func);
    }

    // Trace tocks
    for (auto node_func : top->all_functions) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->name;
      if (!func_name.starts_with("tock")) continue;
      if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);

      err << tracer.start_trace(inst_func, node_func);
    }

    // Trace ticks
    for (auto node_func : top->all_functions) {
      LOG_INDENT_SCOPE();
      auto func_name = node_func->name;
      if (!func_name.starts_with("tick")) continue;
      if (!node_func->is_public) continue;

      LOG_B("Tracing %s\n", func_name.c_str());
      auto inst_func = top_inst->resolve(func_name);

      err << tracer.start_trace(inst_func, node_func);
    }
  }

  tracer.deep_trace = false;

  if (err.has_err()) {
    LOG_R("Error during tracing\n");
    return -1;
  }
  LOG_DEDENT();

  //----------------------------------------

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto f : node_class->all_functions) {
        auto method_type = get_method_type(f);

        if (method_type != MT_FUNC && method_type != MT_TICK &&
            method_type != MT_TOCK && method_type != MT_INIT) {
          assert(false);
        }

        f->method_type = method_type;

        if (method_type == MT_FUNC) {
          assert(f->all_writes.empty());
        } else if (method_type == MT_TOCK) {
          for (auto inst : f->self_writes) {
            auto state = inst->get_state();
            if (state == TS_SIGNAL || state == TS_OUTPUT) {
            } else {
              LOG_R("Tock writes a non-signal\n");
              LOG("");
              exit(-1);
            }
          }
        } else if (method_type == MT_TICK) {
          for (auto inst : f->self_writes) {
            auto state = inst->get_state();
            if (state == TS_REGISTER || state == TS_MAYBE || state == TS_OUTPUT) {
            } else {
              LOG_R("Tick writes a non-register\n");
              LOG("");
              exit(-1);
            }
          }
        }
      }
    }
  }

  //----------------------------------------

  LOG_B("Checking port compatibility\n");
  LOG_INDENT();

  for (auto file : repo.source_files) {
    for (auto inst : file->all_instances) {
      LOG("Module %s\n", inst->name.c_str());
      LOG_INDENT_SCOPE();
      for (auto child : inst->parts) {
        if (auto inst_a = child->as<CInstClass>()) {
          auto inst_b = repo.get_instance(inst_a->node_class->name);
          assert(inst_b);
          bool ports_ok = inst_a->check_port_directions(inst_b);
          if (!ports_ok) {
            LOG_R("-----\n");
            dump_inst_tree(inst_a);
            LOG_R("-----\n");
            dump_inst_tree(inst_b);
            LOG_R("-----\n");
            err << ERR("Bad ports!\n");
            exit(-1);
          }
        }
      }
    }
  }
  LOG_DEDENT();

  //----------------------------------------

  LOG_B("Categorizing fields\n");

  for (auto file : repo.source_files) {
    for (auto inst : file->all_instances) {
      auto inst_class = inst->as<CInstClass>();
      auto node_class = inst_class->node_class;
      assert(inst_class);

      for (auto inst_child : inst_class->ports) {
        if (auto inst_prim = inst_child->as<CInstPrim>()) {
          auto field = inst_prim->node_field;
          if (!field->is_public) continue;
          if (field->node_decl->is_param())
            continue;

          switch (inst_prim->get_state()) {
            case TS_NONE:
              break;
            case TS_INPUT:
              node_class->input_signals.push_back(field);
              break;
            case TS_OUTPUT:
              node_class->output_signals.push_back(field);
              break;
            case TS_MAYBE:
              node_class->output_registers.push_back(field);
              break;
            case TS_SIGNAL:
              node_class->output_signals.push_back(field);
              break;
            case TS_REGISTER:
              node_class->output_registers.push_back(field);
              break;
            case TS_INVALID:
              assert(false);
              break;
            case TS_PENDING:
              assert(false);
              break;
          }
        }

        auto inst_struct = inst_child->as<CInstStruct>();
        auto inst_union  = inst_child->as<CInstUnion>();

        if (inst_struct || inst_union) {
          CNodeField* field = nullptr;
          TraceState state = TS_INVALID;

          if (inst_struct) {
            field = inst_struct->node_field;
            state = inst_struct->get_state();
          }

          if (inst_union) {
            field = inst_union->node_field;
            state = inst_union->get_state();
          }

          if (!field->is_public) continue;
          if (field->node_decl->is_param())
            continue;

          switch (state) {
            case TS_NONE:
              break;
            case TS_INPUT:
              node_class->input_signals.push_back(field);
              break;
            case TS_OUTPUT:
              node_class->output_signals.push_back(field);
              break;
            case TS_MAYBE:
              node_class->output_registers.push_back(field);
              break;
            case TS_SIGNAL:
              node_class->output_signals.push_back(field);
              break;
            case TS_REGISTER:
              node_class->output_registers.push_back(field);
              break;
            case TS_INVALID:
              assert(false);
              break;
            case TS_PENDING:
              assert(false);
              break;
          }
        }
      }
    }
  }

  //----------------------------------------
  // Done!

  LOG_B("========================================\n");
  LOG_B("Converting %s to SystemVerilog\n\n", opts.src_name.c_str());

  std::string out_string;
  Cursor cursor(root_file, &out_string);
  cursor.echo = opts.echo && !opts.quiet;

  Emitter emitter(&repo, cursor);
  err << emitter.emit_everything();

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    // lib.teardown();
    return -1;
  }

  // FIXME - save .d deps file too

  // Save translated source to output directory, if there is one.
  if (opts.dst_name.size()) {
    auto dst_path = split_path(opts.dst_name);
    dst_path.pop_back();
    mkdir_all(dst_path);

    FILE* out_file = fopen(opts.dst_name.c_str(), "wb");
    if (!out_file) {
      LOG_R("ERROR Could not open %s for output\n", opts.dst_name.c_str());
    } else {
      // Copy the BOM over if needed.
      if (root_file->use_utf8_bom) {
        uint8_t bom[3] = {239, 187, 191};
        fwrite(bom, 1, 3, out_file);
      }

      fwrite(out_string.data(), 1, out_string.size(), out_file);
      fclose(out_file);
    }
  }
  LOG("\n");

  LOG_B("========================================\n");
  LOG_B("Output:\n\n");

  LOG_G("%s\n", out_string.c_str());

  LOG_B("========================================\n");

  LOG("Done!\n");
#endif

  return 0;
}













template<typename T>
using permute_callback = std::function<void(std::vector<T>&)>;

template<typename T>
void permute(std::vector<T>& a, permute_callback<T> pc) {
  auto N = a.size();
  std::vector<int> p(N + 1, 0);

  pc(a);

  int i = 1;
  while (i < N) {
    if (p[i] < i) {
      int j = (i & 1) ? p[i] : 0;
      std::swap(a[j], a[i]);

      pc(a);

      p[i]++;
      i = 1;
    }
    else {
      p[i] = 0;
      i++;
    }
  }
}


//------------------------------------------------------------------------------

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
  LOG("\n");
}
