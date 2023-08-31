#include "metron/nodes/MtModule.h"

#include <deque>

#include "metron/app/MtModLibrary.h"
#include "metron/app/MtSourceFile.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"
#include "metron/nodes/MtField.h"
#include "metron/nodes/MtFuncParam.h"
#include "metron/nodes/MtMethod.h"
#include "metron/nodes/MtModParam.h"
#include "metron/nodes/MtNode.h"
#include "metron/nodes/MtStruct.h"
#include "metron/tools/metron_tools.h"
#include "metron/tracer/MtContext.h"
#include "metron/tracer/MtTracer.h"

#include "tree-sitter/lib/include/tree_sitter/api.h"

//#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

//------------------------------------------------------------------------------

MtModule::~MtModule() {
  for (auto p : all_modparams) delete p;
  for (auto f : all_fields) delete f;
  for (auto e : all_enums) delete e;
  for (auto m : all_methods) delete m;
  for (auto i : input_method_params) delete i;

  delete ctx;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file, MnNode _node) {
  this->source_file = _source_file;
  this->lib = _source_file->lib;

  Err err;

  if (_node.sym == sym_template_declaration) {
    source_file = _source_file;
    lib = source_file->lib;
    mod_template = _node;
    root_node = _node;

    for (int i = 0; i < mod_template.child_count(); i++) {
      auto child = mod_template.child(i);

      if (child.sym == sym_template_parameter_list) {
        mod_param_list = MnNode(child);
      }

      if (child.sym == sym_class_specifier) {
        mod_class = MnNode(child);
      }
    }

    if (mod_param_list.is_null()) {
      err << ERR("No template parameter list found under template");
    }

    if (mod_class) {
      mod_name = mod_class.get_field(field_name).text();
    } else {
      err << ERR("No class node found under template\n");
    }
  } else if (_node.sym == sym_class_specifier) {
    source_file = _source_file;
    mod_template = MnNode::null;
    mod_param_list = MnNode::null;
    mod_class = _node;
    root_node = _node;

    if (mod_class) {
      mod_name = mod_class.get_field(field_name).text();
    } else {
      err << ERR("mod_class is null");
    }
  } else {
    err << ERR("MtModule::init() - Not sure what to do with %s\n",
               _node.ts_node_type());
  }

  return err;
}

//------------------------------------------------------------------------------

