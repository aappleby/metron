#include "MtTracer.h"

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

CHECK_RETURN Err merge_parallel(state_map& ma, state_map& mb, state_map& out) {
  Err error;
  std::set<std::string> keys;

  for (const auto& a : ma) keys.insert(a.first);
  for (const auto& b : mb) keys.insert(b.first);

  state_map temp;
  for (const auto& key : keys) {
    auto sa = ma.contains(key) ? ma[key] : FIELD________;
    auto sb = mb.contains(key) ? mb[key] : FIELD________;
    auto sm = merge_parallel(sa, sb);
    if (sm == FIELD_INVALID) {
      LOG_R("%s: %s || %s = %s\n", key.c_str(), to_string(sa), to_string(sb),
            to_string(sm));
      error = true;
      return error;
    }
    temp[key] = sm;
  }

  out.swap(temp);
  return error;
}

CHECK_RETURN Err merge_series(state_map& ma, state_map& mb, state_map& out) {
  Err error;
  std::set<std::string> keys;

  for (const auto& a : ma) keys.insert(a.first);
  for (const auto& b : mb) keys.insert(b.first);

  state_map temp;
  for (const auto& key : keys) {
    auto sa = ma.contains(key) ? ma[key] : FIELD________;
    auto sb = mb.contains(key) ? mb[key] : FIELD________;
    auto sm = merge_series(sa, sb);
    if (sm == FIELD_INVALID) {
      LOG_R("%s: %s -> %s = %s\n", key.c_str(), to_string(sa), to_string(sb),
            to_string(sm));
      error = true;
      return error;
    }
    temp[key] = sm;
  }

  out.swap(temp);
  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_dispatch(MnNode n) {
  Err error;

  if (!n.is_named()) return error;

  switch (n.sym) {
    case sym_assignment_expression:
      // n.dump_source_lines();
      error |= trace_assign(n);
      break;
    case sym_call_expression:
      error |= trace_call(n);
      break;
    case sym_field_expression:
      error |= trace_field(n);
      break;
    case sym_identifier:
      error |= trace_id(n);
      break;
    case sym_if_statement:
      error |= trace_if(n);
      break;
    case sym_switch_statement:
      error |= trace_switch(n);
      break;

    case sym_declaration:
      // do we want to do anything special for this node, like note the
      // existence of the local somewhere?
      error |= trace_children(n);
      break;

    case sym_init_declarator:
      // Skip the lhs
      error |= trace_dispatch(n.get_field(field_value));
      break;

    case sym_conditional_expression:
      error |= trace_ternary(n);
      break;

    case sym_update_expression: {
      // this is "i++" or similar, which is a read and a write.
      auto arg = n.get_field(field_argument);
      if (arg.sym == sym_identifier) {
        auto field = mod()->get_field(arg.text());
        if (field) {
          error |= trace_read(arg);
          error |= trace_write(arg);
        }
        else {
          // local variable increment
        }
      }
      else {
        LOG_R("Not sure how to increment a %s\n", n.ts_node_type());
        error = true;
      }
      break;
    }

    case sym_primitive_type:
    case sym_template_type:
    case sym_number_literal:
    case sym_using_declaration:
    case sym_break_statement:
    case sym_comment:
      // do-nothing nodes
      break;

    case sym_argument_list:
    case sym_parenthesized_expression:
    case sym_binary_expression:
    case sym_compound_statement:
    case sym_return_statement:
    case sym_case_statement:
    case sym_expression_statement:
    case sym_condition_clause:
    case sym_unary_expression:
    case sym_qualified_identifier:
    case alias_sym_namespace_identifier:
    case sym_for_statement:
      error |= trace_children(n);
      break;

    case sym_subscript_expression:
      // I don't think we need to do anything special?
      error |= trace_children(n);
      break;

    default:
      LOG_R("Don't know what to do with %s\n", n.ts_node_type());
      error = true;
      break;
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_children(MnNode n) {
  Err error;
  for (const auto& c : n) {
    error |= trace_dispatch(c);
    if (error) return error;
  }
  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_assign(MnNode n) {
  Err error;

  auto node_lhs = n.get_field(field_left);
  auto node_rhs = n.get_field(field_right);

  error |= trace_dispatch(node_rhs);

  if (node_lhs.sym == sym_identifier) {
    auto node_name = node_lhs;
    if (mod()->get_field(node_name.text())) {
      error |= trace_write(node_name);
    }
  } else if (node_lhs.sym == sym_subscript_expression) {
    auto node_name = node_lhs.get_field(field_argument);
    if (mod()->get_field(node_name.text())) {
      error |= trace_write(node_name);
    }
  } else {
    node_lhs.dump_tree();
    debugbreak();
  }

  return error;
}

//------------------------------------------------------------------------------
// FIXME I need to traverse the args before stepping into the call

CHECK_RETURN Err MtTracer::trace_call(MnNode n) {
  Err error;

  // Trace the args first.
  error |= trace_dispatch(n.get_field(field_arguments));

  // New state map goes on the stack.
  state_map state_call;
  _state_stack.push_back(&state_call);

  auto node_func = n.get_field(field_function);
  if (node_func.sym == sym_field_expression) {
    error |= trace_submod_call(n);
  } else if (node_func.sym == sym_identifier) {
    error |= trace_method_call(n);
  } else if (node_func.sym == sym_template_function) {
    error |= trace_template_call(n);
  } else {
    LOG_R("MtModule::build_call_tree - don't know what to do with %s\n",
          n.ts_node_type());
    error = true;
  }

  _state_stack.pop_back();

  if (error) {
    LOG_R("MtTracer::trace_call failed\n");
    return error;
  }

  // Call traced, close the state map and merge.

  for (auto& pair : state_call) {
    auto old_state = pair.second;
    auto new_state = merge_delta(old_state, DELTA_EF);

    if (new_state == FIELD_INVALID) {
      LOG_R("Field %s was %s, now %s!\n", pair.first.c_str(),
            to_string(old_state), to_string(new_state));
      error = true;
      break;
    }

    pair.second = new_state;
  }

  error |= merge_series(state_top(), state_call, state_top());
  if (error) {
    LOG_R(
        "MtTracer::trace_call failed - caller state cannot be concatenated "
        "with callee state\n");
    n.dump_source_lines();
    LOG_R("Caller state -\n");
    dump_trace(state_top());
    LOG_R("Callee state - \n");
    dump_trace(state_call);
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_method_call(MnNode n) {
  Err error;

  auto node_func = n.get_field(field_function);
  auto sibling_method = mod()->get_method(node_func.text());

  if (sibling_method) {
    if (in_tick()) {
      if (sibling_method->is_tock) {
        LOG_R("Calling a tock() while in a tick() method is forbidden\n");
        error = true;
        return error;
      }
    }

    _method_stack.push_back(sibling_method);
    error |= trace_dispatch(sibling_method->node.get_field(field_body));
    _method_stack.pop_back();
  } else {
    // Utility method call like bN()
    /*
    for (int i = 0; i < depth; i++) printf(" ");
    printf("XCALL! %s.%s -> %s\n",
    name().c_str(), method->name().c_str(),
    node_func.text().c_str());
    */
  }

  if (error) {
    LOG_R("MtTracer::trace_method_call failed @\n");
    n.dump_source_lines();
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_submod_call(MnNode n) {
  Err error;

  // Field call. Pull up the submodule and traverse into the method.

  auto node_func = n.get_field(field_function);

  auto submod_name = node_func.get_field(field_argument).text();
  auto submod = mod()->get_submod(submod_name);
  assert(submod);
  auto submod_type = submod->type_name();
  auto submod_mod = mod()->source_file->lib->get_module(submod_type);
  assert(submod_mod);
  auto submod_method =
      submod_mod->get_method(node_func.get_field(field_field).text());
  assert(submod_method);

  if (in_tick()) {
    if (submod_method->is_tock) {
      LOG_R(
          "Calling a tock() on a submodule while in a tick() method is "
          "forbidden\n");
      error = true;
      return error;
    }
  }

  _field_stack.push_back(submod);
  _mod_stack.push_back(submod_mod);
  _method_stack.push_back(submod_method);
  error |= trace_dispatch(submod_method->node.get_field(field_body));
  _method_stack.pop_back();
  _mod_stack.pop_back();
  _field_stack.pop_back();

  if (error) {
    LOG_R("MtTracer::trace_submod_call failed @\n");
    n.dump_source_lines();
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_template_call(MnNode n) {
  Err error;

  auto node_name = n.get_field(field_function).get_field(field_name).text();

  if (node_name == "bx" || node_name == "dup" || node_name == "sign_extend") {
  } else {
    n.dump_tree();
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

  // node_func.dump_tree();
#endif

  if (error) {
    LOG_R("MtTracer::trace_template_call failed @\n");
    n.dump_source_lines();
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_field(MnNode n) {
  Err error;
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  error |= trace_read(field_name);
  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_id(MnNode n) {
  Err error;

  auto field = mod()->get_field(n.text());

  if (field && !field->is_param()) {
    error |= trace_read(n);
    // n.dump_tree();
    // debugbreak();
  } else {
    // Either a param or a local variable, ignore.

    // n.dump_tree();
    // debugbreak();
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_if(MnNode n) {
  Err error;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  error |= trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  state_map branch_a;
  state_map branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    error |= trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    error |= trace_dispatch(node_branch_b);
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

  error |= merge_series(state_top(), branch_a, branch_a);
  error |= merge_series(state_top(), branch_b, branch_b);
  error |= merge_parallel(branch_a, branch_b, state_top());

  if (error) {
    LOG_R("================================================================================\n");
    LOG_R("MtTracer::trace_if - error\n");
    n.dump_source_lines();
    LOG_R("================================================================================\n");
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_switch(MnNode n) {
  Err error;

  state_map old_state = state_top();

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      if (first_branch) {
        error |= trace_dispatch(c);
      } else {
        state_map state_case = old_state;

        _state_stack.push_back(&state_case);
        error |= trace_dispatch(c);
        _state_stack.pop_back();

        error |= merge_parallel(state_top(), state_case, state_top());
      }
    }
  }

  if (error) {
    LOG_R("MtTracer::trace_switch failed @\n");
    n.dump_source_lines();
  }

  return error;
}

//------------------------------------------------------------------------------
// FIXME this is identical to trace_if, should we merge the two?

CHECK_RETURN Err MtTracer::trace_ternary(MnNode n) {
  Err error;

  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  error |= trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  state_map branch_a;
  state_map branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    error |= trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    error |= trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  // FIXME - Does series-parallel behave differently than parallel-series?
  // I don't think so.

  error |= merge_series(state_top(), branch_a, branch_a);
  error |= merge_series(state_top(), branch_b, branch_b);
  error |= merge_parallel(branch_a, branch_b, state_top());

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_read(const std::string& field_name) {
  Err error;

  auto old_state = state_top()[field_name];
  auto new_state = merge_delta(old_state, DELTA_RD);

  if (new_state == FIELD_INVALID) {
    LOG_R("Field %s was %s, now %s!\n", field_name.c_str(),
      to_string(old_state), to_string(new_state));
    error = true;
  }

  state_top()[field_name] = new_state;
  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_write(const std::string& field_name) {
  Err error;

  assert(in_tick() || in_tock());

  auto old_state = state_top()[field_name];
  auto new_state = merge_delta(old_state, in_tick() ? DELTA_WR : DELTA_WS);

  if (new_state == FIELD_INVALID) {
    LOG_R("Field %s was %s, now %s!\n", field_name.c_str(),
      to_string(old_state), to_string(new_state));
    error = true;
  }

  state_top()[field_name] = new_state;

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_read(MnNode const& n) {
  Err error;

  std::string field_name = n.text();
  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  error |= trace_read(field_name);

  if (error) {
    LOG_R("========== call stack ==========\n");
    for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      LOG_R("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer::trace_write(MnNode const& n) {
  Err error;

  assert(in_tick() || in_tock());
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  error |= trace_write(field_name);

  if (error) {
    LOG_R("========== call stack ==========\n");
    for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      LOG_R("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  return error;
}

//------------------------------------------------------------------------------
// FIXME I guess we handled this above?

CHECK_RETURN Err MtTracer::trace_end_fn() {
  Err error;

  assert(in_tick() || in_tock());

#if 0
  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  auto old_state = state_top()[field_name];
  auto new_state = merge_delta(old_state, DELTA_EF);

  if (new_state == FIELD_INVALID) {
    LOG_R("Field %s was %s, now %s!\n", field_name.c_str(), to_string(old_state), to_string(new_state));
    LOG_R("========== call stack ==========\n");
    for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      LOG_R("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  state_top()[field_name] = new_state;
#endif

  return error;
}

//------------------------------------------------------------------------------

void MtTracer::dump_trace(state_map& m) {
  for (const auto& pair : m) {
    LOG_Y("%s = %s\n", pair.first.c_str(), to_string(pair.second));
  }
}

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
