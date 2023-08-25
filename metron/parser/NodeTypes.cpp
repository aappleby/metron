#include "NodeTypes.hpp"

#include "CNodeClass.hpp"
#include "CNodeType.hpp"
#include "CInstance.hpp"

using namespace matcheroni;
using namespace parseroni;

//------------------------------------------------------------------------------

Err CNodePreproc::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);
  auto v = get_text();

  if (v.starts_with("#include")) {
    err << cursor.emit_replacements(this, "#include", "`include", ".h", ".sv");
  }
  else if (v.starts_with("#ifndef")) {
    err << cursor.emit_replacements(this, "#ifndef", "`ifndef");
  }
  else if (v.starts_with("#define")) {
    err << cursor.emit_replacements(this, "#define", "`define");
  }
  else if (v.starts_with("#endif")) {
    err << cursor.emit_replacements(this, "#endif", "`endif");
  }
  else {
    return ERR("Don't know how to handle this preproc");
  }

  return err << cursor.check_done(this);
}

//------------------------------------------------------------------------------

Err CNodeIdentifier::trace(CInstance* instance, TraceAction action) {
  Err err;

  CInstance* field = instance->resolve(get_string());

  return err;
}

//------------------------------------------------------------------------------