MtMethod *MtModule::get_method(const std::string &name) {
  for (auto n : all_methods)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_field(const std::string &name) {
  for (auto f : all_fields) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_component(const std::string &name) {
  for (auto f : all_fields) {
    if (!f->is_component()) continue;
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_enum(const std::string &name) {
  for (auto f : all_enums) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_input_signal(const std::string &name) {
  for (auto f : input_signals) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_output_signal(const std::string &name) {
  for (auto f : output_signals) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_output_register(const std::string &name) {
  for (auto f : output_registers) {
    if (f->name() == name) return f;
  }
  return nullptr;
}


bool MtModule::is_port(const std::string& name) const {
  for (auto p : input_signals) {
    if (p->name() == name) return true;
  }
  for (auto p : input_method_params) {
    if (p->name() == name) return true;
  }
  for (auto p : output_signals) {
    if (p->name() == name) return true;
  }
  for (auto p : output_registers) {
    if (p->name() == name) return true;
  }
  for (auto p : output_method_returns) {
    if (p->name() == name) return true;
  }
  return false;
}

//------------------------------------------------------------------------------

bool MtModule::needs_tick() const {
  for (auto m : all_methods) {
    if (m->is_tick_) return true;
  }

  for (auto f : all_fields) {
    if (f->_type_mod && f->_type_mod->needs_tick()) return true;
  }

  return false;
}

bool MtModule::needs_tock() const {
  for (auto m : all_methods) {
    if (m->is_tock_) return true;
    if (m->is_func_ && m->internal_callers.empty() && m->is_public()) return true;
  }

  for (auto f : all_fields) {
    if (f->_type_mod && f->_type_mod->needs_tock()) return true;
  }

  return false;
}

//------------------------------------------------------------------------------

// KCOV_OFF
void MtModule::dump_method_list(const std::vector<MtMethod *> &methods) const {
  for (auto n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n->name().c_str());

    if (n->param_nodes.size()) {
      int param_count = int(n->param_nodes.size());
      int param_index = 0;

      for (auto &param : n->param_nodes) {
        LOG_R("%s", param.text().c_str());
        if (param_index++ != param_count - 1) LOG_R(", ");
      }
    }
    LOG_R(")\n");
  }
}
// KCOV_ON

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_fields_and_methods() {
  Err err;

  if (!all_fields.empty()) return err << ERR("all_fields dirty\n");
  if (!all_methods.empty()) return err << ERR("all_methods dirty\n");

  if (mod_template) {
    auto mod_params = mod_template.get_field(field_parameters);

    for (auto child : mod_params) {
      if (child.sym == sym_parameter_declaration ||
          child.sym == sym_optional_parameter_declaration) {
        all_modparams.push_back(new MtModParam(this, child));
      }
    }
  }

  auto mod_body = mod_class.get_field(field_body);

  if (mod_body.is_null()) {
    err << ERR("Class body node is null\n");
    return err;
  }

  bool in_public = false;

  bool noconvert = false;
  bool dumpit = false;

  for (const auto &n : mod_body) {
    if (noconvert) {
      noconvert = false;
      continue;
    }

    if (dumpit) { n.dump_tree(); dumpit = false; }

    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym == sym_comment && n.contains("metron_internal")) {
      in_public = false;
    }

    if (n.sym == sym_comment && n.contains("metron_external")) {
      in_public = true;
    }

    if (n.sym == sym_field_declaration) {
      auto node_type = n.get_field(field_type);
      if (node_type.sym == sym_enum_specifier) {
        auto e = new MtField(this, n, in_public);
        all_enums.push_back(e);
      } else {
        auto new_field = new MtField(this, n, in_public);
        all_fields.push_back(new_field);
      }
    }

    if (n.sym == sym_comment && n.contains("metron_noconvert")) noconvert = true;
    if (n.sym == sym_comment && n.contains("dumpit"))    dumpit = true;

    if (n.sym == sym_function_definition) {
      all_methods.push_back(new MtMethod(this, n, in_public));
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::build_call_graph() {
  Err err;

  for (auto src_method : all_methods) {
    auto src_mod = this;

    src_method->_node.visit_tree([&](MnNode child) {
      if (child.sym != sym_call_expression) return;

      auto func = child.get_field(field_function);

      if (func.sym == sym_identifier) {
        auto dst_mod = this;
        auto dst_method = get_method(func.text());
        if (dst_method) {
          dst_method->internal_callers.insert(src_method);
          src_method->internal_callees.insert(dst_method);
        }
      }

      if (func.sym == sym_field_expression) {
        auto component_name = func.get_field(field_argument);
        auto component_method_name = func.get_field(field_field).text();

        auto component = get_field(component_name.name4());
        if (component) {
          auto dst_mod = source_file->lib->get_module(component->type_name());
          if (dst_mod) {
            auto dst_method = dst_mod->get_method(component_method_name);
            if (dst_method) {
              dst_method->external_callers.insert(src_method);
              src_method->external_callees.insert(dst_method);
            }
          }
        }
      }
    });
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::categorize_fields(bool verbose) {
  Err err;

  if (verbose) {
    LOG_G("Categorizing %s\n", name().c_str());
  }

  for (auto f : all_fields) {
    if (f->is_param()) {
      continue;
    }

    if (f->is_component()) {
      components.push_back(f);
    }
    else if (f->is_public() && f->is_input()) {
      input_signals.push_back(f);
    }
    else if (f->is_public() && f->is_signal()) {
      output_signals.push_back(f);
    }
    else if (f->is_public() && f->is_register()) {
      output_registers.push_back(f);
    }
    else if (f->is_private() && f->is_register()) {
      private_registers.push_back(f);
    }
    else if (f->is_private() && f->is_signal()) {
      private_signals.push_back(f);
    }
    else if (!f->is_public() && f->is_input()) {
      private_registers.push_back(f);
    }
    else if (f->is_enum()) {
    }
    else if (f->is_dead()) {
      dead_fields.push_back(f);
    }
    else {
      err << ERR("Don't know how to categorize %s = %s\n", f->cname(),
                 to_string(f->_state));
      f->error();
    }
  }

  return err;
}

//------------------------------------------------------------------------------

void MtModule::dump_module() {
  LOG_B("Module %s, %d instances\n", cname(), refcount);
  LOG_INDENT();

  {
    LOG_G("all_modparams:\n");
    LOG_INDENT_SCOPE();
    for (auto m : all_modparams) m->dump();
  }

  {
    LOG_G("all_enums:\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_enums) f->dump();
  }

  {
    LOG_G("all_methods:\n");
    LOG_INDENT_SCOPE();
    for (auto m : all_methods) m->dump();
  }

  {
    LOG_G("all_fields:\n");
    LOG_INDENT_SCOPE();
    for (auto f : all_fields) f->dump();
  }

  LOG_G("input_signals         %d\n", input_signals.size());
  LOG_G("output_signals        %d\n", output_signals.size());
  LOG_G("output_registers      %d\n", output_registers.size());

  LOG_G("input_method_params   %d\n", input_method_params.size());
  LOG_G("output_method_returns %d\n", output_method_returns.size());

  LOG_G("private_signals       %d\n", private_signals.size());
  LOG_G("private_registers     %d\n", private_registers.size());

  LOG_DEDENT();
  LOG("\n");
}

//------------------------------------------------------------------------------
