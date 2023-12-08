#include "metron/nodes/CNodeCompound.hpp"
#include "metron/nodes/CNodeKeyword.hpp"
#include "metron/nodes/CNodeReturn.hpp"
#include "metron/nodes/CNodeIf.hpp"
#include "metron/nodes/CNodeFor.hpp"
#include "metron/nodes/CNodeDoWhile.hpp"
#include "metron/nodes/CNodeWhile.hpp"
#include "metron/nodes/CNodeSwitch.hpp"

  /*
  if (has_non_terminal_return(node->node_body)) {
    return ERR("Function has mid-block return");
  }
  */

  /*
  if (node->node_body && !ends_with_break(node->node_body)) {
    return ERR(
        "All non-empty case statements must end with break (no fallthroughs)");
  }
  */

  /*
  if (node->node_body && !ends_with_break(node->node_body)) {
    return ERR(
        "All non-empty case statements must end with break (no fallthroughs)");
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
  else if (auto node_default = node->as<CNodeDefault>()) {
    if (node_default->node_body) {
      for (auto node_statement : node_default->node_body) {
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

bool ends_with_break(CNode* node) {
  if (!node) return false;
  if (node->as<CNodeKeyword>() && node->get_text() == "break") return true;

  if (auto node_compound = node->as<CNodeCompound>()) {
    for (auto i = 0; i < node_compound->statements.size() - 1; i++) {
      if (ends_with_break(node_compound->statements[i])) return false;
    }
    return ends_with_break(node_compound->statements.back());
  }

  if (auto node_if = node->as<CNodeIf>()) {
    if (node_if->node_else == nullptr) return false;
    return ends_with_break(node_if->node_then) &&
           ends_with_break(node_if->node_else);
  }

  if (auto node_list = node->as<CNodeList>()) {
    for (auto i = 0; i < node_list->items.size() - 1; i++) {
      if (ends_with_break(node_list->items[i])) return false;
    }
    return ends_with_break(node_list->items.back());
  }

  if (auto node_expstatement = node->as<CNodeExpStatement>()) {
    return ends_with_break(node_expstatement->node_exp);
  }

  return false;
}
