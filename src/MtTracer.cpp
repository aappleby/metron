#include "MtTracer.h"

#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "metron_tools.h"

//-----------------------------------------------------------------------------

FieldState merge_delta(FieldState a, FieldDelta b) {
  assert(a >= 0 && a < FIELD_MAX);
  assert(b >= 0 && b < DELTA_MAX);

  static const FieldState field_table[FIELD_MAX][DELTA_MAX] = {
    /*                     DELTA_RD,      DELTA_WR,      DELTA_WS,      DELTA_EF, */
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
  static_assert(sizeof(field_table) == sizeof(FieldState) * FIELD_MAX * DELTA_MAX);

  auto r = field_table[a][b];
  return r;
}

//-----------------------------------------------------------------------------

FieldState merge_parallel(FieldState a, FieldState b) {
  assert(a >= 0 && a < FIELD_MAX);
  assert(b >= 0 && b < FIELD_MAX);

  static const FieldState field_table[FIELD_MAX][FIELD_MAX] = {
    /*                     FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, */
    /* FIELD________, */ { FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, },
    /* FIELD____WR__, */ { FIELD____WR__, FIELD____WR__, FIELD____WR_L, FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD____WR_L, */ { FIELD____WR_L, FIELD____WR_L, FIELD____WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_____, */ { FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_WR__, */ { FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_WR_L, */ { FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD____WR__, */ { FIELD____WR__, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, },
    /* FIELD____WS_L, */ { FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD____WS_L, FIELD_INVALID, },
    /* FIELD_INVALID, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
  };
  static_assert(sizeof(field_table) == sizeof(FieldState) * FIELD_MAX * FIELD_MAX);

  auto r = field_table[a][b];
  return r;
}

//-----------------------------------------------------------------------------

FieldState merge_series(FieldState a, FieldState b) {
  assert(a >= 0 && a < FIELD_MAX);
  assert(b >= 0 && b < FIELD_MAX);

  static const FieldState field_table[FIELD_MAX][FIELD_MAX] = {
    /*                     FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, */
    /* FIELD________, */ { FIELD________, FIELD____WR__, FIELD____WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, },
    /* FIELD____WR__, */ { FIELD____WR__, FIELD____WR__, FIELD____WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD____WR_L, */ { FIELD____WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_____, */ { FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_RD_____, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_WR__, */ { FIELD_RD_WR__, FIELD_RD_WR__, FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_RD_WR_L, */ { FIELD_RD_WR_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD____WR__, */ { FIELD____WR__, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD____WR__, FIELD____WS_L, FIELD_INVALID, },
    /* FIELD____WS_L, */ { FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD____WS_L, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
    /* FIELD_INVALID, */ { FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, FIELD_INVALID, },
  };
  static_assert(sizeof(field_table) == sizeof(FieldState) * FIELD_MAX * FIELD_MAX);

  auto r = field_table[a][b];

  if (r == FIELD_INVALID) {
    LOG_R("merge_series produced FIELD_INVALID\n");
  }

  return r;
}

//-----------------------------------------------------------------------------

void merge_parallel(state_map& ma, state_map& mb, state_map& out) {
  std::set<std::string> keys;

  for (const auto& a : ma) keys.insert(a.first);
  for (const auto& b : mb) keys.insert(b.first);

  state_map temp;
  for (const auto& key : keys) {
    auto sa = ma[key];
    auto sb = mb[key];
    temp[key] = merge_parallel(sa, sb);
  }

  out.swap(temp);
}

void merge_series(state_map& ma, state_map& mb, state_map& out) {
  std::set<std::string> keys;

  for (const auto& a : ma) keys.insert(a.first);
  for (const auto& b : mb) keys.insert(b.first);

  state_map temp;
  for (const auto& key : keys) {
    auto sa = ma[key];
    auto sb = mb[key];
    temp[key] = merge_series(sa, sb);
  }

  out.swap(temp);
}

//------------------------------------------------------------------------------

void MtTracer::trace_dispatch(MnNode n) {
  if (!n.is_named()) return;

  switch (n.sym) {
    case sym_assignment_expression:
      trace_assign(n);
      break;
    case sym_call_expression:
      trace_call(n);
      break;
    case sym_field_expression:
      trace_field(n);
      break;
    case sym_identifier:
      trace_id(n);
      break;
    case sym_if_statement:
      trace_if(n);
      break;
    case sym_switch_statement:
      trace_switch(n);
      break;

    case sym_declaration:
      // do we want to do anything special for this node, like note the
      // existence of the local somewhere?
      trace_children(n);
      break;

    case sym_init_declarator:
      // Skip the lhs
      trace_dispatch(n.get_field(field_value));
      break;

    case sym_conditional_expression:
      trace_ternary(n);
      break;

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
      trace_children(n);
      break;

    case sym_subscript_expression:  
      // I don't think we need to do anything special?
      trace_children(n);
      break;

    default:
      LOG_R("Don't know what to do with %s\n", n.ts_node_type());
      n.dump_tree();
      debugbreak();
      // trace_children(n);
      break;
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_children(MnNode n) {
  for (auto c : n) {
    trace_dispatch(c);
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_assign(MnNode n) {

  auto node_lhs = n.get_field(field_left);
  auto node_rhs = n.get_field(field_right);

  trace_dispatch(node_rhs);

  if (node_lhs.sym == sym_identifier) {
    auto node_name = node_lhs;
    if (mod()->get_field(node_name.text())) {
      trace_write(node_name);
    }
  }
  else if (node_lhs.sym == sym_subscript_expression) {
    auto node_name = node_lhs.get_field(field_argument);
    if (mod()->get_field(node_name.text())) {
      trace_write(node_name);
    }
  }
  else {
    node_lhs.dump_tree();
    debugbreak();
  }

}

//------------------------------------------------------------------------------
// FIXME I need to traverse the args before stepping into the call

void MtTracer::trace_call(MnNode n) {
  // n.dump_tree();

  // Trace the args first.
  trace_dispatch(n.get_field(field_arguments));

  //printf("Tracing call %s.%s -> %s\n", mod()->name().c_str(), method()->name().c_str(), node_func.text().c_str());

  // New state map goes on the stack.
  state_map state_call;
  _state_stack.push_back(&state_call);

  auto node_func = n.get_field(field_function);
  if (node_func.sym == sym_field_expression) {
    trace_submod_call(n);
  } else if (node_func.sym == sym_identifier) {
    trace_method_call(n);
  } else if (node_func.sym == sym_template_function) {
    trace_template_call(n);
  } else {
    LOG_R("MtModule::build_call_tree - don't know what to do with %s\n",
          n.ts_node_type());
    n.dump_tree();
    debugbreak();
  }

  _state_stack.pop_back();

  // Call traced, close the state map and merge.

  for (auto& pair : state_top()) {
    auto old_state = pair.second;
    auto new_state = merge_delta(old_state, DELTA_EF);

    if (new_state == FIELD_INVALID) {
      LOG_R("Field %s was %s, now %s!\n", pair.first.c_str(), to_string(old_state), to_string(new_state));
      //LOG_R("========== call stack ==========\n");
      //for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      //  printf("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
      //}
      //LOG_R("========== source ==========\n");
      //n.dump_source_lines();
      //LOG_R("============================\n");
    }

    pair.second = new_state;
  }

  merge_series(state_top(), state_call, state_top());
  dump_trace();
}

//------------------------------------------------------------------------------

void MtTracer::trace_method_call(MnNode n) {

  // Now trace the call.

  auto node_func = n.get_field(field_function);

  auto sibling_method = mod()->get_method(node_func.text());

  if (sibling_method) {
    _method_stack.push_back(sibling_method);
    trace_dispatch(sibling_method->node.get_field(field_body));
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
}

//------------------------------------------------------------------------------

void MtTracer::trace_submod_call(MnNode n) {
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

  _field_stack.push_back(submod);
  _mod_stack.push_back(submod_mod);
  _method_stack.push_back(submod_method);
  trace_dispatch(submod_method->node.get_field(field_body));
  _method_stack.pop_back();
  _mod_stack.pop_back();
  _field_stack.pop_back();
}

//------------------------------------------------------------------------------

void MtTracer::trace_template_call(MnNode n) {

  auto node_name = n.get_field(field_function).get_field(field_name).text();

  if (node_name == "bx" || node_name == "dup") {
  }
  else {
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
}

//------------------------------------------------------------------------------

void MtTracer::trace_field(MnNode n) {
  n.dump_tree();
  debugbreak();
}

//------------------------------------------------------------------------------

void MtTracer::trace_id(MnNode n) {
  auto field = mod()->get_field(n.text());

  if (field) {
    trace_read(n);
    // n.dump_tree();
    // debugbreak();
  } else {
    // Either a param or a local variable, ignore.

    // n.dump_tree();
    // debugbreak();
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_if(MnNode n) {
  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  state_map branch_a;
  state_map branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  // FIXME - Does series-parallel behave differently than parallel-series?
  // I don't think so.

  merge_series(state_top(), branch_a, branch_a);
  merge_series(state_top(), branch_b, branch_b);
  merge_parallel(branch_a, branch_b, state_top());

  return;
}

//------------------------------------------------------------------------------

void MtTracer::trace_switch(MnNode n) {

  state_map old_state = state_top();

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (const auto& c : body) {
    if (c.sym == sym_case_statement) {
      if (first_branch) {
        trace_dispatch(c);
      } else {
        state_map state_case = old_state;

        _state_stack.push_back(&state_case);
        trace_dispatch(c);
        _state_stack.pop_back();

        merge_parallel(state_top(), state_case, state_top());
        dump_trace();
      }
    }
  }
}

//------------------------------------------------------------------------------
// FIXME this is identical to trace_if, should we merge the two?

void MtTracer::trace_ternary(MnNode n) {
  auto node_cond = n.get_field(field_condition);
  auto node_branch_a = n.get_field(field_consequence);
  auto node_branch_b = n.get_field(field_alternative);

  trace_dispatch(node_cond);

  // FIXME - Does preloading branch_a/b with the current state change anything?
  // I don't think so.

  state_map branch_a;
  state_map branch_b;

  if (!node_branch_a.is_null()) {
    _state_stack.push_back(&branch_a);
    trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  if (!node_branch_b.is_null()) {
    _state_stack.push_back(&branch_b);
    trace_dispatch(node_branch_a);
    _state_stack.pop_back();
  }

  // FIXME - Does series-parallel behave differently than parallel-series?
  // I don't think so.

  merge_series(state_top(), branch_a, branch_a);
  merge_series(state_top(), branch_b, branch_b);
  merge_parallel(branch_a, branch_b, state_top());

  return;
}

//------------------------------------------------------------------------------

void MtTracer::trace_read(MnNode const& n) {
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  auto old_state = state_top()[field_name];
  auto new_state = merge_delta(old_state, DELTA_RD);

  if (new_state == FIELD_INVALID) {
    LOG_R("Field %s was %s, now %s!\n", field_name.c_str(), to_string(old_state), to_string(new_state));
    LOG_R("========== call stack ==========\n");
    for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      printf("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  state_top()[field_name] = new_state;
  dump_trace();
}

//------------------------------------------------------------------------------

void MtTracer::trace_write(MnNode const& n) {
  assert(in_tick() || in_tock());
  std::string field_name = n.text();

  for (int i = (int)_field_stack.size() - 1; i >= 0; i--) {
    field_name = _field_stack[i]->name() + "." + field_name;
  }

  auto old_state = state_top()[field_name];
  auto new_state = merge_delta(old_state, in_tick() ? DELTA_WR : DELTA_WS);

  if (new_state == FIELD_INVALID) {
    LOG_R("Field %s was %s, now %s!\n", field_name.c_str(), to_string(old_state), to_string(new_state));
    LOG_R("========== call stack ==========\n");
    for (auto it = _method_stack.rbegin(); it != _method_stack.rend(); ++it) {
      printf("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  state_top()[field_name] = new_state;
  dump_trace();
}

//------------------------------------------------------------------------------

void MtTracer::trace_end_fn() { 
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
      printf("%s.%s\n", (*it)->mod->name().c_str(), (*it)->name().c_str());
    }
    LOG_R("========== source ==========\n");
    n.dump_source_lines();
    LOG_R("============================\n");
  }

  state_top()[field_name] = new_state;
#endif
}

//------------------------------------------------------------------------------

void MtTracer::dump_trace() {
  printf("//----------\n");
  printf("// Trace\n");
  for (const auto& pair : state_top()) {
    printf("%s = %s\n", pair.first.c_str(), to_string(pair.second));
  }
  printf("//----------\n");
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
