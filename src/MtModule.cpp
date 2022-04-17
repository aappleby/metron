#include "MtModule.h"

#include "Log.h"
#include "MtCursor.h"
#include "MtModLibrary.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "metron_tools.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

//------------------------------------------------------------------------------

bool MtField::is_component() const {
  return (node.source->lib->get_module(type_name()));
}

//------------------------------------------------------------------------------

MtModule::MtModule() {
}

CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file, MnTemplateDecl _node) {
  Err err;

  source_file = _source_file;
  mod_template = _node;

  for (int i = 0; i < mod_template.child_count(); i++) {
    auto child = mod_template.child(i);

    if (child.sym == sym_template_parameter_list) {
      mod_param_list = MnTemplateParamList(child);
    }

    if (child.sym == sym_class_specifier) {
      mod_class = MnClassSpecifier(child);
    }
  }

  if (mod_param_list.is_null()) {
    err << ERR("No template parameter list found under template");
  }

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  }
  else {
    err << ERR("No class node found under template");
  }


  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file, MnClassSpecifier _node) {
  Err err;

  source_file = _source_file;
  mod_template = MnNode::null;
  mod_param_list = MnNode::null;
  mod_class = _node;

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  }
  else {
    err << ERR("mod_class is null");
  }

  return err;
}

//------------------------------------------------------------------------------

MtMethod *MtModule::get_method(const std::string &name) {
  for (auto n : all_methods)
    if (n->name() == name) return n;
  return nullptr;
}

