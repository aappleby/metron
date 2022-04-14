#include "MtTracer.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "metron_tools.h"

//-----------------------------------------------------------------------------

FieldState merge_delta(FieldState a, FieldDelta b) {
  if (a < 0 || a >= FIELD_MAX) return FIELD_INVALID;
  if (b < 0 || b >= DELTA_MAX) return FIELD_INVALID;

  // clang-format off
  static const FieldState field_table[FIELD_MAX][DELTA_MAX] = {
      /*                     DELTA_RD,      DELTA_WR,      DELTA_WS,      DELTA_EF,   */
      /* FIELD________, */ { FIELD_RD_____, FIELD____WR__, FIELD____WS__, FIELD________, },
      /* FIELD____WR__, */ { FIELD_INVALID, FIELD____WR__, FIELD_INVALID, FIELD____WR_L, },
      /* FIELD____WR_L, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD____WR_L, },
      /* FIELD_RD_____, */ { FIELD_RD_____, FIELD_RD_WR__, FIELD_INVALID, FIELD_RD_____, },
      /* FIELD_RD_WR__, */ { FIELD_INVALID, FIELD_RD_WR__, FIELD_INVALID, FIELD_RD_WR_L, },
      /* FIELD_RD_WR_L, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_RD_WR_L, },
      /* FIELD____WS__, */ { FIELD____WS_L, FIELD_INVALID, FIELD____WS__, FIELD____WS_L, },
      /* FIELD____WS_L, */ { FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, },
      /* FIELD_INVALID, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
  };
  // clang-format on
  static_assert(sizeof(field_table) ==
                sizeof(FieldState) * FIELD_MAX * DELTA_MAX);

  auto r = field_table[a][b];
  return r;
}

//-----------------------------------------------------------------------------

FieldState merge_parallel(FieldState a, FieldState b) {
  if (a < 0 || a >= FIELD_MAX) return FIELD_INVALID;
  if (b < 0 || b >= FIELD_MAX) return FIELD_INVALID;

  // clang-format off
  static const FieldState field_table[FIELD_MAX][FIELD_MAX] = {
      /*                     FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, */
      /* FIELD________, */ { FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, },
      /* FIELD____WR__, */ { FIELD____WR__, FIELD____WR__, FIELD____WR_L, FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD____WR_L, */ { FIELD____WR_L, FIELD____WR_L, FIELD____WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_____, */ { FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_WR__, */ { FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_WR_L, */ { FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD____WS__, */ { FIELD____WS__, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, },
      /* FIELD____WS_L, */ { FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD____WS_L, FIELD_INVALID, },
      /* FIELD_INVALID, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
  };
  // clang-format on
  static_assert(sizeof(field_table) ==
                sizeof(FieldState) * FIELD_MAX * FIELD_MAX);

  auto r = field_table[a][b];
  return r;
}

//-----------------------------------------------------------------------------

FieldState merge_series(FieldState a, FieldState b) {
  if (a < 0 || a >= FIELD_MAX) return FIELD_INVALID;
  if (b < 0 || b >= FIELD_MAX) return FIELD_INVALID;

  // clang-format off
  static const FieldState field_table[FIELD_MAX][FIELD_MAX] = {
      /*                     FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, */
      /* FIELD________, */ { FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, },
      /* FIELD____WR__, */ { FIELD____WR__, FIELD____WR__, FIELD____WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD____WR_L, */ { FIELD____WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_____, */ { FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_WR__, */ { FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_RD_WR_L, */ { FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD____WS__, */ { FIELD____WS__, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD____WS__, FIELD____WS_L, FIELD_INVALID, },
      /* FIELD____WS_L, */ { FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
      /* FIELD_INVALID, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
  };
  // clang-format on

  static_assert(sizeof(field_table) ==
                sizeof(FieldState) * FIELD_MAX * FIELD_MAX);

  auto r = field_table[a][b];

  if (r == FIELD_INVALID) {
    // LOG_R("merge_series produced FIELD_INVALID\n");
  }

  return r;
}

//-----------------------------------------------------------------------------

CHECK_RETURN Err merge_parallel(MtStateMap & ma, MtStateMap & mb, MtStateMap & out) {
  Err err;
  std::set<std::string> keys;

  if (ma.hit_return != mb.hit_return) {
    err << ERR("MtTracer::merge_parallel - one branch returned, one branch didn't");
  }

  for (const auto& a : ma.s) keys.insert(a.first);
  for (const auto& b : mb.s) keys.insert(b.first);

  MtStateMap temp;
  for (const auto& key : keys) {
    auto sa = ma.s.contains(key) ? ma.s[key] : FIELD________;
    auto sb = mb.s.contains(key) ? mb.s[key] : FIELD________;
    auto sm = merge_parallel(sa, sb);
    if (sm == FIELD_INVALID) {
      err << ERR("%s: %s || %s = %s\n", key.c_str(), to_string(sa), to_string(sb),
            to_string(sm));
    }
    temp.s[key] = sm;
  }

  out.s.swap(temp.s);

  return err;
}

CHECK_RETURN Err merge_series(MtStateMap& ma, MtStateMap& mb, MtStateMap& out) {
  Err err;
  std::set<std::string> keys;

  assert(!ma.hit_return);

  for (const auto& a : ma.s) keys.insert(a.first);
  for (const auto& b : mb.s) keys.insert(b.first);

  MtStateMap temp;
  for (const auto& key : keys) {
    auto sa = ma.s.contains(key) ? ma.s[key] : FIELD________;
    auto sb = mb.s.contains(key) ? mb.s[key] : FIELD________;
    auto sm = merge_series(sa, sb);
    if (sm == FIELD_INVALID) {
      err << ERR("%s: %s -> %s = %s\n", key.c_str(), to_string(sa), to_string(sb),
            to_string(sm));
    }
    temp.s[key] = sm;
  }

  out.s.swap(temp.s);
  out.hit_return = mb.hit_return;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MnNode n) {
  Err err;

  if (!n.is_named()) return err;

  if (state_top().hit_return) {
    err << ERR("Return in the middle of a function");
  }

  switch (n.sym) {
    case sym_assignment_expression:
      err << trace_assign(n);
      break;
    case sym_call_expression:
      err << trace_call(n);
      break;
    case sym_field_expression:
      err << trace_field(n);
      break;
    case sym_identifier:
      err << trace_id(n);
      break;
    case sym_if_statement:
      err << trace_if(n);
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
      err << trace_ternary(n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      auto arg = n.get_field(field_argument);
      if (arg.sym == sym_identifier) {
        auto field = mod()->get_field(arg.text());
        if (field) {
          err << trace_read(arg);
          err << trace_write(arg);
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
      // do-nothing nodes
      break;

    case sym_return_statement:
      // This trace path has hit a return, 
      err << trace_children(n);
      state_top().hit_return = true;
      break;

    case sym_argument_list:
    case sym_parenthesized_expression:
    case sym_binary_expression:
    case sym_compound_statement:
    case sym_case_statement:
    case sym_expression_statement:
    case sym_condition_clause:
    case sym_unary_expression:
    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
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

CHECK_RETURN Err MtTracer::trace_children(MnNode n) {
  Err err;
  for (const auto& c : n) {
    err << trace_dispatch(c);
  }
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_assign(MnNode n) {
  Err err;

  auto node_lhs = n.get_field(field_left);
  auto node_rhs = n.get_field(field_right);

  err << trace_dispatch(node_rhs);

  if (node_lhs.sym == sym_identifier) {
    auto node_name = node_lhs;
    if (mod()->get_field(node_name.text())) {
      err << trace_write(node_name);
    }
  } else if (node_lhs.sym == sym_subscript_expression) {
    auto node_name = node_lhs.get_field(field_argument);
    if (mod()->get_field(node_name.text())) {
      err << trace_write(node_name);
    }
  }
  else if (node_lhs.sym == sym_field_expression) {
    err << trace_write(node_lhs.text());
  }
  else {
    debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_call(MnNode n) {
  Err err;

  // Trace the args first.
  err << trace_dispatch(n.get_field(field_arguments));

  // New state map goes on the stack.
  MtStateMap state_call;
  _state_stack.push_back(&state_call);

  auto node_func = n.get_field(field_function);
  if (node_func.sym == sym_field_expression) {
    err << trace_submod_call(n);
  } else if (node_func.sym == sym_identifier) {
    err << trace_method_call(n);
  } else if (node_func.sym == sym_template_function) {
    err << trace_template_call(n);
  } else {
    err << ERR("MtModule::build_call_tree - don't know what to do with %s\n",
          n.ts_node_type());
  }

  _state_stack.pop_back();

  if (err.has_err()) {
    //LOG_R("MtTracer::trace_call failed\n");
    return err;
  }

  // Call traced, close the state map and merge.

  state_call.hit_return = false;
  for (auto& pair : state_call.s) {
    auto old_state = pair.second;
    auto new_state = merge_delta(old_state, DELTA_EF);

    if (new_state == FIELD_INVALID) {
      err << ERR("Field %s was %s, now %s!\n", pair.first.c_str(),
            to_string(old_state), to_string(new_state));
      break;
    }

    pair.second = new_state;
  }

  err << merge_series(state_top(), state_call, state_top());
  if (err.has_err()) {
    LOG_R(
        "MtTracer::trace_call failed - caller state cannot be concatenated "
        "with callee state\n");
    n.dump_source_lines();
    LOG_R("Caller state -\n");
    dump_trace(state_top());
    LOG_R("Callee state - \n");
    dump_trace(state_call);
    dump_stack();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_method_call(MnNode n) {
  Err err;

  auto node_func = n.get_field(field_function);

  auto src_method = method();
  auto dst_method = mod()->get_method(node_func.text());

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

  //----------------------------------------

  bool requires_input_binding  = dst_method->is_public && dst_method->params.size();

  // We must be either in a tick or a tock, or Metron is broken.
  assert(in_tick() ^ in_tock());

  // Methods that require input port bindings cannot be called from inside the module.
  if (requires_input_binding) {
    err << ERR("Method %s requires input port bindings and can only be called from outside the module.\n", dst_method->name().c_str());
  }

  // Functions can only call other functions.
  if (src_method->is_func && !dst_method->is_func) {
    err << ERR("Can't call non-function %s from function %s.\n", dst_method->name().c_str(), src_method->name().c_str());
  }

  // Public methods with params require input port bindings to call. If we're in a tick(), we can't do that.
  if (in_tick() && requires_input_binding) {
    err << ERR("Can't call %s from a tick() as it requires input port bindings, and port bindings can only be in tock()s.\n", dst_method->name().c_str());
  }

  // Tock methods write signals and can only do so from inside another tock(). If we're in a tick, that's bad.
  if (in_tick() && dst_method->is_tock) {
    err << ERR("Can't call %s from tick method %s\n", dst_method->name().c_str(), src_method->name().c_str());
  }

  //----------------------------------------
  // OK, the call should be valid. If the method requires port bindings, build the port names from the current field stack plus the method name.

  std::string method_port_base = dst_method->name();
  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    method_port_base = _field_stack[i]->name() + "." + method_port_base;
  }

  // Trace input port bindings as if they are fields being written to.

  if (requires_input_binding) {
    for (const auto& param : dst_method->params) {
      std::string port_name = method_port_base + "." + param;
      err << trace_write(port_name);
      err << trace_read(port_name);
    }
  }

  // Now trace the method.

  _method_stack.push_back(dst_method);
  err << trace_dispatch(dst_method->node.get_field(field_body));
  _method_stack.pop_back();

  //----------------------------------------
  // Done.

  if (err.has_err()) {
    err << ERR("MtTracer::trace_method_call failed @\n");
    n.dump_source_lines();
  }

  return err;
}

//------------------------------------------------------------------------------
// Submodule call "submod.method()". Pull up the submodule and traverse into the method.

CHECK_RETURN Err MtTracer::trace_submod_call(MnNode n) {
  Err err;

  auto node_func = n.get_field(field_function);
  auto submod_field_name  = node_func.get_field(field_argument).text();
  auto submod_method_name = node_func.get_field(field_field).text();

  auto submod_field = mod()->get_submod(submod_field_name);
  if (!submod_field) {
    err << ERR("Could not find submodule %s\n", submod_field_name.c_str());
  }

  auto submod_type = submod_field->type_name();
  auto submod_mod = mod()->source_file->lib->get_module(submod_type);

  if (!submod_mod) {
    err << ERR("Could not find module %s for submod %s\n", submod_type.c_str(), submod_field_name.c_str());
  }

  auto src_method = method();
  auto dst_method = submod_mod->get_method(submod_method_name);

  if (!dst_method) {
    err << ERR("Submodule %s has no method %s\n", submod_field_name.c_str(), submod_method_name.c_str());
  }

  //----------------------------------------
  // The target must be a public tick or tock.

  if (dst_method->is_private || (!dst_method->is_tick && !dst_method->is_tock)) {
    err << ERR("Method %s is not a public tick or tock, it can't be called from outside the module.\n", submod_method_name.c_str());
    n.dump_source_lines();
  }

  // Ticks/tocks with params require input port bindings to call. If we're in a tick(), we can't do that.
  
  if (in_tick() && dst_method->params.size()) {
    err << ERR("Can't call %s from a tick() as it requires a port binding, and port bindings can only be in tock()s.\n", dst_method->name());
    n.dump_source_lines();
  }

  // Non-const tock methods write signals and can only do so from inside another tock(). If we're in a tick, that's bad.
  
  if (in_tick() && dst_method->is_tock && !dst_method->is_const) {
    err << ERR("Can't call non-const %s from tick method %s\n", dst_method->name().c_str(), src_method->name().c_str());
    n.dump_source_lines();
  }

  //----------------------------------------
  // OK, the call should be valid. If the method requires port bindings, build the port names from the current field stack plus the method name.

  std::string method_port_base = submod_field_name + "." + dst_method->name();
  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    method_port_base = _field_stack[i]->name() + "." + method_port_base;
  }

  // If we're calling a method that has params, trace the params as if they were input ports.
  // We trace them as both write and read as the current method is writing them and the submod, presumably, is reading them.

  for (const auto& param : dst_method->params) {
    std::string port_name = method_port_base + "." + param;
    err << trace_write(port_name);
    err << trace_read(port_name);
  }

  // Now trace the method.

  _field_stack.push_back(submod_field);
  _mod_stack.push_back(submod_mod);
  _method_stack.push_back(dst_method);
  err << trace_dispatch(dst_method->node.get_field(field_body));
  _method_stack.pop_back();
  _mod_stack.pop_back();
  _field_stack.pop_back();

  //----------------------------------------
  // Done.

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_template_call(MnNode n) {
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
    err << ERR("MtTracer::trace_template_call failed @\n");
    n.dump_source_lines();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_field(MnNode n) {
  Err err;
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  err << trace_read(field_name);
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_id(MnNode n) {
  Err err;

  auto field = mod()->get_field(n.text());

  if (field && !field->is_param()) {
    err << trace_read(n);
    // debugbreak();
  } else {
    // Either a param or a local variable, ignore.
    // debugbreak();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_if(MnNode n) {
  Err err;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  err << trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  MtStateMap branch_a;
  MtStateMap branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    err << trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    err << trace_dispatch(node_branch_b);
    _state_stack.pop_back();
  }

  // FIXME - Does series-parallel behave differently than parallel-series?
  // YES IT DOES - if the branch below is merge-parallel'd before the
  // merge-series, it ends up invalid because we merge a FIELD_RD_____ with a
  // FIELD____WS__
  //
  // But the paths through the whole thing are write-read and write-write, both
  // of which are valid.

  // sig = 1;
  // if (blah) {
  //   y = sig;
  // } else {
  //   sig = 2;
  // }

  err << merge_series(state_top(), branch_a, branch_a);
  err << merge_series(state_top(), branch_b, branch_b);
  err << merge_parallel(branch_a, branch_b, state_top());

  if (err.has_err()) {
    err << ERR("MtTracer::trace_if - error\n");
    n.dump_source_lines();
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MnNode n) {
  Err err;

  err << trace_dispatch(n.get_field(field_condition));

  MtStateMap old_state = state_top();

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
        MtStateMap state_case = old_state;

        _state_stack.push_back(&state_case);
        err << trace_dispatch(c);
        _state_stack.pop_back();

        err << merge_parallel(state_top(), state_case, state_top());
      }
    }
  }

  if (err.has_err()) {
    err << ERR("MtTracer::trace_switch failed @\n");
    n.dump_source_lines();
  }

  return err;
}

//------------------------------------------------------------------------------
// FIXME this is identical to trace_if, should we merge the two?

CHECK_RETURN Err MtTracer::trace_ternary(MnNode n) {
  Err err;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  err << trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  MtStateMap branch_a;
  MtStateMap branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    err << trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    err << trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  // FIXME - Does series-parallel behave differently than parallel-series?
  // I don't think so.

  err << merge_series(state_top(), branch_a, branch_a);
  err << merge_series(state_top(), branch_b, branch_b);
  err << merge_parallel(branch_a, branch_b, state_top());

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_read(const std::string& field_name) {
  Err err;

  auto old_state = state_top().s[field_name];
  auto new_state = merge_delta(old_state, DELTA_RD);

  if (new_state == FIELD_INVALID) {
    err << ERR("Field %s was %s, now %s!\n", field_name.c_str(),
          to_string(old_state), to_string(new_state));
  }

  state_top().s[field_name] = new_state;
  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_write(const std::string& field_name) {
  Err err;

  // Don't assert this, calls to submod functions with args are considered a
  // signal write
  //assert(in_tick() || in_tock());

  auto old_state = state_top().s[field_name];
  auto new_state = merge_delta(old_state, in_tick() ? DELTA_WR : DELTA_WS);

  if (new_state == FIELD_INVALID) {
    err << ERR("Field %s was %s, now %s!\n", field_name.c_str(),
          to_string(old_state), to_string(new_state));
  }

  state_top().s[field_name] = new_state;

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_read(MnNode const& n) {
  Err err;

  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  return err << trace_read(field_name);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_write(MnNode const& n) {
  Err err;

  assert(in_tick() || in_tock());
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  return err << trace_write(field_name);
}

//------------------------------------------------------------------------------

// KCOV_OFF
void MtTracer::dump_trace(MtStateMap& m) {
  for (const auto& pair : m.s) {
    LOG_Y("%s = %s\n", pair.first.c_str(), to_string(pair.second));
  }
}
// KCOV_ON

//------------------------------------------------------------------------------

// KCOV_OFF
void MtTracer::dump_stack() {
  LOG_B("Field Stack:\n");
  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    auto f = _field_stack[i];
    LOG_G("%s\n", f->name().c_str());
  }
  LOG_B("\n");

  LOG_B("Mod Stack:\n");
  for (int i = (int)_mod_stack.size() - 1; i >= 0; i--) {
    auto m = _mod_stack[i];
    LOG_G("%s\n", m->name().c_str());
  }
  LOG_B("\n");

  LOG_B("Method Stack:\n");
  for (int i = (int)_method_stack.size() - 1; i >= 0; i--) {
    auto m = _method_stack[i];
    LOG_G("%s\n", m->name().c_str());
  }
  LOG_B("\n");
}
// KCOV_ON

//------------------------------------------------------------------------------

bool MtTracer::in_tick() const {
  for (int i = (int)_method_stack.size() - 1; i >= 0; i--) {
    auto method = _method_stack[i];
    if (method->is_tock) return false;
    if (method->is_tick) return true;
  }
  return false;
}

//------------------------------------------------------------------------------

bool MtTracer::in_tock() const {
  for (int i = (int)_method_stack.size() - 1; i >= 0; i--) {
    auto method = _method_stack[i];
    if (method->is_tock) return true;
    if (method->is_tick) return false;
  }
  return false;
}

//------------------------------------------------------------------------------

bool MtTracer::ends_with_break(MnNode n) {
  switch(n.sym) {
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
    case sym_compound_statement:
    {
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
