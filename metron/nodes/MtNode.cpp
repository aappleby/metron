#include "metron/nodes/MtNode.h"

#include "metron/app/MtSourceFile.h"

#include "metrolib/core/Log.h"


const MnNode MnNode::null;

bool operator==(const TSTreeCursor& a, const TSTreeCursor& b) {
  if (a.context[0] != b.context[0]) return false;
  if (a.context[1] != b.context[1]) return false;
  if (a.tree != b.tree) return false;
  if (a.id != b.id) return false;
  return true;
}

bool MnConstIterator::operator!=(const MnConstIterator& b) const {
  return !(cursor == b.cursor);
}

//------------------------------------------------------------------------------

MnNode::MnNode() {
  this->node = {0};
  this->sym = 0;
  this->field = 0;
  this->source = nullptr;
}

MnNode::MnNode(TSNode node, int sym, int field, MtSourceFile* source) {
  this->node = node;
  this->sym = sym;
  this->field = field;
  this->source = source;
}

//------------------------------------------------------------------------------

SourceRange MnNode::get_source() const {
  auto source_start = source->source;
  auto source_end = source->source_end;

  auto sp = ts_node_start_point(node);
  auto ep = ts_node_end_point(node);

  auto a = &source->source[ts_node_start_byte(node)];
  auto b = &source->source[ts_node_end_byte(node)];

  while (a > source_start && *a != '\n' && *a != '\r') a--;
  while (b < source_end && *b != '\n' && *b != '\r') b++;
  if (*a == '\n' || *a == '\r') a++;
  if (*b == '\n' || *b == '\r') b--;

  return {source->filename.c_str(), source->full_path.c_str(), (int)sp.row, (int)sp.column, a, b};
}

//------------------------------------------------------------------------------

// KCOV_OFF
void MnNode::dump_source_lines() const {
  TSPoint p = ts_node_start_point(node);

  LOG_C(0xFFFFFF, "==========\n");
  LOG_C(0xFFFFFF, "%s(%d,%d):\n", source->full_path.c_str(), p.row + 1,
        p.column + 1);
  auto source_start = source->source;
  auto source_end = source->source_end;

  auto a = start();
  auto b = end();

  while (a > source_start && *a != '\n' && *a != '\r') a--;
  while (b < source_end && *b != '\n' && *b != '\r') b++;
  if (*a == '\n' || *a == '\r') a++;
  if (*b == '\n' || *b == '\r') b--;

  TinyLog::get().print_buffer(stdout, 0x008080FF, a, int(b - a + 1));

  LOG("\n");

  LOG_C(0xFFFFFF, "==========\n");
}
// KCOV_ON

//------------------------------------------------------------------------------

MnNode MnNode::get_field(int field_id) const {
  if (is_null()) return MnNode::null;

  for (auto c : *this) {
    if (c.field == field_id) return c;
  }

  return MnNode::null;
}

//------------------------------------------------------------------------------

MnNode MnNode::child(int index) const {
  int i = 0;
  for (auto c : *this) {
    if (index == i) return c;
    i++;
  }
  return MnNode::null;
}

//----------

MnNode MnNode::named_child(int index) const {
  int i = 0;
  for (auto c : *this) {
    if (!c.is_named()) continue;
    if (index == i) return c;
    i++;
  }
  return MnNode::null;
}

//----------

MnNode MnNode::first_named_child() const { return named_child(0); }

MnNode MnNode::child_by_sym(TSSymbol _sym) const {
  for (auto c : *this) {
    if (c.sym == _sym) return c;
  }
  return MnNode::null;
}

bool MnNode::is_static() const {
  for (const auto& c : *this) {
    if (c.sym == sym_storage_class_specifier && c.text() == "static")
      return true;
  }
  return false;
}

bool MnNode::is_const() const {
  for (const auto& c : *this) {
    if (c.sym == sym_type_qualifier && c.text() == "const") return true;
  }
  return false;
}

//------------------------------------------------------------------------------

const char* MnNode::start() const {
  assert(!is_null());
  return &source->source[start_byte()];
}

const char* MnNode::end() const {
  assert(!is_null());
  return &source->source[end_byte()];
}

std::string MnNode::text() const { return std::string(start(), end()); }

bool MnNode::match(const char* s) {
  if (is_null()) return false;
  const char* a = start();
  const char* b = end();
  while (a != b) {
    if (*a++ != *s++) return false;
  }
  return true;
}

