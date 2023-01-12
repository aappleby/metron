#include "MtModule.h"

#include <deque>

#include "Log.h"
#include "MtField.h"
#include "MtFuncParam.h"
#include "MtMethod.h"
#include "MtContext.h"
#include "MtModLibrary.h"
#include "MtModParam.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "MtStruct.h"
#include "metron_tools.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

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

/*
CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file, MnNode _node) {
  Err err;


  return err;
}
*/

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

MtField *MtModule::get_field(MnNode node) {
  if (node.sym == sym_subscript_expression) {
    return get_field(node.get_field(field_argument));
  }

  if (node.sym == sym_identifier || node.sym == alias_sym_field_identifier) {
    for (auto f : all_fields) {
      if (f->name() == node.text()) return f;
    }
    return nullptr;
  }
  else if (node.sym == sym_field_expression) {
    auto lhs = node.get_field(field_argument);
    auto rhs = node.get_field(field_field);

    auto lhs_field = get_field(lhs);
    if (lhs_field) {
      return lhs_field->get_field(rhs);
    } else {
      return nullptr;
    }
  }
  else {
    return nullptr;
  }
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

#if 0
MtField* MtModule::get_enum(const std::string &name) {
  for (auto n : all_enums) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

MtField *MtModule::get_input_field(const std::string &name) {
  for (auto f : input_signals)
    if (f->name() == name) return f;
  return nullptr;
}

MtFuncParam *MtModule::get_input_param(const std::string &name) {
  for (auto p : input_method_params)
    if (p->name() == name) return p;
  return nullptr;
}

MtField *MtModule::get_output_signal(const std::string &name) {
  for (auto n : output_signals)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_output_register(const std::string &name) {
  for (auto n : output_registers)
    if (n->name() == name) return n;
  return nullptr;
}

MtMethod *MtModule::get_output_return(const std::string &name) {
  for (auto n : output_method_returns)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_component(const std::string &name) {
  for (auto n : components) {
    if (n->name() == name) return n;
  }
  return nullptr;
}
#endif

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

// KCOV_OFF
void MtModule::dump_banner() const {
  LOG_Y("//----------------------------------------\n");
  if (mod_class.is_null()) {
    LOG_Y("// Package %s\n", source_file->full_path.c_str());
    LOG_Y("\n");
    return;
  }

  LOG_Y("// Module %s", mod_name.c_str());
  // if (parents.empty()) LOG_Y(" ########## TOP ##########");
  LOG_Y("\n");

  //----------

#if 0
  LOG_B("Modparams:\n");
  for (auto param : all_modparams) LOG_G("  %s\n", param->name().c_str());

  LOG_B("Localparams:\n");
  for (auto param : localparams) LOG_G("  %s\n", param->name().c_str());

  LOG_B("Enums:\n");
  for (auto n : all_enums) LOG_G("  %s\n", n->name().c_str());

  LOG_B("All Fields:\n");
  for (auto n : all_fields) {
    LOG_G("  %s:%s %s\n", n->name().c_str(), n->type_name().c_str(),
          to_string(n->state));
  }
#endif

  LOG_B("Input Fields:\n");
  for (auto n : input_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  /*
  LOG_B("Input Params:\n");
  for (auto n : input_method_params)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  */

  LOG_B("Output Signals:\n");
  for (auto n : output_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Registers:\n");
  for (auto n : output_registers) {
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  }

  /*
  LOG_B("Output Returns:\n");
  for (auto n : output_method_returns)
    LOG_G("  %s:%s\n", n->name().c_str(),
          n->_node.get_field(field_type).text().c_str());
  */

#if 0
  /*
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  */

  LOG_B("Components:\n");
  for (auto component : components) {
    auto component_mod = source_file->lib->get_module(component->type_name());
    LOG_G("  %s:%s\n", component->name().c_str(),
          component_mod->mod_name.c_str());
  }

  //----------

  LOG_B("Init methods:\n");
  dump_method_list(init_methods);

  LOG_B("Tick methods:\n");
  dump_method_list(tick_methods);

  LOG_B("Tock methods:\n");
  dump_method_list(tock_methods);

  LOG_B("Tasks:\n");
  dump_method_list(task_methods);

  LOG_B("Functions:\n");
  dump_method_list(func_methods);

  //----------

  /*
  LOG_B("Port map:\n");
  for (auto &kv : port_map)
    LOG_G("  %s = %s\n", kv.first.c_str(), kv.second.c_str());
  */

  /*
  LOG_B("State map:\n");
  for (auto &kv : mod_states.get_map())
    LOG_G("  %s = %s\n", kv.first.c_str(), to_string(kv.second));
  */
#endif
  LOG_B("\n");
}
// KCOV_ON

//------------------------------------------------------------------------------

// KCOV_OFF
#if 0
void MtModule::dump_deltas() const {
  if (mod_struct.is_null()) return;

  LOG_G("%s\n", mod_name.c_str());
  {
    LOG_INDENT_SCOPE();

    for (auto tick : tick_methods) {
      LOG_G("%s error %d\n", tick->name().c_str(), tick->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tick->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tick->delta->state_new.contains(s.first)) {
            auto s2 = tick->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }

    for (auto tock : tock_methods) {
      LOG_G("%s error %d\n", tock->name().c_str(), tock->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tock->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tock->delta->state_new.contains(s.first)) {
            auto s2 = tock->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }
  }
  LOG_G("\n");
}
#endif
// KCOV_ON

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_parts() {
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
      n.dump_tree();
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
        e->_is_enum = true;
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

#if 0

int MtMethod::categorize() {
  int changes = 0;

  if (in_init) return 0;


  // Methods that only call ticks and are only called by tocks could be either a tick or a tock.
  // Assume they are tocks.
  if (fields_written.empty() && !categorized() && !callers.empty() && !callees.empty()) {
    bool only_called_by_tocks = !callers.empty();
    bool only_calls_ticks_or_funcs = !callees.empty();
    for (auto caller : callers) {
      if (!caller->in_tock) only_called_by_tocks = false;
    }
    for (auto callee : callees) {
      if (!callee->in_tick && !callee->in_func) only_calls_ticks_or_funcs = false;
    }
    if (only_called_by_tocks && only_calls_ticks_or_funcs && !in_tock) {
      in_tock = true;
      changes++;
    }
  }

  /*
  if (fields_written.empty() && !in_tock) {
    LOG_B("%-20s is tock because it doesn't call or write anything.\n", cname());
    in_tock = true;
    changes++;
  }
  */

  /*
  // Methods that call a tock are tocks.
  for (auto& ref : callees) {
    if (ref.method->in_tock && !in_tock) {
      LOG_B("%-20s is tock because it calls tock %s.\n", cname(), ref.method->cname());
      in_tock = true;
      changes++;
    }
  }
  */

  /*
  // Methods that call a tick that has parameters are tocks.
  for (auto& ref : callees) {
    if (ref.method->in_tick && ref.method->params.size() && !in_tock) {
      LOG_B("%-20s is tock because it calls tick with params %s.\n", cname(), ref.method->cname());
      in_tock = true;
      changes++;
    }
  }
  */

  /*
  // Methods called by a tick are ticks.
  for (auto& ref : callers) {
    if (ref.method->in_tick && !in_tick) {
      LOG_B("%-20s is tick because it calls tick %s.\n", cname(), ref.method->cname());
      in_tick = true;
      changes++;
    }
  }
  */

  /*
  // Methods that write a signal are tocks.
  for (auto ref : fields_written) {
    auto f = ref.subfield ? ref.subfield : ref.field;
    if (f->state == CTX_SIGNAL && !in_tock) {
      LOG_B("%-20s is tock because it writes signal %s.\n", cname(), f->cname());
      in_tock = true;
      changes++;
    }
  }

  // Methods that write a register are ticks.
  for (auto ref : fields_written) {
    auto f = ref.subfield ? ref.subfield : ref.field;
    if (f->state == CTX_REGISTER && !in_tick) {
      in_tick = true;
      changes++;
    }
  }
  */

  /*
  // Methods that only write outputs are tocks.
  {
    bool only_writes_outputs = !fields_written.empty();
    for (auto ref : fields_written) {
      if (ref.subfield) {
        if (ref.subfield->state != CTX_OUTPUT) only_writes_outputs = false;
      }
      else if (ref.field) {
        if (ref.field->state != CTX_OUTPUT) only_writes_outputs = false;
      }
    }
    if (only_writes_outputs && !in_tock) {
      in_tock = true;
      changes++;
    }
  }
  */



  return changes;
}

#endif

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::build_call_graph() {
  Err err;

  for (auto src_method : all_methods) {
    auto src_mod = this;

    src_method->_node.visit_tree([&](MnNode child) {
      if (child.sym == sym_call_expression) {
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

          auto component = get_field(component_name);
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
      // localparams.push_back(MtFuncParam::construct("<localparam>", f->node));
      continue;
    }

    if (f->is_component()) {
      components.push_back(f);
    }
    else if (f->is_public_input()) {
      input_signals.push_back(f);
    }
    else if (f->is_public_signal()) {
      output_signals.push_back(f);
    }
    else if (f->is_public_register()) {
      output_registers.push_back(f);
    }
    else if (f->is_private_register()) {
      private_registers.push_back(f);
    }
    else if (f->is_private_signal()) {
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
      f->_node.error();
    }
  }

  return err;
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

#if 0
CHECK_RETURN Err MtModule::build_port_map() {
  assert(port_map.empty());

  Err err;

  //----------------------------------------
  // Assignments to component fields bind ports.

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_assignment_expression) return;

    auto node_lhs = child.get_field(field_left);
    auto node_rhs = child.get_field(field_right);

    if (node_lhs.sym == sym_field_expression) {
      auto component_name = node_lhs.get_field(field_argument).text();
      if (get_component(component_name)) {
        port_map[node_lhs.text()] = node_rhs.text();
      }
    }
  });

  //----------------------------------------
  // Calls to methods bind ports.

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_call_expression) return;
    if (child.get_field(field_function).sym != sym_field_expression) return;

    auto node_call = child;
    auto node_func = node_call.get_field(field_function);
    auto node_args = node_call.get_field(field_arguments);

    MtField *call_member = nullptr;
    MtModule *call_component = nullptr;
    MtMethod *call_method = nullptr;

    if (node_func.sym == sym_field_expression) {
      auto node_member = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);
      if (node_method.text() == "as_signed") {
        return;
      }
    }

    if (node_func.sym == sym_field_expression) {
      auto node_this = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);

      if (node_method.text() == "as_signed") {
      } else {
        call_member = get_component(node_this.text());
        if (call_member) {
          call_component = source_file->lib->get_module(call_member->type_name());
          call_method = call_component->get_method(node_method.text());
        } else {
          err << ERR("Component %s not found!\n", node_this.text().c_str());
          return;
        }
      }
    }

    auto arg_count = node_args.named_child_count();

    for (auto i = 0; i < arg_count; i++) {
      auto key = call_member->name() + "." + call_method->params[i];

      std::string val;
      MtCursor cursor(source_file->lib, source_file, this, &val);
      auto arg_node = node_args.named_child(i);
      cursor.cursor = arg_node.start();
      err << cursor.emit_dispatch(arg_node);

      auto it = port_map.find(key);
      if (it != port_map.end()) {
        if ((*it).second != val) {
        }
      } else {
        port_map.insert({key, val});
      }
    }
  });

  //----------------------------------------
  // Port mapping walk done.

  // Verify that all input ports of all components are bound.

  for (const auto& component : all_components) {
    auto component_mod = source_file->lib->get_module(component->type_name());

    for (int i = 0; i < component_mod->input_signals.size(); i++) {
      auto key = component->name() + "." + component_mod->input_signals[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to component port %s\n", key.c_str());
      }
    }

    for (int i = 0; i < component_mod->input_method_params.size(); i++) {
      auto key = component->name() + "." + component_mod->input_method_params[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to component port %s\n", key.c_str());
      }
    }
  }

  return err;
}
#endif

//------------------------------------------------------------------------------

void MtModule::dump() {
  LOG_G("Module %s, %d instances\n", cname(), refcount);
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

#if 0
  // LOG_G("constructor %p\n", constructor);

  LOG_G("inputs                %d\n", inputs.size());
  LOG_G("outputs               %d\n", outputs.size());
  LOG_G("signals               %d\n", signals.size());
  LOG_G("registers             %d\n", registers.size());
  LOG_G("components            %d\n", components.size());
#endif

  LOG_G("localparams           %d\n", localparams.size());
  LOG_G("input_signals         %d\n", input_signals.size());
  LOG_G("output_signals        %d\n", output_signals.size());
  LOG_G("output_registers      %d\n", output_registers.size());

  LOG_G("input_method_params   %d\n", input_method_params.size());
  LOG_G("output_method_returns %d\n", output_method_returns.size());

  LOG_G("private_signals       %d\n", private_signals.size());
  LOG_G("private_registers     %d\n", private_registers.size());

#if 0
  LOG_G("init_methods      %d\n", init_methods.size());
  LOG_G("tick_methods      %d\n", tick_methods.size());
  LOG_G("tock_methods      %d\n", tock_methods.size());
  LOG_G("task_methods      %d\n", task_methods.size());
  LOG_G("func_methods      %d\n", func_methods.size());
#endif

  LOG_DEDENT();
  LOG("\n");
}

//------------------------------------------------------------------------------
