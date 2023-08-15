#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  virtual uint32_t debug_color() const { return 0x00FF00; }

  void init(const char* match_name, SpanType span, uint64_t flags) {
    CNode::init(match_name, span, flags);
  }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);

    auto n_class = child("class");
    auto n_name  = child("name");
    auto n_body  = child("body");

    err << cursor.emit_replacements(n_class->as_string_view(), "class", "module");
    err << cursor.emit_gap(n_class, n_name);
    err << n_name->emit(cursor);
    err << cursor.emit_gap(n_name, n_body);

    err << cursor.emit_print("(\n");
    //cursor.push_indent(node_body);
    err << cursor.emit_print("{{port list}}");
    //err << cursor.emit_module_ports(node_body);
    //cursor.pop_indent(node_body);
    err << cursor.emit_line(");");


    err << n_body->emit(cursor);

    return err << cursor.check_done(this);
  }

  std::string class_name() {
    return child("name")->as_string();
  }
};

//------------------------------------------------------------------------------

struct CNodeDeclaration : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);
    err << cursor.emit_print("{{CNodeDeclaration}}");
    cursor.text_cursor = text_end();
    return err << cursor.check_done(this);
  }
};

struct CNodeEnum : public CNode {};

struct CNodeFunction : public CNode {
  virtual uint32_t debug_color() const { return 0x0000FF; }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);
    err << cursor.emit_print("{{CNodeFunction}}");
    cursor.text_cursor = text_end();
    return err << cursor.check_done(this);
  }
};

struct CNodeNamespace : public CNode {};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }

  virtual Err emit(Cursor& cursor) {
    return emit_default(cursor);
  }
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const { return 0x00FFFF; }
  virtual Err emit(Cursor& cursor) { return emit_default(cursor); }
};

//------------------------------------------------------------------------------

struct CNodeAccess : public CNode {
  virtual uint32_t debug_color() const { return 0xFF88FF; }
  virtual Err emit(Cursor& cursor) {
    return cursor.comment_out(this);
  }
};

//------------------------------------------------------------------------------

struct CNodeFieldList : public CNode {
  virtual uint32_t debug_color() const { return 0xFF8080; }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);

    for (auto c : (CNode*)this) {
      if (c->as_string_view() == "{") {
        //err << cursor.emit_replacements(d->as_string_view(), "{", "", "}", "endmodule");
        err << cursor.skip_over(c);
        err << c->emit_gap_after(cursor);
        err << cursor.emit_print("{{template parameter list}}\n");
      }
      else if (c->as_string_view() == "}") {
        //err << cursor.emit_replacements(d->as_string_view(), "{", "", "}", "endmodule");
        err << cursor.emit_print("endmodule");
        cursor.text_cursor = c->text_end();
        err << c->emit_gap_after(cursor);
      }
      else {
        err << c->emit(cursor);
        err << c->emit_gap_after(cursor);
      }

    }

    return err << cursor.check_done(this);

    //return emit_default(cursor);
  }
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  virtual uint32_t debug_color() const { return 0x00BBBB; }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);
    auto v = as_string_view();

    if (v.starts_with("#include")) {
      err << cursor.emit_replacements(v, "#include", "`include", ".h", ".sv");
    }
    else if (v.starts_with("#ifndef")) {
      err << cursor.emit_replacements(v, "#ifndef", "`ifndef");
    }
    else if (v.starts_with("#define")) {
      err << cursor.emit_replacements(v, "#define", "`define");
    }
    else if (v.starts_with("#endif")) {
      err << cursor.emit_replacements(v, "#endif", "`endif");
    }
    else {
      return ERR("Don't know how to handle this preproc");
    }

    return err << cursor.check_done(this);
  }
};

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {};

struct CNodeCall : public CNode {
  virtual uint32_t debug_color() const { return 0xFF0040; }
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF00; }
  virtual Err emit(Cursor& cursor) { return emit_default(cursor); }
};

//------------------------------------------------------------------------------

struct CNodeTemplate : public CNode {
  virtual uint32_t debug_color() const { return 0x00FFFF; }

  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);


    // Template params have to go _inside_ the class definition in Verilog, so
    // we skip them here.
    auto class_node = child("template_class");
    err << cursor.skip_span(cursor.text_cursor, class_node->text_begin());
    err << class_node->emit(cursor);

    err << cursor.skip_span(cursor.text_cursor, text_end());
    //err << emit_rest(cursor);

    //cursor.current_mod.pop();
    return err << cursor.check_done(this);
  }
};

struct CNodeTemplateParams : public CNode {
  virtual uint32_t debug_color() const { return 0x0088FF; }
};

struct CNodeTemplateParam : public CNode {
  virtual uint32_t debug_color() const { return 0x0044FF; }
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF88; }
  virtual Err emit(Cursor& cursor) { return emit_default(cursor); }
};

struct CNodeType : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFFFF; }
};

struct CNodeTypedef : public CNode {};

struct CNodeUnion : public CNode {};

struct CNodeTranslationUnit : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF00; }
  virtual Err emit(Cursor& cursor) { return emit_default(cursor); }
};

struct CNodeCompoundStatement : public CNode {
  virtual uint32_t debug_color() const { return 0xFF8888; }

  /*
  virtual Err emit(Cursor& cursor) {
    return emit_default(cursor);
  }
  */
};

//------------------------------------------------------------------------------