bool MnNode::contains(const char* s) const {
  if (is_null()) return false;
  const char* a = start();
  const char* b = end();
  const char* c = s;

  while(a != b && *c) {
    if (*a == *c) {
      a++;
      c++;
    }
    else {
      a++;
      c = s;
    }
  }

  return *c == 0;
}

//------------------------------------------------------------------------------

std::string MnNode::name4() const {
  if (is_null()) return "<null>";
  //assert(!is_null());

  switch (sym) {
    case sym_subscript_expression:
      return get_field(field_argument).name4();

    case sym_qualified_identifier:
      return child(child_count() - 1).name4();

    case sym_field_expression:
      return text();

    case sym_call_expression:
      return get_field(field_function).name4();

    case alias_sym_type_identifier:
    case sym_identifier:
    case alias_sym_field_identifier:
      return text();

    case sym_field_declaration: {
      auto node_type = get_field(field_type);
      if (node_type.sym == sym_enum_specifier) {
        return node_type.name4();
      } else {
        return get_field(field_declarator).name4();
      }
    }

    case sym_array_declarator:
    case sym_parameter_declaration:
    case sym_optional_parameter_declaration:
    case sym_function_definition:
    case sym_function_declarator: {
      auto declarator = get_field(field_declarator);
      if (declarator.is_null()) {
        error();
      }
      return declarator.name4();
    }

    case sym_enum_specifier: {
      auto name = get_field(field_name);
      if (name.is_null()) {
        return "<anonymous enum>";
      } else {
        return get_field(field_name).name4();
      }
    }

    case sym_struct_specifier:
    case sym_template_function:
      return get_field(field_name).name4();

    case sym_primitive_type:
      return text();

    case sym_pointer_declarator:
      return get_field(field_declarator).name4();

    case sym_declaration:
      return get_field(field_declarator).name4();

    case sym_init_declarator:
      return get_field(field_declarator).name4();

    default:
      Err err;
      err << ERR("Unknown node type %s for name4()\n", ts_node_type());
      return "";
  }
}

std::string MnNode::type5() const {
  switch (sym) {
    case alias_sym_type_identifier:
      return text();
    case sym_primitive_type:
      return text();
    case sym_field_declaration:
      return get_field(field_type).type5();
    case sym_template_type:
      return get_field(field_name).type5();
    case sym_enum_specifier: {
      auto name = get_field(field_name);
      if (name) {
        return name.type5();
      } else {
        return "<anon enum>";
      }
    }
    case sym_parameter_declaration:
      return get_field(field_type).type5();
    case sym_optional_parameter_declaration:
      return get_field(field_type).type5();
    default:
      Err err;
      err << ERR("Unknown node type %s for type5()\n", ts_node_type());
      return "";
  }
}

//------------------------------------------------------------------------------

void MnNode::visit_tree(NodeVisitor cv) {
  cv(*this);
  for (auto c : *this) {
    if (c.is_null()) {
      Err err;
      err << ERR("Null node hit in visit_tree() for %s\n", text().c_str());
    }
    c.visit_tree(cv);
  }
}

//------------------------------------------------------------------------------
// Node debugging

std::string escape(const char* source, int a, int b) {
  std::string result;
  result.push_back('"');
  for (; a < b; a++) {
    switch (source[a]) {
      case '\n':
        result.append("\\n");
        break;
      case '\r':
        result.append("\\r");
        break;
      case '\t':
        result.append("\\t");
        break;
      case '"':
        result.append("\\\"");
        break;
      case '\\':
        result.append("\\\\");
        break;
      default:
        result.push_back(source[a]);
    }
  }
  result.push_back('"');
  return result;
}

struct NodeDumper {
  //----------------------------------------

  void dump_tree(const MnNode& n, int index, int depth, int maxdepth) {
    LOG("\n========== tree dump begin\n");
    dump_tree_recurse(n, index, depth, maxdepth, false);
    LOG("========== tree dump end\n");
  }

  //----------------------------------------

  void dump_tree_recurse(const MnNode& n, int index, int depth, int maxdepth,
                         bool is_last) {
    dump_node(n, index, depth, is_last);

    color_stack.push_back(node_to_color(n));

    if (!n.is_null() && depth < maxdepth) {
      for (int i = 0; i < n.child_count(); i++) {
        dump_tree_recurse(n.child(i), i, depth + 1, maxdepth,
                          i == n.child_count() - 1);
      }
    }

    color_stack.pop_back();
  }

