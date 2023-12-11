#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeIf.hpp"
#include "metron/nodes/CNodeFor.hpp"
#include "metron/nodes/CNodeDoWhile.hpp"
#include "metron/nodes/CNodeWhile.hpp"
#include "metron/nodes/CNodeSwitch.hpp"

void dump_parse_tree(CNode* node);


  /*
  if (has_non_terminal_return(node->node_body)) {
    return ERR("Function has mid-block return");
  }
  */

//------------------------------------------------------------------------------

typedef std::function<void(CNodeStatement*)> statement_visitor;

void visit_statements(CNode* node, statement_visitor visit_cb) {
  assert(node);

  if (auto node_compound = node->as<CNodeCompound>()) {
    for (auto statement : node_compound->statements) {
      visit_statements(statement, visit_cb);
    }
  }
  else if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_then) visit_statements(node_if->node_then, visit_cb);
    if (node_if->node_else) visit_statements(node_if->node_else, visit_cb);
  }
  else if (auto node_for = node->as<CNodeFor>()) {
    visit_statements(node_for->child("body"), visit_cb);
  }
  else if (auto node_while = node->as<CNodeWhile>()) {
    visit_statements(node_while->child("body"), visit_cb);
  }
  else if (auto node_dowhile = node->as<CNodeDoWhile>()) {
    visit_statements(node_dowhile->child("body"), visit_cb);
  }
  else if (auto node_switch = node->as<CNodeSwitch>()) {
    for (auto node_case : node_switch->node_body->items) {
      visit_statements(node_case, visit_cb);
    }
  }
  else if (auto node_case = node->as<CNodeCase>()) {
    if (node_case->node_body) {
      for (auto node_statement : node_case->node_body) {
        visit_statements(node_statement, visit_cb);
      }
    }
  }
  else {
    visit_cb(node->req<CNodeStatement>());
  }
}

//------------------------------------------------------------------------------

bool has_non_terminal_return(CNodeCompound* node_compound) {

  bool hit_return = false;
  bool bad_return = false;

  statement_visitor visitor = [&](CNodeStatement* node) {
    auto node_return = node->as<CNodeReturn>();
    if (node_return) {
      if (hit_return) bad_return = true;
      hit_return = true;
    }
    else if (hit_return) {
      bad_return = true;
    }
  };

  visit_statements(node_compound, visitor);

  return bad_return;
}

//------------------------------------------------------------------------------

bool has_break(CNode* node) {
  bool result = false;

  visit(node, [&](CNode* node) {
    if (node->as<CNodeKeyword>() && node->get_text() == "break") result = true;
  });

  return result;
}

//------------------------------------------------------------------------------

bool check_ends_with_break(CNode* node) {

  if (node->as<CNodeKeyword>() && node->get_text() == "break") return true;

  if (auto node_compound = node->as<CNodeCompound>()) {
    for (auto i = 0; i < node_compound->statements.size() - 1; i++) {
      auto s = node_compound->statements[i];

      // Break statements in a nested switch statement don't count as breaks
      // for our current statement.
      if (s->as<CNodeSwitch>()) continue;

      if (has_break(s)) return false;
    }
    return check_ends_with_break(node_compound->statements.back());
  }

  if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_else == nullptr) {
      return false;
    }
    else {
      return check_ends_with_break(node_if->node_then) &&
            check_ends_with_break(node_if->node_else);
    }
  }

  if (auto node_expstatement = node->as<CNodeExpStatement>()) {
    return check_ends_with_break(node_expstatement->node_exp);
  }

  if (auto node_list = node->as<CNodeList>()) {
    for (auto i = 0; i < node_list->items.size() - 1; i++) {
      if (has_break(node_list->items[i])) return false;
    }
    return check_ends_with_break(node_list->items.back());
  }

  dump_parse_tree(node);
  assert(false);
  return false;
}

//------------------------------------------------------------------------------

bool check_switch_breaks(CNodeSwitch* node_switch) {

  auto& items = node_switch->node_body->items;

  for (int i = 0; i < items.size(); i++) {
    auto node_case = items[i]->as<CNodeCase>();

    if (node_case->node_body) {
      // If a case has a body, it must end with a break.
      if (!check_ends_with_break(node_case->node_body)) {
        return false;
      }
    }
    else {
      // Fallthrough is OK, but it the last case does not have a body, then
      // it can't have a break and the switch statement is invalid.
      if (i == items.size() - 1) return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
