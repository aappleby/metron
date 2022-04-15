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

FieldState merge_delta(FieldState a, FieldDelta b);
CHECK_RETURN Err merge_series(MtStateMap & ma, MtStateMap & mb, MtStateMap & out);

//------------------------------------------------------------------------------

bool MtField::is_submod() const {
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
  for (auto n : output_registers)
    if (n->name() == name) return n;
  return nullptr;
}

MtMethod *MtModule::get_output_return(const std::string &name) {
  for (auto n : output_returns)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_submod(const std::string &name) {
  for (auto n : all_submods) {
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
  for (auto n : output_registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Returns:\n");
  for (auto n : output_returns)
    LOG_G("  %s:%s\n", n->name().c_str(), n->node.get_field(field_type).text().c_str());
  
  /*
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  */
  
  LOG_B("Submods:\n");
  for (auto submod : all_submods) {
    auto submod_mod = source_file->lib->get_module(submod->type_name());
    LOG_G("  %s:%s\n", submod->name().c_str(), submod_mod->mod_name.c_str());
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

  LOG_B("State map:\n");
  for (auto &kv : mod_states.get_map())
    LOG_G("  %s = %s\n", kv.first.c_str(), to_string(kv.second));

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
  err << collect_field_and_submods();

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
        MtSubmod submod(n);
        submod.mod = source_file->lib->get_mod(type_name);
        submod.name = n.get_field(field_declarator).text();
        submods.push_back(submod);
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

CHECK_RETURN Err MtModule::collect_field_and_submods() {
  Err err;

  assert(all_fields.empty());
  assert(all_submods.empty());
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
    else if (new_field->is_submod()) {
      all_submods.push_back(new_field);
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
  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym != sym_function_definition) continue;

    MtMethod *m = MtMethod::construct(n, this, source_file->lib);

    auto func_type =
        n.get_field(field_type);  // Can be null for constructor/destructor
    auto func_decl = n.get_field(field_declarator);
    auto func_body = n.get_field(field_body);

    auto func_name = func_decl.get_field(field_declarator).name4();
    auto func_args = func_decl.get_field(field_parameters);

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


    m->is_init = func_name.starts_with("init") || func_type.is_null();
    m->is_tick = func_name.starts_with("tick");
    m->is_tock = func_name.starts_with("tock");

    // Anything that's not an init/tick/tock is either a task (non-const) or a function (const).

    if (!m->is_init && !m->is_tick && !m->is_tock) {
      m->is_task = !m->is_const;
      m->is_func = m->is_const;
    }

    for (int i = 0; i < func_args.named_child_count(); i++) {
      auto param = func_args.named_child(i);
      assert(param.sym == sym_parameter_declaration);
      auto param_name = param.get_field(field_declarator).text();
      m->params.push_back(param_name);
      m->param_nodes.push_back(param);
    }

    all_methods.push_back(m);

    if (m->is_tick && m->has_return) {
      err << ERR("Tick method %s has a return value", m->name().c_str());
      break;
    }

    if (m->is_init && !m->is_public) {
      err << ERR("Init method %s is not public", m->name().c_str());
      break;
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
  LOG_G("Tracing %s\n", name().c_str());
  LOG_INDENT_SCOPE();

  // Trace all methods, except for inits.

  for (auto m : all_methods) {
    if (!(m->is_public && (m->is_tick || m->is_tock))) {
      continue;
    }

    LOG_G("Tracing %s.%s\n", name().c_str(), m->name().c_str());
    LOG_INDENT_SCOPE();

    MtTracer tracer;
    tracer._path_stack.push_back("<top>");
    tracer._mod_stack.push_back(this);
    tracer._method_stack.push_back(m);
    tracer.push_state(&m->method_state);

    if (m->is_tick) {
    }
    else if (m->is_tock) {
    }
    else if (m->is_task) {
    }
    else if (m->is_func) {
    }

    err << tracer.trace_method(m);

    // Lock all states touched by the method.

    for (auto& pair : m->method_state.get_map()) {
      auto old_state = pair.second;
      auto new_state = merge_delta(old_state, DELTA_EF);

      if (new_state == FIELD_INVALID) {
        err << ERR("Field %s was %s, now %s!", pair.first.c_str(), to_string(old_state), to_string(new_state));
      }

      pair.second = new_state;
    }
  }

  // Merge the traces for all public tick/tock methods in lexical order.
  // FIXME - sort methods, then merge

  for (auto m : all_methods) {
    if (!(m->is_public && (m->is_tick || m->is_tock))) {
      continue;
    }

    err << merge_series(mod_states, m->method_state, mod_states);
    if (err.has_err()) {
      LOG_R("MtModule::trace - Cannot merge state_mod with m->method_state\n");

      LOG_R("state_mod:\n");
      LOG_INDENT();
      MtTracer::dump_trace(mod_states);
      LOG_DEDENT();

      LOG_R("state_method:\n");
      LOG_INDENT();
      MtTracer::dump_trace(m->method_state);
      LOG_DEDENT();
    }
  }

  // Check that all entries in the state map ended up in a valid state.

  for (auto& pair : mod_states.get_map()) {

    switch(pair.second) {

      // These states are OK.
    case FIELD_RD_____:
    case FIELD____WR_L:
    case FIELD_RD_WR_L:
    case FIELD____WS_L:
      break;

      // These states are bad, but we would error out of tracing before we got here, so they can't currently be hit.
      // KCOV_OFF
    case FIELD________:
      err << ERR("Field %s was never read or written!\n", pair.first.c_str());
      break;
    case FIELD____WR__:
      err << ERR("Register %s was written but never read or locked - internal error?\n", pair.first.c_str());
      break;
    case FIELD_RD_WR__:
      err << ERR("Register %s was written but never locked - internal error?\n", pair.first.c_str());
      break;
    case FIELD____WS__:
      err << WARN("Signal %s was written but never read - is it an output?\n", pair.first.c_str());
      break;
    default:
      err << ERR("Field %s has an invalid state %d\n", pair.first.c_str(), pair.second);
      break;
      // KCOV_ON
    }
  }

  // Assign the final merged states back from the map to the fields.

  for (auto& pair : mod_states.get_map()) {

    auto path = pair.first;
    assert(path.starts_with("<top>."));
    path.erase(path.begin(), path.begin() + 6);

    auto field = get_field(path);
    if (field) {
      field->state = pair.second;

      if (!field->is_public()) {
        if (pair.second == FIELD____WR_L) {
          err << WARN("Private register %s was written and locked but never read - is it redundant?\n", pair.first.c_str());
        }

        if (pair.second == FIELD_RD_____) {
          err << WARN("Private field %s was read but never written - should it be const?\n", pair.first.c_str());
        }
      }
    }
  }


  // Check that all fields got a state assigned to them.

  for (auto f : all_fields) {
    if (f->is_submod()) continue;
    if (f->is_param()) continue;
    if (!mod_states.contains("<top>." + f->name())) {
      err << ERR("No method in the public interface of %s touched field %s!\n", name().c_str(), f->name().c_str());
    }
  }

  return err;
}

//------------------------------------------------------------------------------
// All modules have populated their fields, match up tick/tock calls with their
// corresponding methods.

CHECK_RETURN Err MtModule::load_pass2() {
  Err err;
  assert (!mod_class.is_null());

  err << collect_input_arguments();
  err << categorize_fields();

  err << build_port_map();
  err << sanity_check();
  return err;
}

//------------------------------------------------------------------------------
// Collect all inputs to all tock and getter methods and merge them into a list
// of input ports.

CHECK_RETURN Err MtModule::collect_input_arguments() {
  Err err;

  assert(input_arguments.empty());

  for (auto m : all_methods) {
    if (!m->is_public) continue;
    if (!m->is_tick && !m->is_tock) continue;

    auto params =
        m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      MtParam* new_input = MtParam::construct(m->name(), param);
      input_arguments.push_back(new_input);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::categorize_fields() {
  Err err;

  assert(localparams.empty());
  assert(input_signals.empty());
  assert(output_signals.empty());
  assert(output_registers.empty());
  assert(output_returns.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {

    if (f->is_param()) {
      localparams.push_back(MtParam::construct("<localparam>", f->node));
    }
    else if (f->is_public()) {
      if (f->is_input_signal()) {
        input_signals.push_back(f);
      }
      else if (f->is_output_signal()) {
        output_signals.push_back(f);
      }
      else if (f->is_output_register()) {
        output_registers.push_back(f);
      }
      // KCOV_OFF
      else {
        debugbreak();
      }
      // KCOV_ON
    }
    else {
      if (f->state == FIELD____WS_L) {
        private_signals.push_back(f);
      }
      else if (f->state == FIELD_RD_____) {
        // Probably a read-only memory loaded in init().
        private_registers.push_back(f);
      }
      else if (f->state == FIELD____WR_L) {
        // Write-only field. Not a bug, tracer will warn about it.
        private_registers.push_back(f);
      }
      else if (f->state == FIELD_RD_WR_L) {
        private_registers.push_back(f);
      }
      // KCOV_OFF
      else {
        debugbreak();
      }
      // KCOV_ON
    }
  }

  for (auto m : all_methods) {
    if (m->is_public && m->is_tock && m->has_return) {
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
  // Assignments to submod fields bind ports.

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_assignment_expression) return;

    auto node_lhs = child.get_field(field_left);
    auto node_rhs = child.get_field(field_right);

    if (node_lhs.sym == sym_field_expression) {
      auto submod_name = node_lhs.get_field(field_argument).text();
      if (get_submod(submod_name)) {
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
    MtModule *call_submod = nullptr;
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
        call_member = get_submod(node_this.text());
        if (call_member) {
          call_submod = source_file->lib->get_module(call_member->type_name());
          call_method = call_submod->get_method(node_method.text());
        } else {
          err << ERR("Submodule %s not found!\n", node_this.text().c_str());
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

  // Verify that all input ports of all submods are bound.

  for (const auto& submod : all_submods) {
    auto submod_mod = source_file->lib->get_module(submod->type_name());

    for (int i = 0; i < submod_mod->input_signals.size(); i++) {
      auto key = submod->name() + "." + submod_mod->input_signals[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to submod port %s\n", key.c_str());
      }
    }

    for (int i = 0; i < submod_mod->input_arguments.size(); i++) {
      auto key = submod->name() + "." + submod_mod->input_arguments[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to submod port %s\n", key.c_str());
      }
    }
  }
    
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::sanity_check() {
  Err err;

  // Inputs, outputs, registers, and submods must not overlap.

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

  for (auto n : output_registers) {
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

  for (auto n : all_submods) {
    if (field_names.contains(n->name())) {
      err << ERR("Duplicate submod name %s\n", n->name().c_str());
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
