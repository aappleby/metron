// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CNode.hpp"

#include "Cursor.hpp"

#include "metrolib/core/Log.h"

using namespace matcheroni;

CHECK_RETURN Err CNode::emit(Cursor& c) {
  LOG_R("CNode::emit() %s\n", match_name);
  return ERR("Don't know how to emit a %s", typeid(*this).name());
}

CHECK_RETURN Err CNode::emit_children(Cursor& cursor) {
  Err err;
  for (auto c = child_head; c; c = c->node_next) {
    err << c->emit(cursor);
    auto n = c->node_next;
    if (n) {
      err << cursor.emit_gap(c, n);
    }
  }
  return err;
}

CHECK_RETURN Err CNode::emit_rest(Cursor& cursor) {
  return cursor.emit_span(cursor.text_cursor, text_end());
}
