#include "MtNode.h"

#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtNode.h"
#include "MtSourceFile.h"
//#include "Platform.h"
#include "metron_tools.h"

const MnNode MnNode::null;

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
  auto source_start = source->source;
  auto source_end = source->source_end;

  auto a = start();
  auto b = end();

  while (a > source_start && *a != '\n' && *a != '\r') a--;
  while (b < source_end   && *b != '\n' && *b != '\r') b++;
  if (*a == '\n' || *a == '\r') a++;
  if (*b == '\n' || *b == '\r') b--;

  TinyLog::get().print_buffer(0x008080FF, a, int(b-a));
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

  auto a = &source->source[start_byte()];
  auto b = &source->source[end_byte()];

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
      error();
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
    case sym_enum_specifier:
      return get_field(field_name).type5();
    case sym_parameter_declaration:
      return get_field(field_type).type5();
    case sym_optional_parameter_declaration:
      return get_field(field_type).type5();
    default:
      error();
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

void MnNode::dump_node(int index, int depth) const {
  if (is_null()) {
    printf("### NULL ###\n");
    return;
  }

  uint32_t color = 0x888888;
  if (sym == sym_template_declaration) color = 0xAADDFF;
  if (sym == sym_struct_specifier) color = 0xFF00FF;
  if (sym == sym_class_specifier) color = 0xFFAAFF;
  if (sym == sym_expression_statement) color = 0xAAFFFF;
  if (sym == sym_expression_statement) color = 0xAAFFFF;
  if (sym == sym_compound_statement) color = 0xFFFFFF;
  if (sym == sym_function_definition) color = 0xAAAAFF;
  if (sym == sym_field_declaration) color = 0xFFAAAA;
  if (sym == sym_comment) color = 0xAAFFAA;

  printf("[%02d:%03d:%03d] ", index, int16_t(field), int16_t(sym));

  for (int i = 0; i < depth; i++) printf(color != 0x888888 ? "|--" : "|  ");

  if (field) printf("%s: ", ts_language_field_name_for_id(source->lang, field));

  printf("%s = ", is_named() ? ts_node_type() : "lit");

  if (!child_count()) print_escaped(source->source, start_byte(), end_byte());

  printf("\n");
}

//------------------------------------------------------------------------------

void MnNode::dump_tree(int index, int depth, int maxdepth) const {
  if (depth == 0) {
    printf("\n========== tree dump begin\n");
  }
  dump_node(index, depth);

  if (!is_null() && depth < maxdepth) {
    for (int i = 0; i < child_count(); i++) {
      child(i).dump_tree(i, depth + 1, maxdepth);
    }
  }
  if (depth == 0) printf("========== tree dump end\n");
}

//------------------------------------------------------------------------------
