#include "MtTracer.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "metron_tools.h"

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MnNode n) {
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
      auto component = mod()->get_component(base_node.text());
      auto field = mod()->get_field(base_node.text());

      if (component) {
        auto component_mod = mod()->source_file->lib->get_module(component->type_name());
        auto component_field = component_mod->get_field(n.get_field(field_field).text());
        err << trace_read(component, component_field);
      }
      else if (field) {
        // struct
        err << trace_read(field);
      }

      break;
    }
    case sym_identifier: {
      auto field = mod()->get_field(n.text());
      err << trace_read(field);
      break;
    }

    case sym_conditional_expression:
    case sym_if_statement:
      err << trace_branch(n);
      break;

    case sym_switch_statement:
      err << trace_switch(n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      auto arg = n.get_field(field_argument);
      if (arg.sym == sym_identifier) {
        auto field = mod()->get_field(arg.text());
        err << trace_read(field);
        err << trace_write(nullptr, field);
      } else {
        err << ERR("Not sure how to increment a %s\n", n.ts_node_type());
      }
      break;
    }

    default:
      for (const auto& c : n) {
        err << trace_dispatch(c);
      }
      break;
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_assign(MnNode n) {
  Err err;

  auto node_lhs = n.get_field(field_left);
  auto node_rhs = n.get_field(field_right);

  err << trace_dispatch(node_rhs);

  //----------

  if (node_lhs.sym == sym_identifier) {
    auto node_name = node_lhs;
    auto field = mod()->get_field(node_name.text());
    err << trace_write(nullptr, field);
  }
  
  //----------

  else if (node_lhs.sym == sym_subscript_expression) {
    auto node_name = node_lhs.get_field(field_argument);
    auto field = mod()->get_field(node_name.text());
    err << trace_write(nullptr, field);
  }

  //----------

  else if (node_lhs.sym == sym_field_expression) {
    auto node_name = node_lhs.get_field(field_argument);
    auto component = mod()->get_component(node_name.text());

    if (component) {
      auto component_mod = mod()->source_file->lib->get_module(component->type_name());
      auto component_field = component_mod->get_field(node_lhs.get_field(field_field).text());
      err << trace_write(component, component_field);
    }
  }

  //----------

  else {
    debugbreak();
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

    auto component_name  = node_func.get_field(field_argument).text();
    auto component_method_name = node_func.get_field(field_field).text();
    auto component = mod()->get_component(component_name);
    auto component_type = component->type_name();
    auto dst_module = mod()->source_file->lib->get_module(component_type);
    auto dst_method = dst_module->get_method(component_method_name);

    _component_stack.push_back(component);
    _path_stack.push_back(_path_stack.back() + "." + component_name);
    _mod_stack.push_back(dst_module);
    _method_stack.push_back(dst_method);
    err << trace_dispatch(dst_method->node.get_field(field_body));
    _method_stack.pop_back();
    _mod_stack.pop_back();
    _path_stack.pop_back();
    _component_stack.pop_back();
  }

  //----------

  else if (node_func.sym == sym_identifier) {

    auto dst_method = mod()->get_method(node_func.text());

    if (dst_method) {
      _component_stack.push_back(_component_stack.back());
      _path_stack.push_back(_path_stack.back());
      _mod_stack.push_back(_mod_stack.back());
      _method_stack.push_back(dst_method);
      err << trace_dispatch(dst_method->node.get_field(field_body));
      _method_stack.pop_back();
      _mod_stack.pop_back();
      _path_stack.pop_back();
      _component_stack.pop_back();
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

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MnNode n) {
  Err err;

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

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_read(MtField* field, MtField* component_field) {
  Err err;
  if (!field) return err;

  if (field->is_param()) return err;

  method()->fields_read.push_back({field, component_field});

  std::string field_name = field->name();
  if (component_field) field_name = field_name + "." + component_field->name();

  auto field_path = _path_stack.back() + "." + field_name;
  auto& old_state = (*state_top)[field_path];
  FieldState new_state;

  switch(old_state) {
  case FIELD_NONE     : new_state = FIELD_INPUT;    break;
  case FIELD_INPUT    : new_state = FIELD_INPUT;    break;
  case FIELD_OUTPUT   : new_state = FIELD_SIGNAL;   break;
  case FIELD_SIGNAL   : new_state = FIELD_SIGNAL;   break;
  case FIELD_REGISTER : new_state = FIELD_INVALID;  break;
  case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
  default: assert(false);
  }

  if (new_state == FIELD_INVALID) {
    err << ERR("Reading field %s changed state from %s to %s\n", field_name.c_str(), to_string(old_state), to_string(new_state));
  }

  old_state = new_state;
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_write(MtField* component, MtField* field) {
  Err err;
  if (!field) return err;
  if (field->is_param()) return err;

  method()->fields_written.push_back({component, field});

  std::string field_name = field->name();
  if (component) field_name = component->name() + "." + field_name;

  auto field_path = _path_stack.back() + "." + field_name;
  auto& old_state = (*state_top)[field_path];
  FieldState new_state;

  switch(old_state) {
  case FIELD_NONE     : new_state = FIELD_OUTPUT;   break;
  case FIELD_INPUT    : new_state = FIELD_REGISTER; break;
  case FIELD_OUTPUT   : new_state = FIELD_OUTPUT;   break;
  case FIELD_SIGNAL   : new_state = FIELD_INVALID;  break;
  case FIELD_REGISTER : new_state = FIELD_REGISTER; break;
  case FIELD_INVALID  : new_state = FIELD_INVALID;  break;
  default: assert(false);
  }

  if (new_state == FIELD_INVALID) {
    err << ERR("Writing field %s changed state from %s to %s\n", field_name.c_str(), to_string(old_state), to_string(new_state));
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
