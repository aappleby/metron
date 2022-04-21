#include "MtTracer.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "metron_tools.h"

MtModule* MtTracer::mod_top() { return _method_stack.back()->mod; }

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MnNode n, bool is_write) {
  Err err;

  switch (n.sym) {

    case sym_field_expression: {
      auto node_arg   = n.get_field(field_argument);

      if (mod_top()->get_component(node_arg.text())) {
        err << trace(n.text(), is_write);
      }
      else if (mod_top()->get_field(node_arg.text())) {
        err << trace(n.text(), is_write);
      }
      break;
    }

    case sym_identifier:
      if (mod_top()->get_field(n.text())) {
        err << trace(n.text(), is_write);
      }
      break;

    case sym_subscript_expression:
      err << trace_dispatch(n.get_field(field_argument), is_write);
      break;

    case sym_call_expression:
      err << trace_call(n);
      break;

    case sym_conditional_expression:
    case sym_if_statement:
      err << trace_branch(n);
      break;

    case sym_switch_statement:
      err << trace_switch(n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      err << trace_dispatch(n.get_field(field_argument), false);
      err << trace_dispatch(n.get_field(field_argument), true);
      break;
    }

    case sym_assignment_expression:
      err << trace_dispatch(n.get_field(field_right), false);
      err << trace_dispatch(n.get_field(field_left), true);
      break;

    default:
      for (const auto& c : n) err << trace_dispatch(c);
      break;
  }

  return err;
}

//------------------------------------------------------------------------------


CHECK_RETURN Err MtTracer::trace_call(MnNode n) {
  Err err;

  auto node_func = n.get_field(field_function);
  auto node_args = n.get_field(field_arguments);

  // Trace the args first.
  err << trace_dispatch(node_args);

  //----------

  if (node_func.sym == sym_field_expression) {
    auto node_arg   = node_func.get_field(field_argument);
    auto node_field = node_func.get_field(field_field);

    auto component = mod_top()->get_component(node_arg.text());
    auto dst_module = lib->get_module(component->type_name());
    auto dst_method = dst_module->get_method(node_field.text());

    _component_stack.push_back(component);
    _method_stack.push_back(dst_method);
    err << trace_dispatch(dst_method->node);
    _method_stack.pop_back();
    _component_stack.pop_back();
  }

  //----------

  else if (node_func.sym == sym_identifier) {

    auto dst_method = mod_top()->get_method(node_func.text());

    if (dst_method) {
      _method_stack.push_back(dst_method);
      err << trace_dispatch(dst_method->node);
      _method_stack.pop_back();
    }
  }
  
  //----------

  else if (node_func.sym == sym_template_function) {

    // FIXME this is a stub, we don't currently have real template function support
    auto node_name = n.get_field(field_function).get_field(field_name).text();
    if (node_name == "bx" || node_name == "dup" || node_name == "sign_extend") {
    } else {
      debugbreak();
    }
    return err;

  }
  
  //----------

  else {
    err << ERR("MtModule::build_call_tree - don't know what to do with %s\n", n.ts_node_type());
  }

  return err;
}

//------------------------------------------------------------------------------
// good

CHECK_RETURN Err MtTracer::trace_branch(MnNode n) {
  Err err;

  auto node_cond     = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  err << trace_dispatch(node_cond);

  StateMap branch_a = *state_top();
  StateMap branch_b = *state_top();

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

  err << merge_branch(branch_a, branch_b, *state_top());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MnNode n) {
  Err err;

  err << trace_dispatch(n.get_field(field_condition));

  StateMap old_state = *state_top();

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {

      if (c.named_child_count() == 1) {
        // case statement without body
        continue;
      }

      if (first_branch) {
        err << trace_dispatch(c);
        first_branch = false;
      } else {
        StateMap state_case = old_state;

        push_state(&state_case);
        err << trace_dispatch(c);
        pop_state();

        err << merge_branch(*state_top(), state_case, *state_top());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace(const std::string& field_name, bool is_write) {
  Err err;

  std::string field_path = "";
  for (int i = 0; i < _component_stack.size(); i++) {
    if (field_path.size()) field_path.push_back('.');
    field_path = field_path + (_component_stack[i] ? _component_stack[i]->name() : "<top>");
  }

  field_path = field_path + "." + field_name;

  auto& old_state = (*state_top())[field_path];
  FieldState new_state;

  if (is_write) {
    switch(old_state) {
    case FIELD_NONE     : new_state = FIELD_OUTPUT;   break;
    case FIELD_INPUT    : new_state = FIELD_REGISTER; break;
    case FIELD_OUTPUT   : new_state = FIELD_OUTPUT;   break;
    case FIELD_SIGNAL   : new_state = FIELD_INVALID;  break;
    case FIELD_REGISTER : new_state = FIELD_REGISTER; break;
    case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
    default: assert(false);
    }
  }
  else {
    switch(old_state) {
    case FIELD_NONE     : new_state = FIELD_INPUT;    break;
    case FIELD_INPUT    : new_state = FIELD_INPUT;    break;
    case FIELD_OUTPUT   : new_state = FIELD_SIGNAL;   break;
    case FIELD_SIGNAL   : new_state = FIELD_SIGNAL;   break;
    case FIELD_REGISTER : new_state = FIELD_INVALID;  break;
    case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
    default: assert(false);
    }
  }

  old_state = new_state;
  return err;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::merge_branch(StateMap & ma, StateMap & mb, StateMap & out) {
  Err err;

  StateMap temp;

  for (const auto& a : ma) temp[a.first] = FIELD_INVALID;
  for (const auto& b : mb) temp[b.first] = FIELD_INVALID;

  for (auto& pair : temp) {
    auto a = ma[pair.first];
    auto b = mb[pair.first];
   
    if (a > b) {
      auto t = a; a = b; b = t;
    }

    if (a == b) {
      pair.second = a;
    }
    else if (a == FIELD_INVALID || b == FIELD_INVALID) {
      pair.second = FIELD_INVALID;
    }
    else {
      if (a == FIELD_NONE     && b == FIELD_INPUT)    pair.second = FIELD_INPUT;    // promote
      if (a == FIELD_OUTPUT   && b == FIELD_SIGNAL)   pair.second = FIELD_SIGNAL;   // promote

      if (a == FIELD_NONE     && b == FIELD_OUTPUT)   pair.second = FIELD_REGISTER; // half-write, infer reg
      if (a == FIELD_INPUT    && b == FIELD_OUTPUT)   pair.second = FIELD_REGISTER; // half-write, infer reg
      if (a == FIELD_NONE     && b == FIELD_REGISTER) pair.second = FIELD_REGISTER; // promote
      if (a == FIELD_INPUT    && b == FIELD_REGISTER) pair.second = FIELD_REGISTER; // promote
      if (a == FIELD_OUTPUT   && b == FIELD_REGISTER) pair.second = FIELD_REGISTER; // promote

      if (a == FIELD_NONE     && b == FIELD_SIGNAL)   pair.second = FIELD_INVALID; // half-write, bad signal
      if (a == FIELD_INPUT    && b == FIELD_SIGNAL)   pair.second = FIELD_INVALID; // order conflict

      if (a == FIELD_SIGNAL   && b == FIELD_REGISTER) pair.second = FIELD_INVALID; // order conflict
    }
  }

  out.swap(temp);

  return err;
}


//------------------------------------------------------------------------------
