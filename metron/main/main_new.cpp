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

  LOG_B("Count module instances so we can find top modules\n");

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

  if (top) LOG_Y("Top module is %s\n", top->name.c_str());

  //----------------------------------------

  LOG_B("Instantiate individual classes\n");
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      node_class->instance = instantiate_class(
        &repo, node_class->name, nullptr, nullptr, node_class, 1000);
    }
  }

  //----------------------------------------

  LOG_B("Build call graph\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      err << build_call_graph(node_class, &repo);
    }
  }

  //----------------------------------------

  LOG_B("Shallow-trace individual classes to collect reads/writes\n");

  LOG_INDENT();
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      Tracer tracer(&repo, node_class->instance, /*deep_trace*/ false, /*log_actions*/ false);

      auto name = node_class->name;
      LOG_B("Tracing public methods in %.*s\n", int(name.size()), name.data());

      // Trace constructors first
      if (auto node_func = node_class->constructor) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->name;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = node_class->instance->resolve(func_name);

        err << tracer.start_trace(inst_func, node_func);
      }

      for (auto node_func : node_class->all_functions) {
        LOG_INDENT_SCOPE();
        auto func_name = node_func->name;
        if (!node_func->is_public) continue;

        LOG_B("Tracing %s\n", func_name.c_str());
        auto inst_func = node_class->instance->resolve(func_name);

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

  LOG_B("Assign method types\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {

      for (auto node_func : node_class->all_functions) {
        assert(node_func->method_type == MT_UNKNOWN);
      }

      for (auto node_func : node_class->all_functions) {
        if (node_func->is_public) node_func->update_type();
      }

      if (node_class->constructor) node_class->constructor->update_type();

      for (auto node_func : node_class->all_functions) {
        if (node_func->method_type != MT_UNKNOWN) {
        }
      }
    }
  }

  //----------------------------------------

  LOG_B("Check for ticks with return values\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      for (auto node_func : node_class->all_functions) {
        if (node_func->method_type == MT_TICK && node_func->has_return()) {
          LOG_R("Tick method %s has a return value and it shouldn't\n", node_func->name.c_str());
          exit(-1);
        }
      }
    }
  }

  //----------------------------------------

  LOG_B("Check for multiple returns\n");

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

  LOG_B("Check for mid-block break\n");

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {

      node_visitor visit = [&](CNode* node) {
        if (auto node_switch = node->as<CNodeSwitch>()) {
          if (!check_switch_breaks(node_switch)) {
            LOG_R("Switch has cases that do not end with break\n");
            exit(-1);
          }
        }
      };

      visit_children(node_class, visit);
    }
  }

  //----------------------------------------

  LOG_B("Sort top methods for each module\n");

  LOG_INDENT();
  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      auto& af = node_class->all_functions;
      auto& sf = node_class->sorted_functions;

      for (auto f : af) {
        if (f->internal_callers.size()) continue;

        bool inserted = false;
        for (auto it = sf.begin(); it != sf.end(); ++it) {
          auto s = *it;
          assert(f != s);

          auto f_before_s = f->must_call_before(s);
          auto s_before_f = s->must_call_before(f);
          assert(!f_before_s || !s_before_f);

          if (f_before_s) {
            inserted = true;
            sf.insert(it, f);
            break;
          }
        }

        if (!inserted) sf.push_back(f);
      }

      LOG_S("Call order for %s:\n", node_class->name.c_str());
      LOG_INDENT_SCOPE();
      for (int i = 0; i < sf.size(); i++) {
        LOG_S("%02d: %s\n", i, sf[i]->name.c_str());
      }
    }
  }
  LOG_DEDENT();

  //----------------------------------------

  if (top) {
    LOG_B("Deep-tracing top module\n");
    LOG_INDENT();

    for (auto f : top->sorted_functions) {
      LOG_B("Tracing %s\n", f->name.c_str());

      Tracer tracer(&repo, top->instance, /*deep_trace*/ true, /*log_actions*/ true);
      auto inst_func = top->instance->resolve(f->name);
      err << tracer.start_trace(inst_func, f);

      if (err.has_err()) {
        LOG_R("Error during tracing\n");
        return -1;
      }
    }

    LOG_DEDENT();
  }

  //----------------------------------------

  LOG_B("Checking port compatibility\n");
  LOG_INDENT();

  for (auto file : repo.source_files) {
    for (auto node_class : file->all_classes) {
      LOG("Module %s\n", node_class->instance->name.c_str());
      LOG_INDENT_SCOPE();
      for (auto child : node_class->instance->parts) {
        if (auto inst_a = child->as<CInstClass>()) {
          auto inst_b = inst_a->node_class->instance;
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
  // Dump

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
  LOG_G("Parse tree is sane\n");
}
