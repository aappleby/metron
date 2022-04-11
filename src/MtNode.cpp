#include "MtNode.h"

#include "Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"
//#include "Platform.h"
#include "metron_tools.h"

const MnNode MnNode::null;

/*
bool operator<(const TSTreeCursor& a, const TSTreeCursor& b) {
  if (a.context[0] < b.context[0]) return true;
  if (a.context[0] > b.context[0]) return false;
  if (a.context[1] < b.context[1]) return true;
  if (a.context[1] > b.context[1]) return false;
  if (a.tree < b.tree) return true;
  if (a.tree > b.tree) return false;
  if (a.id < b.id) return true;
  if (a.id > b.id) return false;
  return false;
}
*/

bool operator==(const TSTreeCursor& a, const TSTreeCursor& b) {
  if (a.context[0] != b.context[0]) return false;
  if (a.context[1] != b.context[1]) return false;
  if (a.tree != b.tree) return false;
  if (a.id != b.id) return false;
  return true;
}

//bool MnConstIterator::operator<(const MnConstIterator& b) const { return cursor < b.cursor; }
bool MnConstIterator::operator!=(const MnConstIterator& b) const { return !(cursor == b.cursor); }

//bool MnIterator::operator<(const MnIterator& b) const { return cursor < b.cursor; }
bool MnIterator::operator!=(const MnIterator& b) const { return !(cursor == b.cursor); }

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

void MnNode::dump_source_lines() const {

  TSPoint p = ts_node_start_point(node);
  TinyLog::get().printf(0xFFFFFF, "==========\n");
  TinyLog::get().printf(0xFFFFFF, "%s(%d,%d):\n", source->full_path.c_str(), p.row + 1, p.column + 1);
  auto source_start = source->source;
  auto source_end = source->source_end;

  auto a = start();
  auto b = end();

  while (a > source_start && *a != '\n' && *a != '\r') a--;
  while (b < source_end   && *b != '\n' && *b != '\r') b++;
  if (*a == '\n' || *a == '\r') a++;
  if (*b == '\n' || *b == '\r') b--;

  TinyLog::get().print_buffer(0x008080FF, a, int(b-a));
  TinyLog::get().printf(0x008080FF, "\n");
  TinyLog::get().printf(0xFFFFFF, "==========\n");
}

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

  auto old_a = &source->source[start_byte()];
  auto old_b = &source->source[end_byte()];

  auto a = old_a;
  auto b = old_b;

  if (sym == sym_preproc_arg) {
    // TreeSitter bug - #defines include the whitespace before the value, trim
    // it.
    while (isspace(*a)) a++;
  } else {
    assert(!isspace(a[0]));
  }

  return a;
}

const char* MnNode::end() const {
  assert(!is_null());
  auto b = &source->source[end_byte()];
  while (isspace(b[-1])) b--;
  return b;
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

//------------------------------------------------------------------------------

std::string MnNode::name4() const {
  assert(!is_null());

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
    case sym_function_declarator:
      return get_field(field_declarator).name4();

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

    default:
      dump_tree();
      debugbreak();
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
      }
      else {
        return "<anon enum>";
      }
    }
    case sym_parameter_declaration:
      return get_field(field_type).type5();
    case sym_optional_parameter_declaration:
      return get_field(field_type).type5();
    default:
      dump_tree();
      debugbreak();
      return "";
  }
}

//------------------------------------------------------------------------------

void MnNode::visit_tree(NodeVisitor cv) {
  cv(*this);
  for (auto c : *this) {
    if (c.is_null()) {
      printf("NULL!\n");
      debugbreak();
    }
    c.visit_tree(cv);
  }
}

//------------------------------------------------------------------------------
// Node debugging


struct NodeDumper {

  //----------------------------------------

  void dump_tree(const MnNode& n, int index, int depth, int maxdepth) {
    printf("\n========== tree dump begin\n");
    dump_tree_recurse(n, index, depth, maxdepth, false);
    printf("========== tree dump end\n");
  }

  //----------------------------------------

  void dump_tree_recurse(const MnNode& n, int index, int depth, int maxdepth, bool is_last) {

    dump_node(n, index, depth, is_last);

    color_stack.push_back(node_to_color(n));

    if (!n.is_null() && depth < maxdepth) {
      for (int i = 0; i < n.child_count(); i++) {
        dump_tree_recurse(n.child(i), i, depth + 1, maxdepth, i == n.child_count() - 1);
      }
    }

    color_stack.pop_back();
  }

  //----------------------------------------

  void set_color(int color) {
    if (color <= 0) {
      printf("\u001b[0m");
    }
    else {
      printf("\u001b[38;2;%d;%d;%dm", (color >> 0) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
    }
  }

  void dump_node(const MnNode& n, int index, int depth, bool is_last) {
    printf(" ");
    for (int i = 0; i < color_stack.size(); i++) {
      bool stack_top = i == color_stack.size() - 1;
      int color = color_stack[i];

      set_color(color);

      if      (color == -1) printf("   ");
      else if (!stack_top)  printf("\261  ");
      else if (is_last)     printf("\261\315\315");
      else                  printf("\261\315\315");
    }
    printf("\u001b[0m");

    {
      set_color(node_to_color(n));
      if (n.child_count()) {
        printf("\333 ");
      }
      else {
        printf("\376 ");
      }
      if (n.field) printf("%s: ", ts_language_field_name_for_id(n.source->lang, n.field));
      printf("%s = ", n.is_named() ? n.ts_node_type() : "lit");
      if (!n.child_count()) print_escaped(n.source->source, n.start_byte(), n.end_byte());
      printf("\n");
      set_color(-1);
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
    //if (n.sym == alias_sym_type_identifier) color = 0xEEEEEE;
    //if (n.sym == sym_identifier) color = 0xEEEEEE;
    //if (n.sym == alias_sym_field_identifier) color = 0xEEEEEE;

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

  std::vector<int> color_stack;
};

//------------------------------------------------------------------------------

void MnNode::dump_tree(int index, int depth, int maxdepth) const {
  NodeDumper d;
  d.dump_tree(*this, index, depth, maxdepth);
}

//------------------------------------------------------------------------------
