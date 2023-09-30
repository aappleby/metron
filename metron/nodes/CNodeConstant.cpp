#include "CNodeConstant.hpp"

//==============================================================================

Err CNodeConstant::trace(CInstance* inst, call_stack& stack) {
  return Err();
}

//----------------------------------------


Err CNodeConstant::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  std::string body = get_textstr();

  // FIXME what was this for?
  // This was for forcing enum constants to the size of the enum type
  int size_cast = cursor.override_size.top();
  //int size_cast = 0;

  // Count how many 's are in the number
  int spacer_count = 0;
  int prefix_count = 0;

  for (auto& c : body)
    if (c == '\'') {
      c = '_';
      spacer_count++;
    }

  if (body.starts_with("0x")) {
    prefix_count = 2;
    if (!size_cast) size_cast = ((int)body.size() - 2 - spacer_count) * 4;
    err << cursor.emit_print("%d'h", size_cast);
  } else if (body.starts_with("0b")) {
    prefix_count = 2;
    if (!size_cast) size_cast = (int)body.size() - 2 - spacer_count;
    err << cursor.emit_print("%d'b", size_cast);
  } else {
    if (size_cast) {
      err << cursor.emit_print("%d'd", size_cast);
    }
  }

  err << cursor.emit_print("%s", body.c_str() + prefix_count);

  cursor.gap_emitted = false;
  cursor.tok_cursor = tok_end();

  return err << cursor.check_done(this);
}

//==============================================================================