  //----------------------------------------

  void dump_node(const MnNode& n, int index, int depth, bool is_last) {
    if (n.is_null()) {
      LOG_R("<NULL>??? %d %d %d\n", index, depth, is_last);
      return;
    }

    LOG(" ");
    for (int i = 0; i < color_stack.size(); i++) {
      bool stack_top = i == color_stack.size() - 1;
      uint32_t color = color_stack[i];

      if (color == -1)
        LOG_C(0x000000, "   ");
      else if (!stack_top)
        //LOG_C(color, "\261  ");
        LOG_C(color, "|  ");
      else if (is_last)
        //LOG_C(color, "\261\315\315");
        LOG_C(color, "|--");
      else
        //LOG_C(color, "\261\315\315");
        LOG_C(color, "|--");
    }

    {
      auto color = node_to_color(n);

      if (n.child_count()) {
        //LOG_C(color, "\333 ");
        LOG_C(color, "+ ");
      } else {
        //LOG_C(color, "\376 ");
        LOG_C(color, "# ");
      }

      if (n.field) {
        LOG_C(color,
              "%s: ", ts_language_field_name_for_id(n.source->lang, n.field));
      }

      LOG_C(color, "%s (%d) = ", n.is_named() ? n.ts_node_type() : "lit", n.sym);

      if (!n.child_count()) {
        std::string escaped =
            escape(n.source->source, n.start_byte(), n.end_byte());
        LOG_C(color, "%s", escaped.c_str());
      }

      LOG_C(color, "\n");
    }

    if (is_last) {
      color_stack.pop_back();
      color_stack.push_back(-1);
    }
  }

  //----------------------------------------

  int node_to_color(const MnNode& n) {
    int color = 0x999999;

    // Preproc stuff = brown
    if (n.sym == sym_preproc_include) color = 0x00BBBB;

    // Top-level translation unit = white
    if (n.sym == sym_translation_unit) color = 0xFFFFFF;

    // Template decls = yellow
    if (n.sym == sym_template_declaration) color = 0x00FFFF;
    if (n.sym == sym_template_parameter_list) color = 0x00FFFF;

    // Struct/class + field decl list = green
    if (n.sym == sym_struct_specifier) color = 0x00FF00;
    if (n.sym == sym_class_specifier) color = 0x00FF00;
    if (n.sym == sym_field_declaration_list) color = 0x00FF00;

    // Template types = cyan
    if (n.sym == sym_template_type) color = 0xFFFF00;
    if (n.sym == sym_template_argument_list) color = 0xFFFF00;

    // Functions and function params = red
    if (n.sym == sym_function_definition) color = 0x0000FF;
    if (n.sym == sym_function_declarator) color = 0x0000FF;
    if (n.sym == sym_parameter_list) color = 0x0000FF;

    // Compound statements = blue
    if (n.sym == sym_compound_statement) color = 0xFF8888;

    // Identifiers lighter grey
    // if (n.sym == alias_sym_type_identifier) color = 0xEEEEEE;
    // if (n.sym == sym_identifier) color = 0xEEEEEE;
    // if (n.sym == alias_sym_field_identifier) color = 0xEEEEEE;

    // Calls are transitions between modules, magenta
    if (n.sym == sym_call_expression) color = 0xFF00FF;
    if (n.sym == sym_argument_list) color = 0xFF00FF;

    // Ifs, assigns and returns are important, bright yellow
    if (n.sym == sym_assignment_expression) color = 0x00EEEE;
    if (n.sym == sym_return_statement) color = 0x00EEEE;
    if (n.sym == sym_if_statement) color = 0x00EEEE;

    // Comments dark green like the code theme :)
    if (n.sym == sym_comment) color = 0x00A000;

    // Dim literals a bit.
    if (!n.is_named()) color = 0x666666;

    return color;
  }

  //----------------------------------------

  std::string indent;

  std::vector<uint32_t> color_stack;
};

//------------------------------------------------------------------------------

void MnNode::dump_tree(int index, int depth, int maxdepth) const {
  NodeDumper d;
  d.dump_tree(*this, index, depth, maxdepth);
}

//------------------------------------------------------------------------------
