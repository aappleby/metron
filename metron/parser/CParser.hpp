#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>

inline void replace(std::string& s, const std::string& a, const std::string& b) {
  auto i = s.find(a);
  if (i == std::string::npos) {
    assert(false);
    return;
  }
  s.replace(i, a.size(), b);
}

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  void init(const char* match_name, SpanType span, uint64_t flags) {
    CNode::init(match_name, span, flags);
  }

  virtual Err emit(Cursor& cursor) {
    Err err;
    err << cursor.emit_print("{{CNodeClass}}");
    cursor.text_cursor = text_end();
    return err;
  }

  std::string class_name() {
    return child("name")->as_string();
  }
};

//------------------------------------------------------------------------------

struct CNodeDeclaration : public CNode {};
struct CNodeEnum : public CNode {};
struct CNodeFunction : public CNode {};
struct CNodeNamespace : public CNode {};

//------------------------------------------------------------------------------

/*
CHECK_RETURN Err MtCursor::emit_sym_preproc_include(MnNode n) {
  Err err = check_at(sym_preproc_include, n);

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_path) {
      auto path = child.text();
      if (!path.ends_with(".h\"")) return err << ERR("Include did not end with .h\n");
      path.resize(path.size() - 3);
      path = path + ".sv\"";
      err << emit_replacement(child, path.c_str());
    }
    else {
      err << emit_dispatch(child);
    }
  }

  return err << check_done(n);
}

CHECK_RETURN Err MtCursor::emit_sym_preproc_arg(MnNode n) {
  Err err = check_at(n);

  // If we see any other #defined constants inside a #defined value,
  // prefix them with '`'
  std::string arg = n.text();
  std::string new_arg;

  for (int i = 0; i < arg.size(); i++) {
    for (const auto& def_pair : preproc_vars.top()) {
      if (def_pair.first == arg) continue;
      if (strncmp(&arg[i], def_pair.first.c_str(), def_pair.first.size()) ==
          0) {
        new_arg.push_back('`');
        break;
      }
    }
    new_arg.push_back(arg[i]);
  }

  err << emit_replacement(n, new_arg.c_str());

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_preproc_ifdef(MnNode n) {
  Err err = check_at(sym_preproc_ifdef, n);

  for (auto child : n) {
    err << emit_ws_to(child);
    err << emit_toplevel_node(child);
  }

  return err << check_done(n);
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtCursor::emit_sym_preproc_def(MnNode n) {
  Err err = check_at(sym_preproc_def, n);

  MnNode node_name;

  for (auto child : n) {
    err << emit_ws_to(child);

    if (child.field == field_name) {
      node_name = child;
    }
    else if (child.field == field_value) {
      preproc_vars.top()[node_name.text()] = child;
    }

    err << emit_dispatch(child);
  }

  return err << check_done(n);
}

*/

struct CNodePreproc : public CNode {
  virtual Err emit(Cursor& cursor) {
    Err err = cursor.check_at(this);
    //return cursor.emit_span(text_begin(), text_end());

    if (starts_with("#ifndef")) {
      err << cursor.emit_print("`ifndef");
      auto s = as_text_span().advance(strlen("#ifndef"));
      err << cursor.emit_span(s.begin, s.end);
    }
    else if (starts_with("#define")) {
      err << cursor.emit_print("`define");
      auto s = as_text_span().advance(strlen("#define"));
      err << cursor.emit_span(s.begin, s.end);
    }
    else if (starts_with("#include")) {
      auto s = as_string();
      replace(s, "#include", "`include");
      replace(s, ".h", ".sv");
      err << cursor.emit_replacement(this, s);
    }
    else if (starts_with("#endif")) {
      err << cursor.emit_print("`endif");
      auto s = as_text_span().advance(strlen("#endif"));
      err << cursor.emit_span(s.begin, s.end);
    }
    else {
      return ERR("Don't know how to handle this preproc");
    }

    return err << cursor.check_done(this);
  }
};

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {};

//------------------------------------------------------------------------------

/*
CHECK_RETURN Err MtCursor::emit_sym_template_declaration(MnNode n) {
  Err err = check_at(sym_template_declaration, n);

  auto node_template = n.child(0);
  auto node_params   = n.child(1);
  auto node_class    = n.child(2);
  auto node_semi     = n.child(3);

  std::string class_name = node_class.get_field(field_name).text();
  current_mod.push(lib->get_module(class_name));

  // Template params have to go _inside_ the class definition in Verilog, so
  // we skip them here.
  err << skip_over(node_template);
  err << skip_gap(node_template, node_params);
  err << skip_over(node_params);
  err << skip_gap(node_params, node_class);
  err << emit_dispatch(node_class);
  err << skip_gap(node_class, node_semi);
  err << skip_over(node_semi);

  current_mod.pop();
  return err << check_done(n);
}
*/

struct CNodeTemplate : public CNode {
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

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {};
struct CNodeUnion : public CNode {};

struct CNodeTranslationUnit : public CNode {
  virtual Err emit(Cursor& cursor) {
    return emit_default(cursor);
  }
};

//------------------------------------------------------------------------------