MtField* MtModule::get_enum(const std::string &name) {
  for (auto n : all_enums) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

MtField *MtModule::get_field(const std::string &name) {
  for (auto f : all_fields) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_input_field(const std::string &name) {
  for (auto f : input_signals)
    if (f->name() == name) return f;
  return nullptr;
}

MtParam *MtModule::get_input_param(const std::string &name) {
  for (auto p : input_arguments)
    if (p->name() == name) return p;
  return nullptr;
}

MtField *MtModule::get_output_signal(const std::string &name) {
  for (auto n : output_signals)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_output_register(const std::string &name) {
  for (auto n : public_registers)
    if (n->name() == name) return n;
  return nullptr;
}

MtMethod *MtModule::get_output_return(const std::string &name) {
  for (auto n : output_returns)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_component(const std::string &name) {
  for (auto n : all_components) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModule::dump_method_list(const std::vector<MtMethod *> &methods) const {
  for (auto n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n->name().c_str());

    if (n->params.size()) {
      int param_count = int(n->params.size());
      int param_index = 0;

      for (auto &param : n->params) {
        LOG_R("%s", param.c_str());
        if (param_index++ != param_count - 1) LOG_R(", ");
      }
    }
    LOG_R(")\n");
  }
}

//------------------------------------------------------------------------------

void MtModule::dump_banner() const {
  LOG_Y("//----------------------------------------\n");
  if (mod_class.is_null()) {
    LOG_Y("// Package %s\n", source_file->full_path.c_str());
    LOG_Y("\n");
    return;
  }

  LOG_Y("// Module %s", mod_name.c_str());
  if (parents.empty()) LOG_Y(" ########## TOP ##########");
  LOG_Y("\n");

  //----------

  LOG_B("Modparams:\n");
  for (auto param : modparams) LOG_G("  %s\n", param->name().c_str());
  
  LOG_B("Localparams:\n");
  for (auto param : localparams) LOG_G("  %s\n", param->name().c_str());
  
  LOG_B("Enums:\n");
  for (auto n : all_enums) LOG_G("  %s\n", n->name().c_str());
  
  LOG_B("All Fields:\n");
  for (auto n : all_fields) {
    LOG_G("  %s:%s %s\n",
      n->name().c_str(),
      n->type_name().c_str(),
      to_string(n->state)
    );
  }
  
  LOG_B("Input Fields:\n");
  for (auto n : input_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  
  LOG_B("Input Params:\n");
  for (auto n : input_arguments)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  
  LOG_B("Output Signals:\n");
  for (auto n : output_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  
  LOG_B("Output Registers:\n");
  for (auto n : public_registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Returns:\n");
  for (auto n : output_returns)
    LOG_G("  %s:%s\n", n->name().c_str(), n->node.get_field(field_type).text().c_str());
  
  /*
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  */
  
  LOG_B("Components:\n");
  for (auto component : all_components) {
    auto component_mod = source_file->lib->get_module(component->type_name());
    LOG_G("  %s:%s\n", component->name().c_str(), component_mod->mod_name.c_str());
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

  LOG_B("Port map:\n");
  for (auto &kv : port_map)
    LOG_G("  %s = %s\n", kv.first.c_str(), kv.second.c_str());

  /*
  LOG_B("State map:\n");
  for (auto &kv : mod_states.get_map())
    LOG_G("  %s = %s\n", kv.first.c_str(), to_string(kv.second));
  */

  LOG_B("\n");
}

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// All modules are now in the library, we can resolve references to other
// modules when we're collecting fields.

CHECK_RETURN Err MtModule::load_pass1() {
  Err err;

  err << collect_modparams();
  err << collect_methods();
  err << collect_fields_and_components();

  if (err.has_err()) {
    err << ERR("Module %s failed in load_pass1()\n", name().c_str());
  }

#if 0
  // enums = new std::vector<MtEnum>();

  auto mod_body = mod_class.get_field(field_body).check_null();
  for (const auto& n : mod_body) {
    if (n.sym != sym_field_declaration) continue;

    // MtField f(n);

    // enum class
    if (n.get_field(::field_type).sym == sym_enum_specifier) {
      auto new_enum = new MtEnum(n);
      enums.push_back(new_enum);
      continue;
    }

    // Everything not an enum shoul have 1 or more declarator fields that
    // contain the field name(s).

    // int field_count = 0;

    /*
    for (auto c : n) {
      if (c.field != field_declarator) continue;
      field_count++;
      auto name =
          c.sym == sym_array_declarator ? c.get_field(field_declarator) : c;

      MtField f(n);

      auto type_name = n.node_to_type();
      if (source_file->lib->has_mod(type_name)) {
        MtSu bmod su bmod(n);
        sub mod.mod = source_file->lib->get_mod(type_name);
        sub mod.name = n.get_field(field_declarator).text();
        sub mods.push_back(sub mod);
      }
    }
    */
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_modparams() {
  Err err;

  if (mod_template) {
    auto params = mod_template.get_field(field_parameters);

    for (int i = 0; i < params.named_child_count(); i++) {
      auto child = params.named_child(i);
      if (child.sym == sym_parameter_declaration ||
          child.sym == sym_optional_parameter_declaration) {
        modparams.push_back(MtParam::construct("<template>", child));
      } else {
        err << ERR("Unknown template parameter type %s", child.ts_node_type());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_fields_and_components() {
  Err err;

  assert(all_fields.empty());
  assert(all_components.empty());
  assert(localparams.empty());

  auto mod_body = mod_class.get_field(field_body).check_null();
  bool in_public = false;


  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym != sym_field_declaration) {
      continue;
    }

    auto new_field = MtField::construct(n, in_public);


    // FIXME why are we excluding enums here? because they're not a "field"?
    if (n.get_field(field_type).sym == sym_enum_specifier) {
      all_enums.push_back(new_field);
    }
    else if (new_field->is_component()) {
      all_components.push_back(new_field);
    }
    else {
      all_fields.push_back(new_field);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_methods() {
  Err err;

  assert(all_methods.empty());

  bool in_public = false;
  auto mod_body = mod_class.get_field(field_body).check_null();

  // Create method objects for all function defition nodes

  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym != sym_function_definition) continue;

    MtMethod *m = MtMethod::construct(n, this, source_file->lib);

    // Type can be null for constructor/destructor
    auto func_type = n.get_field(field_type);  
    auto func_decl = n.get_field(field_declarator);
    auto func_name = func_decl.get_field(field_declarator).name4();

    m->is_public = in_public;
    m->is_private = !in_public;
    m->is_const = false;
    for (const auto& n : func_decl) {
      if (n.sym == sym_type_qualifier && n.text() == "const") {
        m->is_const = true;
        break;
      }
    }

    m->has_return = func_type && func_type.text() != "void";

    auto func_args = func_decl.get_field(field_parameters);
    for (int i = 0; i < func_args.named_child_count(); i++) {
      auto param = func_args.named_child(i);
      assert(param.sym == sym_parameter_declaration);
      auto param_name = param.get_field(field_declarator).text();
      m->params.push_back(param_name);
      m->param_nodes.push_back(param);
    }

    all_methods.push_back(m);
  }

  // Categorize all methods

  for (auto m : all_methods) {

    // Type can be null for constructor/destructor
    auto func_type = m->node.get_field(field_type);  
    auto func_decl = m->node.get_field(field_declarator);
    auto func_name = func_decl.get_field(field_declarator).name4();

    m->is_init = func_name.starts_with("init") || func_type.is_null();
    m->is_tick = func_name.starts_with("tick");
    m->is_tock = m->is_public && !m->is_init;

    // Anything that's not an init/tick/tock is either a task (non-const) or a function (const).

    if (!m->is_init && !m->is_tick && !m->is_tock) {
      m->is_task = !m->is_const;
      m->is_func = m->is_const;
    }

    if (m->is_tick && m->has_return) {
      err << ERR("Tick method %s has a return value\n", m->name().c_str());
    }

    if (m->is_tick && m->is_public) {
      err << ERR("Tick %s must be private\n", m->name().c_str());
    }

    if (m->is_init && !m->is_public) {
      err << ERR("Init method %s is not public\n", m->name().c_str());
    }

    if (m->is_init && m->params.size()) {
      err << ERR("Init method %s may not have params\n", m->name().c_str());
    }

    if (m->is_init) {
      init_methods.push_back(m);
    } else if (m->is_tick) {
      tick_methods.push_back(m);
    } else if (m->is_tock) {
      tock_methods.push_back(m);
    } else if (m->is_task) {
      if (m->is_const) {
        err << ERR("CONST TASK FUNCTION BAD!\n");
      }
      task_methods.push_back(m);
    } else {
      assert(m->is_func);
      func_methods.push_back(m);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::trace() {
  Err err;
  LOG_G("Tracing all public tocks in %s\n", name().c_str());

  mod_state.clear();

  for (auto m : all_methods) {
    LOG_INDENT_SCOPE();

    if (!(m->is_public && m->is_tock)) {
      continue;
    }

    LOG_G("Tracing %s.%s\n", name().c_str(), m->name().c_str());
    LOG_INDENT_SCOPE();

    MtTracer tracer;
    tracer._path_stack.push_back("<top>");
    tracer._mod_stack.push_back(this);
    tracer._method_stack.push_back(m);
    tracer.push_state(&mod_state);

    err << tracer.trace_method(m);

    LOG_G("Trace done\n");
  }

  if (err.has_err()) {
    err << ERR("Method trace failed.");
    return err;
  }

  // Check that all entries in the state map ended up in a valid state.

  //MtTracer::dump_trace(mod_state);

  for (auto& pair : mod_state) {

    if (pair.second == FIELD_INVALID) {
      err << ERR("Field %s has invalid state\n", pair.first.c_str(), to_string(pair.second));
    }

    // Assign the final merged states back from the map to the fields.

    auto path = pair.first;
    assert(path.starts_with("<top>."));
    path.erase(path.begin(), path.begin() + 6);

    auto field = get_field(path);
    if (field) {
      field->state = pair.second;
    }
  }

  // Check that all fields got a state assigned to them.

  /*
  for (auto f : all_fields) {
    if (f->is_component() || f->is_param()) continue;

    if (mod_states.get_actions("<top>." + f->name()) == FIELD_NONE) {
      err << ERR("No method in the public interface of %s touched field %s!\n", name().c_str(), f->name().c_str());
    }
  }
  */

  return err;
}

//------------------------------------------------------------------------------
// All modules have populated their fields, match up tick/tock calls with their
// corresponding methods.

CHECK_RETURN Err MtModule::load_pass2() {
  Err err;
  assert (!mod_class.is_null());

  err << categorize_fields();
  err << build_port_map();
  err << sanity_check();

  return err;
}

//------------------------------------------------------------------------------
// Collect all inputs to all tock and getter methods and merge them into a list
// of input ports.

CHECK_RETURN Err MtModule::categorize_fields() {
  Err err;

  LOG_G("Categorizing %s\n", name().c_str());

  for (auto f : all_fields) {

    if (f->is_param()) {
      localparams.push_back(MtParam::construct("<localparam>", f->node));
    }
    else if (f->is_public()) {
      switch(f->state) {
      case FIELD_NONE     : assert(false); break;
      case FIELD_INPUT    : input_signals.push_back(f); break;
      case FIELD_OUTPUT   : output_signals.push_back(f); break;
      case FIELD_SIGNAL   : output_signals.push_back(f); break;
      case FIELD_REGISTER : public_registers.push_back(f); break;
      case FIELD_INVALID  : assert(false); break;
      default: assert(false);
      }
    }
    else {
      switch(f->state) {
      case FIELD_NONE     : {
        err << ERR("Field %s was not touched during trace\n", f->name().c_str());
        break;
      }
      case FIELD_INPUT    : private_registers.push_back(f); break; // read-only thing like mem initialized in init()
      case FIELD_OUTPUT   : private_signals.push_back(f); break;
      case FIELD_SIGNAL   : private_signals.push_back(f); break;
      case FIELD_REGISTER : private_registers.push_back(f); break;
      case FIELD_INVALID  : assert(false); break;
      default: assert(false);
      }
    }
  }

  for (auto m : all_methods) {
    if (!m->is_public) continue;
    if (!m->is_tock) continue;

    auto params =
      m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      MtParam* new_input = MtParam::construct(m->name(), param);
      input_arguments.push_back(new_input);
    }

    if (m->is_tock && m->has_return) {
      output_returns.push_back(m);
    }
  }

  return err;
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

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

    for (int i = 0; i < component_mod->input_arguments.size(); i++) {
      auto key = component->name() + "." + component_mod->input_arguments[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to component port %s\n", key.c_str());
      }
    }
  }
    
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::sanity_check() {
  Err err;

  // Inputs, outputs, registers, and components must not overlap.

  std::set<std::string> field_names;

  for (auto n : input_signals) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate input name %s\n", n->name().c_str());
    } else {
      field_names.insert(n->name());
    }
  }

  /*
  for (auto n : input_arguments) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate input name %s\n", n->name().c_str());
    } else {
      field_names.insert(n->name());
    }
  }
  */

  for (auto n : output_signals) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate output name %s\n", n->name().c_str());
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : public_registers) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate output name %s\n", n->name().c_str());
    } else {
      field_names.insert(n->name());
    }
  }

  /*
  for (auto n : registers) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate register name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }
  */

  for (auto n : all_components) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate component name %s\n", n->name().c_str());
    } else {
      field_names.insert(n->name());
    }
  }

  return err;
}

//------------------------------------------------------------------------------
#if 0
void MtModule::check_dirty_ticks() {
  for (auto tick : tick_methods) {
    tick->update_delta();
    dirty_check_fail |= tick->delta->error;
  }
}

//------------------------------------------------------------------------------

void MtModule::check_dirty_tocks() {
  for (auto tock : tock_methods) {
    tock->update_delta();
    dirty_check_fail |= tock->delta->error;
  }
}
#endif
