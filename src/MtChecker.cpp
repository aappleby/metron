#include "MtChecker.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "metron_tools.h"

#if 0
/*
digraph G {

none -> input     [label="read"];
none -> output    [label="write"];

input -> input    [label="read"];
input -> register [label="write"];

output -> signal  [label="read"];
output -> output  [label="write"];

signal -> signal   [label="read"];
signal -> invalid  [label="write"];

register -> invalid [label="read"];
register -> register [label="write"];
}
*/

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::merge_branch(StateMap& ma, StateMap& mb,
                                         StateMap& out) {
  Err err;
  std::set<std::string> keys;

  StateMap temp;

  for (const auto& a : ma) temp[a.first] = FIELD_INVALID;
  for (const auto& b : mb) temp[b.first] = FIELD_INVALID;

  for (auto& pair : temp) {
    auto a = ma[pair.first];
    auto b = mb[pair.first];

    if (a > b) {
      auto t = a;
      a = b;
      b = t;
    }

    if (pair.first == "<top>.data_memory_bus.data_memory.byteena") {
      int x = 1;
      x++;
    }

    if (a == b) {
      pair.second = a;
    } else if (a == FIELD_INVALID || b == FIELD_INVALID) {
      pair.second = FIELD_INVALID;
    } else {
      if (a == FIELD_UNKNOWN && b == FIELD_INPUT)
        pair.second = FIELD_INPUT;  // promote
      if (a == FIELD_OUTPUT && b == FIELD_SIGNAL)
        pair.second = FIELD_SIGNAL;  // promote

      if (a == FIELD_UNKNOWN && b == FIELD_OUTPUT)
        pair.second = FIELD_REGISTER;  // half-write, infer reg
      if (a == FIELD_INPUT && b == FIELD_OUTPUT)
        pair.second = FIELD_REGISTER;  // half-write, infer reg
      if (a == FIELD_UNKNOWN && b == FIELD_REGISTER)
        pair.second = FIELD_REGISTER;  // promote
      if (a == FIELD_INPUT && b == FIELD_REGISTER)
        pair.second = FIELD_REGISTER;  // promote
      if (a == FIELD_OUTPUT && b == FIELD_REGISTER)
        pair.second = FIELD_REGISTER;  // promote

      if (a == FIELD_UNKNOWN && b == FIELD_SIGNAL)
        pair.second = FIELD_INVALID;  // half-write, bad signal
      if (a == FIELD_INPUT && b == FIELD_SIGNAL)
        pair.second = FIELD_INVALID;  // order conflict

      if (a == FIELD_SIGNAL && b == FIELD_REGISTER)
        pair.second = FIELD_INVALID;  // order conflict
    }
  }

  for (auto& pair : temp) {
    if (pair.second == FIELD_INVALID) {
      debugbreak();
    }
  }

  out.swap(temp);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_method(MtMethod* method) {
  Err err;

  root = method;

  if (has_non_terminal_return(method->_node)) {
    err << ERR("Found non-terminal return in method %s\n",
               method->name().c_str());
  }

  err << trace_dispatch(method->_node.get_field(field_body));

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_dispatch(MnNode n) {
  Err err;
  if (!n.is_named()) return err;

  switch (n.sym) {
    case sym_assignment_expression:
      err << trace_assign(n);
      break;
    case sym_call_expression:
      err << trace_call(n);
      break;
    case sym_field_expression: {
      auto base_node = n.get_field(field_argument);
      auto field = mod()->get_field(base_node.text());

      if (field->is_component()) {
        auto component_mod =
            mod()->source_file->lib->get_module(field->type_name());
        auto component_field =
            component_mod->get_field(n.get_field(field_field).text());
        err << trace_read(field, component_field);
      } else if (field) {
        // struct
        err << trace_read(field);
      }

      break;
    }
    case sym_identifier: {
      auto id = n.text();
      auto field = mod()->get_field(id);
      if (field) {
        err << trace_read(field);
      } else {
        // err << WARN("FIXME trace parameter read %s\n", id.c_str());
      }
      break;
    }
    case sym_if_statement:
      err << trace_branch(n);
      break;
    case sym_switch_statement:
      err << trace_switch(n);
      break;

    case sym_declaration:
      // do we want to do anything special for this node, like note the
      // existence of the local somewhere?
      err << trace_children(n);
      break;

    case sym_init_declarator:
      // Skip the lhs
      err << trace_dispatch(n.get_field(field_value));
      break;

    case sym_conditional_expression:
      err << trace_branch(n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      auto arg = n.get_field(field_argument);
      if (arg.sym == sym_identifier) {
        auto field = mod()->get_field(arg.text());
        if (field) {
          err << trace_read(field);
          err << trace_write(field);
        } else {
          // local variable increment
        }
      } else {
        err << ERR("Not sure how to increment a %s\n", n.ts_node_type());
      }
      break;
    }

    case sym_primitive_type:
    case sym_template_type:
    case sym_number_literal:
    case sym_using_declaration:
    case sym_break_statement:
    case sym_comment:
    case alias_sym_type_identifier:
    case sym_storage_class_specifier:
    case sym_type_qualifier:
      // do-nothing nodes
      break;

    case sym_return_statement:
      err << trace_children(n);
      break;

    case sym_argument_list:
      err << trace_children(n);
      break;

    case sym_parenthesized_expression:
      err << trace_children(n);
      break;

    case sym_binary_expression:
      err << trace_children(n);
      break;

    case sym_compound_statement:
      err << trace_children(n);
      break;

    case sym_case_statement:
      err << trace_children(n);
      break;

    case sym_expression_statement:
      err << trace_children(n);
      break;

    case sym_condition_clause:
      err << trace_children(n);
      break;

    case sym_unary_expression:
      err << trace_children(n);
      break;

    case sym_qualified_identifier:
      err << trace_children(n);
      break;

    case alias_sym_namespace_identifier:
      err << trace_children(n);
      break;

    case sym_for_statement:
      err << trace_children(n);
      break;

    case sym_subscript_expression:
      // I don't think we need to do anything special?
      err << trace_children(n);
      break;

    default:
      err << ERR("Don't know what to do with %s\n", n.ts_node_type());
      n.dump_tree();
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_children(MnNode n) {
  Err err;
  for (const auto& c : n) {
    err << trace_dispatch(c);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_assign(MnNode n) {
  Err err;

  auto node_lhs = n.get_field(field_left);
  auto node_rhs = n.get_field(field_right);

  err << trace_dispatch(node_rhs);

  auto node_name = node_lhs;
  if (node_lhs.sym == sym_subscript_expression) {
    node_name = node_lhs.get_field(field_argument);
  }
  auto field = mod()->get_field(node_name.text());

  if (node_lhs.sym == sym_identifier) {
    if (field) {
      err << trace_write(field);
    }
  } else if (node_lhs.sym == sym_subscript_expression) {
    if (field) {
      err << trace_write(field);
    }
  } else if (node_lhs.sym == sym_field_expression) {
    auto base_node = node_lhs.get_field(field_argument);
    auto field = mod()->get_field(base_node.text());

    if (field->is_component()) {
      auto component_mod =
          mod()->source_file->lib->get_module(field->type_name());
      auto component_field =
          component_mod->get_field(node_lhs.get_field(field_field).text());
      err << trace_write(field, component_field);
    } else if (field) {
      err << trace_write(field);
    }
  } else {
    debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_call(MnNode n) {
  Err err;

  // Trace the args first.
  err << trace_dispatch(n.get_field(field_arguments));

  auto src_method = method();
  MtMethod* dst_method = nullptr;

  auto node_func = n.get_field(field_function);

  if (node_func.sym == sym_field_expression) {
    auto node_func = n.get_field(field_function);
    auto component_name = node_func.get_field(field_argument).text();
    auto component_method_name = node_func.get_field(field_field).text();

    auto component = mod()->get_field(component_name);
    if (!component) {
      err << ERR("Could not find component %s\n", component_name.c_str());
      return err;
    }

    auto component_type = component->type_name();
    auto dst_module = mod()->source_file->lib->get_module(component_type);

    if (!dst_module) {
      err << ERR("Could not find module %s for component %s\n",
                 component_type.c_str(), component_name.c_str());
    }

    dst_method = dst_module->get_method(component_method_name);

    if (!dst_method) {
      err << ERR("Component %s has no method %s\n", component_name.c_str(),
                 component_method_name.c_str());
      return err;
    }

    /*
    if (!dst_method->is_tock) {
      err << ERR("Method %s is calling a non-tock method %s.%s on a
    component.\n", src_method->name().c_str(), component_name.c_str(),
    component_method_name.c_str());
    }
    */

    // New state map goes on the stack.
    // MtStateMap state_call;
    // push_state(&state_call);

    err << trace_component_call(component_name, dst_module, dst_method);

    // pop_state();

  } else if (node_func.sym == sym_identifier) {
    auto node_func = n.get_field(field_function);

    dst_method = mod()->get_method(node_func.text());

    if (!dst_method) {
      // Utility method call like bN()
      /*
      for (int i = 0; i < depth; i++) printf(" ");
      printf("XCALL! %s.%s -> %s\n",
      name().c_str(), method->name().c_str(),
      node_func.text().c_str());
      */
      return err;
    }

    /*
    if (in_tick() && dst_method->is_public) {
      err << ERR("Can't call public method %s from private %s\n",
    dst_method->name().c_str(), src_method->name().c_str());
    }
    */

    err << trace_method_call(dst_method);

  } else if (node_func.sym == sym_template_function) {
    // FIXME this is a stub, we don't currently have real template function
    // support
    auto node_name = n.get_field(field_function).get_field(field_name).text();
    if (node_name == "bx" || node_name == "dup" || node_name == "sign_extend") {
    } else {
      debugbreak();
    }
    return err;

  } else {
    err << ERR("MtModule::build_call_tree - don't know what to do with %s\n",
               n.ts_node_type());
  }

  if (err.has_err()) {
    return err;
  }

#if 0


  // Call traced, close the state map and merge.

  for (auto& pair : state_call.get_map()) {
    auto old_state = pair.second;
    auto new_state = merge_delta(old_state, DELTA_EF);

    if (new_state == FIELD_INVALID) {
      err << ERR("Field %s was %s, now %s!\n", pair.first.c_str(),
            to_string(old_state), to_string(new_state));
      break;
    }

    pair.second = new_state;
  }

  err << merge_series(*state_top, state_call, *state_top);
#endif

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_method_call(MtMethod* dst_method) {
  Err err;

  auto src_method = method();

  //----------------------------------------

  // bool requires_input_binding  = (dst_method->is_tick || dst_method->is_tock)
  // && dst_method->params.size();

  // We must be either in a tick or a tock, or Metron is broken.
  // assert(in_tick() ^ in_tock());

  /*
  // Methods that require input port bindings cannot be called from inside the
  module. if (requires_input_binding) { err << ERR("Method %s requires input
  port bindings and can only be called from outside the module.\n",
  dst_method->name().c_str()); n.dump_source_lines();
  }
  */

  /*
  // Tasks can only call tasks and functions.
  if (src_method->is_task && !(dst_method->is_func || dst_method->is_task)) {
    err << ERR("Can't call non-function/task %s from task %s.\n",
  dst_method->name().c_str(), src_method->name().c_str());
  }

  // Functions can only call other functions.
  if (src_method->is_func && !dst_method->is_func) {
    err << ERR("Can't call non-function %s from function %s.\n",
  dst_method->name().c_str(), src_method->name().c_str());
  }

  // Public methods with params require input port bindings to call. If we're in
  a tick(), we can't do that. if (in_tick() && requires_input_binding) { err <<
  ERR("Can't call %s from a tick() as it requires input port bindings, and port
  bindings can only be in tock()s.\n", dst_method->name().c_str());
  }

  // Tock methods write signals and can only do so from inside another tock().
  If we're in a tick, that's bad. if (in_tick() && dst_method->is_tock) { err <<
  ERR("Can't call %s from tick method %s\n", dst_method->name().c_str(),
  src_method->name().c_str());
  }
  */

  //----------------------------------------
  // OK, the call should be valid. If the method requires port bindings, build
  // the port names from the current field stack plus the method name.

  // Trace input port bindings as if they are fields being written to.

  /*
  if (requires_input_binding) {
    for (const auto& param : dst_method->params) {
      std::string port_name = dst_method->name() + "." + param;
      err << trace_write(port_name);
      err << trace_read(port_name);
    }
  }
  */

  // if we want sequential actions, we _do_ want to trace into the tick.
  // if we want parallel actions, we do _not_ want to trace into the tick.

  /*
  if (root->is_tock && dst_method->is_tick) {
    dst_method->is_triggered = true;
  }
  else {
    // Now trace the method.
    _path_stack.push_back(_path_stack.back());
    _mod_stack.push_back(_mod_stack.back());
    _method_stack.push_back(dst_method);
    err << trace_dispatch(dst_method->node.get_field(field_body));
    _method_stack.pop_back();
    _mod_stack.pop_back();
    _path_stack.pop_back();
  }
  */

  // Now trace the method.
  _path_stack.push_back(_path_stack.back());
  _mod_stack.push_back(_mod_stack.back());
  _method_stack.push_back(dst_method);
  err << trace_dispatch(dst_method->_node.get_field(field_body));
  _method_stack.pop_back();
  _mod_stack.pop_back();
  _path_stack.pop_back();

  //----------------------------------------
  // Done.

  if (err.has_err()) {
    err << ERR("MtChecker::trace_method_call failed @\n");
  }

  return err;
}

//------------------------------------------------------------------------------
// Component call "component.method()". Pull up the component's module and
// traverse into the method.

CHECK_RETURN Err
MtChecker::trace_component_call(const std::string& component_name,
                                MtModule* dst_module, MtMethod* dst_method) {
  Err err;

  auto src_method = method();

  //----------------------------------------
  // The target must be a public tick or tock.

  /*
  if (dst_method->is_private || !dst_method->is_tock) {
    err << ERR("Method %s is not a public tock, it can't be called from outside
  the module.\n", dst_method->name().c_str());
  }

  // Ticks/tocks with params require input port bindings to call. If we're in a
  tick(), we can't do that.

  if (in_tick() && dst_method->params.size()) {
    err << ERR("Can't call %s from a tick() as it requires a port binding, and
  port bindings can only be in tock()s.\n", dst_method->name());
  }

  // Non-const tock methods write signals and can only do so from inside another
  tock(). If we're in a tick, that's bad.

  if (in_tick() && dst_method->is_tock && !dst_method->is_const) {
    err << ERR("Can't call non-const %s from tick method %s\n",
  dst_method->name().c_str(), src_method->name().c_str());
  }
  */

  //----------------------------------------
  // OK, the call should be valid. If the method requires port bindings, build
  // the port names from the current field stack plus the method name.

  /*
  for (const auto& param : dst_method->params) {
    std::string port_name = component_name + "." + dst_method->name() + "." +
  param; err << trace_write(port_name); err << trace_read(port_name);
  }
  */

  // If we're calling a method that has params, trace the params as if they were
  // input ports. We trace them as both write and read as the current method is
  // writing them and the component, presumably, is reading them.

  // push_state(&dst_method->method_state);
  _path_stack.push_back(_path_stack.back() + "." + component_name);
  _mod_stack.push_back(dst_module);
  _method_stack.push_back(dst_method);

  // Now trace the method.

  err << trace_dispatch(dst_method->_node.get_field(field_body));

  _method_stack.pop_back();
  _mod_stack.pop_back();
  _path_stack.pop_back();
  // pop_state();

  //----------------------------------------
  // Done.

  return err;
}

//------------------------------------------------------------------------------

#if 0

CHECK_RETURN Err MtChecker::trace_template_call(MnNode n) {
  Err err;

  auto node_name = n.get_field(field_function).get_field(field_name).text();

  if (node_name == "bx" || node_name == "dup" || node_name == "sign_extend") {
  } else {
    debugbreak();
  }

#if 0
  auto sibling_method =
    mod()->get_method(node_func.get_field(field_name).text());

  if (sibling_method) {
    // Should probably not see any of these yet?
    debugbreak();
    for (int i = 0; i < depth; i++) printf(" ");
    printf("TCALL! %s.%s -> %s\n", mod()->name().c_str(),
      method()->name().c_str(), node_func.text().c_str());
  } else {
    // Templated utility method call like bx<>, dup<>
  }

#endif

  if (err.has_err()) {
    err << ERR("MtChecker::trace_template_call failed @\n");
    n.dump_source_lines();
  }

  return err;
}
#endif

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_branch(MnNode n) {
  Err err;

  /*
  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  err << trace_dispatch(node_cond);

  StateMap branch_a = *state_top;
  StateMap branch_b = *state_top;

  if (!node_branch_a.is_null()) {
    push_state(&branch_a);
    err << trace_dispatch(node_branch_a);
    pop_state();
  }

  if (!node_branch_b.is_null()) {
    push_state(&branch_b);
    err << trace_dispatch(node_branch_b);
    pop_state();
  }

  err << merge_branch(branch_a, branch_b, *state_top);

  if (err.has_err()) {
    err << ERR("MtChecker::trace_if - error\n");
    n.dump_source_lines();
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_switch(MnNode n) {
  Err err;

  /*

  err << trace_dispatch(n.get_field(field_condition));

  StateMap old_state = *state_top;

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {

      if (c.named_child_count() == 1) {
        // case statement without body
        continue;
      }

      if (!ends_with_break(c)) {
        err << ERR("Case statement doesn't end with break.\n");
        c.dump_source_lines();
      }

      if (first_branch) {
        err << trace_dispatch(c);
        first_branch = false;
      } else {
        StateMap state_case = old_state;

        push_state(&state_case);
        err << trace_dispatch(c);
        pop_state();

        err << merge_branch(*state_top, state_case, *state_top);
      }
    }
  }

  if (err.has_err()) {
    err << ERR("MtChecker::trace_switch failed @\n");
    n.dump_source_lines();
  }
  */

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_read(MtField* field,
                                       MtField* component_field) {
  Err err;

  /*
  assert(field);

  if (field->is_param()) return err;

  method()->fields_read.push_back({field, component_field});

  std::string field_name = field->name();
  if (component_field) field_name = field_name + "." + component_field->name();

  auto field_path = _path_stack.back() + "." + field_name;
  auto& old_state = (*state_top)[field_path];
  FieldState new_state;

  switch(old_state) {
  case FIELD_UNKNOWN     : new_state = FIELD_INPUT;    break;
  case FIELD_INPUT    : new_state = FIELD_INPUT;    break;
  case FIELD_OUTPUT   : new_state = FIELD_SIGNAL;   break;
  case FIELD_SIGNAL   : new_state = FIELD_SIGNAL;   break;
  case FIELD_REGISTER : new_state = FIELD_INVALID;  break;
  case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
  default: assert(false);
  }

  if (new_state == FIELD_INVALID) {
    err << ERR("Reading field %s changed state from %s to %s\n",
  field_name.c_str(), to_string(old_state), to_string(new_state));
  }

  old_state = new_state;
  */
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtChecker::trace_write(MtField* field,
                                        MtField* component_field) {
  Err err;

  /*
  method()->fields_written.push_back({field, component_field});

  std::string field_name = field->name();
  if (component_field) field_name = field_name + "." + component_field->name();

  auto field_path = _path_stack.back() + "." + field_name;
  auto& old_state = (*state_top)[field_path];
  FieldState new_state;

  switch(old_state) {
  case FIELD_UNKNOWN     : new_state = FIELD_OUTPUT;   break;
  case FIELD_INPUT    : new_state = FIELD_REGISTER; break;
  case FIELD_OUTPUT   : new_state = FIELD_OUTPUT;   break;
  case FIELD_SIGNAL   : new_state = FIELD_INVALID;  break;
  case FIELD_REGISTER : new_state = FIELD_REGISTER; break;
  case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
  default: assert(false);
  }

  if (new_state == FIELD_INVALID) {
    err << ERR("Writing field %s changed state from %s to %s\n",
  field_name.c_str(), to_string(old_state), to_string(new_state));
  }

  old_state = new_state;
  */

  return err;
}

//------------------------------------------------------------------------------

// KCOV_OFF
void MtChecker::dump_trace(StateMap& m) {
  LOG_Y("Trace:\n");
  for (const auto& pair : m) {
    LOG_Y("%s = %s\n", pair.first.c_str(), to_string(pair.second));
  }
}

void MtChecker::dump_stack() {
  int depth = (int)_path_stack.size();

  assert(_mod_stack.size() == depth);
  assert(_method_stack.size() == depth);

  for (int i = depth - 1; i >= 0; i--) {
    LOG_G("%s %s.%s()\n", _mod_stack[i]->name().c_str(), _path_stack[i].c_str(),
          _method_stack[i]->name().c_str());
  }
}
// KCOV_ON

//------------------------------------------------------------------------------

bool MtChecker::in_tick() const {
#if 0
  for (int i = (int)_method_stack.size() - 1; i >= 0; i--) {
    auto method = _method_stack[i];
    if (method->is_tock) return false;
    if (method->is_tick) return true;
  }
#endif
  return false;
}

//------------------------------------------------------------------------------

bool MtChecker::in_tock() const {
#if 0
  for (int i = (int)_method_stack.size() - 1; i >= 0; i--) {
    auto method = _method_stack[i];
    if (method->is_tock) return true;
    if (method->is_tick) return false;
  }
#endif
  return false;
}

//------------------------------------------------------------------------------

bool MtChecker::ends_with_break(MnNode n) {
  switch (n.sym) {
    case sym_break_statement:
      return true;

    case sym_for_statement:
    case sym_goto_statement:
    case sym_continue_statement:
    case sym_return_statement:
    case sym_expression_statement:
    case sym_switch_statement:
    case sym_do_statement:
    case sym_while_statement:
      return false;

    case sym_case_statement:
    case sym_compound_statement: {
      for (int i = 0; i < n.named_child_count() - 2; i++) {
        if (ends_with_break(n.named_child(i))) {
          return false;
        }
      }
      return ends_with_break(n.named_child(n.named_child_count() - 1));
    }

    case sym_if_statement: {
      auto node_then = n.get_field(field_consequence);
      auto node_else = n.get_field(field_alternative);
      if (node_else.is_null()) return false;
      return ends_with_break(node_then) && ends_with_break(node_else);
    }
    default:
      return false;
  }
}

//------------------------------------------------------------------------------

bool MtChecker::has_return(MnNode n) {
  if (n.is_null()) return false;

  bool result = false;
  n.visit_tree([&](MnNode child) {
    if (child.sym == sym_return_statement) result = true;
  });
  return result;
}

//------------------------------------------------------------------------------

bool MtChecker::has_non_terminal_return(MnNode n) {
  // A node with no returns has no non-terminal returns.
  if (!has_return(n)) {
    return false;
  }

  switch (n.sym) {
    // A return by itself is techncially a terminal return.
    case sym_return_statement:
      return false;

    // A non-terminal return in a block is bad.
    case sym_compound_statement: {
      for (int i = 0; i < n.named_child_count() - 1; i++) {
        auto child = n.named_child(i);
        if (has_return(child)) {
          return true;
        }
      }
      return has_non_terminal_return(n.named_child(n.named_child_count() - 1));
    }

    // Either both sides of the if must have a terminal return, or neither.
    case sym_if_statement: {
      auto branch_a = n.get_field(field_consequence);
      auto branch_b = n.get_field(field_alternative);

      bool return_a = has_return(branch_a);
      bool return_b = has_return(branch_b);

      // If only one branch returns, that's a non-terminal return.
      if (return_a != return_b) return true;

      // If neither branch returns, there's no non-terminal return (and we
      // shouldn't have gotten here...)
      assert(return_a);

      // Both branches have a return, check if they're both terminal.
      bool result_a = has_non_terminal_return(branch_a);
      bool result_b = has_non_terminal_return(branch_b);
      return result_a || result_b;
    }

    default:
      for (const auto& c : n) {
        if (has_non_terminal_return(c)) return true;
      }
      return false;
  }
}

//------------------------------------------------------------------------------
#endif