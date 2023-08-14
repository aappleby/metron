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

CHECK_RETURN Err CNode::emit_default(Cursor& cursor) {
  Err err = cursor.check_at(this);

  if (!child_head) {
    err << cursor.emit_span(text_begin(), text_end());
    return err;
  }

  err << cursor.emit_span(text_begin(), child_head->text_begin());

  for (auto c = child_head; c; c = c->node_next) {
    err << c->emit(cursor);
    auto n = c->node_next;
    if (n) {
      err << cursor.emit_gap(c, n);
    }
  }

  err << cursor.emit_span(child_tail->text_end(), text_end());

  return err << cursor.check_done(this);
}


CHECK_RETURN Err CNode::emit_children(Cursor& cursor) {
  Err err = cursor.check_at(this);
  for (auto c = child_head; c; c = c->node_next) {
    err << c->emit(cursor);
    auto n = c->node_next;
    if (n) {
      err << cursor.emit_gap(c, n);
    }
  }
  return err << cursor.check_done(this);
}

CHECK_RETURN Err CNode::emit_rest(Cursor& cursor) {
  return cursor.emit_span(cursor.text_cursor, text_end());
}
