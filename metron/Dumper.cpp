#include "Dumper.hpp"

#include <string>
#include <vector>
#include <map>

#include "metrolib/core/Log.h"
#include "metron/CInstance.hpp"
#include "metron/CNode.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/CSourceRepo.hpp"

void dump_decl(CNodeDeclaration* decl);

//------------------------------------------------------------------------------

void dump_dispatch(CNode* node) {
  LOG_R("???");
}

//------------------------------------------------------------------------------
// Node debugging

static std::string escape(const char* a, const char* b) {
  std::string result;
  result.push_back('"');
  for (; a < b; a++) {
    switch (*a) {
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
        result.push_back(*a);
        break;
    }
  }
  result.push_back('"');
  return result;
}

//------------------------------------------------------------------------------

template <typename T>
const char* class_name(const T& t) {
  const char* name = typeid(t).name();
  while (*name && isdigit(*name)) name++;
  return name;
}

//------------------------------------------------------------------------------

/*
cnode uint32_t color = 0x222244;
cnodeaccess     color = COL_VIOLET;
cnodecall SKY
declaration 0xFF00FF
field pink
fieldexp     color = 0x80FF80;
function     color = COL_ORANGE;
keyword     color = 0xFFFF88;
list     color = 0xCCCCCC;
namespace     color = 0x00FFFFFF;
preproc     color = 0x00BBBB;
qual     color = 0x80FF80;
statement     color = COL_TEAL;
struct     color = 0xFFAAAA;
template     color = 0x00FFFF;
translation unit     color = 0xFFFF00;
type     color = COL_VIOLET;
typedef     color = 0xFFFF88;
*/

std::map<std::string, uint32_t> color_map1 = {
  {"include", 0x00BBBB},
  {"define",  0x00BBBB},

  {"class",  0x00FF00},
  {"params", 0xCCCCCC},
  {"body",   0xCCCCCC},

  {"template", 0x00FFFF},
};

std::map<std::string, uint32_t> color_map2 = {
  {"CNodePreproc",       0x00BBBB},
  {"CNodeExpStatement",  COL_TEAL},
  {"CNodeExpression",    COL_AQUA},
  {"CNodeOperator",      COL_SKY},
  {"CNodeUsing",         0x00DFFF},
  {"CNodePunct",         0x88CC88},
  {"CNodeIdentifier",    0x80FF80},
  {"CNodeClass",         0x00FF00},
};

struct NodeDumper {
  //----------------------------------------

  void dump_parse_tree_recurse(const CNode& n, int depth, int max_depth) {
    if (n.match_tag && color_map1.contains(n.match_tag)) {
      color_stack.push_back(color_map1[n.match_tag]);
    }
    else if (color_map2.contains(class_name(n))) {
      color_stack.push_back(color_map2[class_name(n)]);
    }
    else {
      color_stack.push_back(0xA0A0A0);
    }
    dump_parse_node(n, depth);
    if (!max_depth || depth < max_depth) {
      for (auto child = n.child_head; child; child = child->node_next) {
        dump_parse_tree_recurse(*child, depth + 1, max_depth);
      }
    }
    color_stack.pop_back();
  }

  //----------------------------------------

  void dump_parse_node(const CNode& n, int depth) {
    LOG(" ");

    /*
    const char* trellis_1 = "   ";
    const char* trellis_2 = "|  ";
    const char* trellis_3 = "|--";
    const char* trellis_4 = "\\--";
    const char* dot_1     = "O ";
    const char* dot_2     = "O ";
    */

    const char* trellis_1 = "    ";
    const char* trellis_2 = "┃   ";
    const char* trellis_3 = "┣━━╸";
    const char* trellis_4 = "┗━━╸";
    const char* dot_1 = "▆ ";
    const char* dot_2 = "▆ ";

    for (int i = 0; i < color_stack.size() - 1; i++) {
      bool stack_top = i == color_stack.size() - 2;
      uint32_t color = color_stack[i];
      if (color == -1)
        LOG_C(0x000000, trellis_1);
      else if (!stack_top)
        LOG_C(color, trellis_2);
      else if (n.node_next)
        LOG_C(color, trellis_3);
      else
        LOG_C(color, trellis_4);
    }

    auto color = color_stack.back();

    if (n.child_head != nullptr) {
      LOG_C(color, dot_1);
    } else {
      LOG_C(color, dot_2);
    }

    if (n.match_tag) {
      LOG_C(color, "%s : ", n.match_tag);
    }
    LOG_C(color, "%s = ", class_name(n));

    if (n.child_head == nullptr) {
      std::string escaped = escape(n.text_begin(), n.text_end());
      LOG_C(color, "%s", escaped.c_str());
    }

    LOG_C(color, "\n");

    if (n.node_next == nullptr && color_stack.size() > 1) {
      color_stack[color_stack.size() - 2] = -1;
    }
  }

  //----------------------------------------

  std::string indent;
  std::vector<uint32_t> color_stack;
};

void dump_parse_tree(CNode* node) {
  LOG("\n");
  NodeDumper d;
  d.dump_parse_tree_recurse(*node, 0, 1000);
}

//------------------------------------------------------------------------------

void dump_inst_tree(CInstance* inst) {
  if (auto inst_class = inst->as<CInstClass>()) {
    LOG_G("Class %s\n", inst_class->path.c_str());

    {
      LOG_INDENT_SCOPE();
      LOG_G("Ports:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_class->ports) dump_inst_tree(child);
    }
    {
      LOG_INDENT_SCOPE();
      LOG_R("Parts:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_class->parts) dump_inst_tree(child);
    }
  }

  if (auto inst_struct = inst->as<CInstStruct>()) {
    LOG_G("Struct %s\n", inst_struct->path.c_str());

    {
      LOG_INDENT_SCOPE();
      LOG_G("Parts:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_struct->parts) dump_inst_tree(child);
    }
  }

  if (auto inst_prim = inst->as<CInstPrim>()) {
    if (inst_prim->node_field && inst_prim->node_field->is_public) {
      LOG_G("Prim %s", inst_prim->path.c_str());
    } else {
      LOG_R("Prim %s", inst_prim->path.c_str());
    }

    for (auto state : inst_prim->state_stack) LOG(" %s", to_string(state));
    LOG("\n");
  }

  if (auto inst_func = inst->as<CInstFunc>()) {
    LOG_G("Func %s\n", inst_func->path.c_str());

    LOG_INDENT();

    if (inst_func->params.size()) {
      LOG_G("Params:\n");
      LOG_INDENT();
      for (auto p : inst_func->params) dump_inst_tree(p);
      LOG_DEDENT();
    }

    if (inst_func->inst_return) {
      LOG_G("Return:\n");
      LOG_INDENT();
      dump_inst_tree(inst_func->inst_return);
      LOG_DEDENT();
    }

    LOG_DEDENT();
  }
}

#if 0
//------------------------------------------------------------------------------

void CNode::dump() const {
  LOG_R("CNode::dump() : %s\n", typeid(*this).name());
}

void CNodePunct::dump() const override {
  auto text = get_text();
  LOG_B("CNodePunct \"%.*s\"\n", text.size(), text.data());
}

void CNodeNamespace::dump() const override {
  LOG_G("Fields:\n");
  LOG_INDENT_SCOPE();
  for (auto n : all_fields) n->dump();
}


void CNodeEnum::dump() const override {
  LOG_G("Enum %.*s\n", name.size(), name.data());
}


void CNodeDeclaration::dump() const override {
  auto text = get_text();
  LOG_G("Declaration `%.*s`\n", text.size(), text.data());
}

void CNodeAccess::dump() const override {
  auto text = get_text();
  LOG_B("CNodeAccess \"%.*s\"\n", text.size(), text.data());
}

//------------------------------------------------------------------------------

void CNode::dump_tree(int max_depth) const {
  NodeDumper d;
  d.dump_parse_tree_recurse(*this, 0, max_depth);
}

//----------------------------------------------------------------------------

void CScope::dump() {

  LOG_G("class_types\n");
  for (auto s : class_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("struct_types\n");
  for (auto s : struct_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("union_types\n");
  for (auto s : union_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("enum_types\n");
  for (auto s : enum_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("typedef_types\n");
  for (auto s : typedef_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }
}

#endif

//----------------------------------------------------------------------------

void dump_lexeme(Lexeme& l) {
  const int span_len = 20;
  std::string dump = "";

  if (l.type == LEX_BOF) dump = "<bof>";
  if (l.type == LEX_EOF) dump = "<eof>";

  for (auto c = l.text_begin; c < l.text_end; c++) {
    if      (*c == '\n') dump += "\\n";
    else if (*c == '\t') dump += "\\t";
    else if (*c == '\r') dump += "\\r";
    else                 dump += *c;
    if (dump.size() >= span_len) break;
  }

  dump = '`' + dump + '`';
  if (dump.size() > span_len) {
    dump.resize(span_len - 4);
    dump = dump + "...`";
  }
  while (dump.size() < span_len) dump += " ";

  LOG("r%04d c%02d i%02d ", l.row, l.col, l.indent);
  LOG_C(l.type_to_color(), "%-14.14s ", l.type_to_str());
  LOG("%s", dump.c_str());
}


//------------------------------------------------------------------------------

#if 0
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void CNodeField::dump() const {
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (node_decl->node_static) LOG_A("static ");
  if (node_decl->node_const)  LOG_A("const ");
  if (is_public)              LOG_A("public ");
  if (node_decl->is_array())  LOG_A("array ");

  if (parent_class) {
    LOG_A("parent class %s ", parent_class->name.c_str());
  }

  if (parent_struct) {
    LOG_A("parent struct %s ", parent_struct->name.c_str());
  }

  if (node_decl->_type_class) {
    LOG_A("type class %s ", node_decl->_type_class->name.c_str());
  }

  if (node_decl->_type_struct) {
    LOG_A("type struct %s ", node_decl->_type_struct->name.c_str());
  }

  LOG_A("\n");
}

//------------------------------------------------------------------------------

// FIXME constructor needs to be in internal_callers

void CNodeFunction::dump() const {

}
#endif

//------------------------------------------------------------------------------

void dump_function(CNodeFunction* node) {
  LOG_S("Method \"%s\" ", node->name.c_str());

  if (node->is_public)  LOG_G("public ");
  if (!node->is_public) LOG_G("private ");
  LOG_G("%s ", to_string(node->method_type));
  LOG_G("\n");

  if (node->params.size()) {
    LOG_INDENT_SCOPE();
    for (auto param : node->params) {
      LOG_G("Param: ");
      dump_decl(param);
      LOG_G("\n");
    }
  }

  if (node->self_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : node->self_reads) {
      LOG_G("Directly reads  %s : %s\n", r->path.c_str(), to_string(r->get_state()));
    }
  }

  if (node->self_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : node->self_writes) {
      LOG_G("Directly writes %s : %s\n", w->path.c_str(), to_string(w->get_state()));
    }
  }

  if (node->all_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : node->all_reads) {
      if (node->self_reads.contains(r)) continue;
      LOG_G("Indirectly reads  %s : %s\n", r->path.c_str(), to_string(r->get_state()));
    }
  }

  if (node->all_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : node->all_writes) {
      if (node->self_writes.contains(w)) continue;
      LOG_G("Indirectly writes %s : %s\n", w->path.c_str(), to_string(w->get_state()));
    }
  }

  if (node->internal_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : node->internal_callers) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (node->external_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : node->external_callers) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (node->internal_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : node->internal_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (node->external_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : node->external_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }
}

//------------------------------------------------------------------------------

void dump_class(CNodeClass* node) {
  LOG_B("Class %s, refcount %d\n", node->name.c_str(), node->refcount);

  if (node->all_modparams.size()) {
    LOG_G("Modparams\n");
    LOG_INDENT_SCOPE();
    for (auto f : node->all_modparams) {
      LOG_G("%s\n", f->name.c_str());
    }
  }

  if (node->all_localparams.size()) {
    LOG_G("Localparams\n");
    LOG_INDENT_SCOPE();
    for (auto f : node->all_localparams) {
      LOG_G("%s\n", f->name.c_str());
    }
  }

  if (node->constructor) {
    dump_function(node->constructor);
  }

  if (node->all_functions.size()) {
    for (auto func : node->all_functions) {
      dump_function(func);
    }
  }
}

//------------------------------------------------------------------------------

void dump_list(CNodeList* list) {
  LOG_G("(");
  for (auto i = 0; i < list->items.size(); i++) {
    if (i) LOG_G(", ");
    dump_dispatch(list->items[i]);
  }
  LOG_G(")");
}

//------------------------------------------------------------------------------

void dump_type(CNodeType* type) {
  LOG_G("Type %s", type->node_name->name.c_str());
}

//------------------------------------------------------------------------------

void dump_decl(CNodeDeclaration* decl) {
  LOG_G("Name %s, ", decl->node_name->name.c_str());
  dump_type(decl->node_type);
}

//------------------------------------------------------------------------------

void dump_field(CNodeField* field) {
  LOG_G("Field: ");
  dump_decl(field->node_decl);
  LOG_G("\n");
}

//------------------------------------------------------------------------------

void dump_struct(CNodeStruct* node) {
  LOG_G("Struct %s\n", node->name.c_str());
  LOG_INDENT_SCOPE();

  if (node->all_fields.size()) {
    for (auto f : node->all_fields) {
      dump_field(f);
    }
  }
}

//------------------------------------------------------------------------------

void dump_namespace(CNodeNamespace* node) {
  LOG_G("Namespace %s\n", node->name.c_str());
}

//------------------------------------------------------------------------------

void dump_enum(CNodeEnum* node) {
  LOG_G("Enum %s\n", node->name.c_str());
}

//------------------------------------------------------------------------------

void dump_source_file(CSourceFile* file) {
  LOG_L("Source file `%s`\n", file->filepath.c_str());

  for (auto c : file->all_classes) {
    LOG_INDENT_SCOPE();
    dump_class(c);
  }

  for (auto s : file->all_structs) {
    LOG_INDENT_SCOPE();
    dump_struct(s);
  }

  for (auto n : file->all_namespaces) {
    LOG_INDENT_SCOPE();
    dump_namespace(n);
  }

  for (auto e : file->all_enums) {
    LOG_INDENT_SCOPE();
    dump_enum(e);
  }
}

//------------------------------------------------------------------------------

void dump_repo(CSourceRepo* repo) {
  LOG_B("Repo dump:\n");
  LOG_INDENT_SCOPE();

  for (auto s : repo->search_paths) {
    LOG_G("Search path `%s`\n", s.c_str());
  }

  for (auto file : repo->source_files) {
    dump_source_file(file);
  }

  /*
  {
    LOG_G("Classes:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_classes) {
      n->dump();
      LOG("\n");
    }
    if (all_classes.empty()) LOG("\n");
  }

  {
    LOG_G("Structs:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_structs) {
      n->dump();
      LOG("\n");
    }
    if (all_structs.empty()) LOG("\n");
  }

  {
    LOG_G("Namespaces:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_namespaces) {
      n->dump();
      LOG("\n");
    }
    if (all_namespaces.empty()) LOG("\n");
  }

  {
    LOG_G("Enums:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_enums) {
      n->dump();
      LOG("\n");
    }
    if (all_enums.empty()) LOG("\n");
  }
  */
}



//------------------------------------------------------------------------------

void dump_call_graph(CNodeClass* node_class) {
  LOG_G("Class %s\n", node_class->name.c_str());

  LOG_INDENT();

  if (node_class->constructor) {
    LOG("Constructor\n");
    LOG_INDENT();
    dump_call_graph(node_class->constructor);
    LOG_DEDENT();
  }

  for (auto node_func : node_class->all_functions) {
    if (node_func->internal_callers.size()) {
      continue;
    }
    else {
      LOG("Func %s\n", node_func->name.c_str());
      LOG_INDENT();
      dump_call_graph(node_func);
      LOG_DEDENT();
    }
  }
  LOG_DEDENT();
}


void dump_call_graph(CNodeFunction* node_func) {
  for (auto r : node_func->self_reads) {
    LOG("Reads %s\n", r->path.c_str());
  }
  for (auto w : node_func->self_writes) {
    LOG("Writes %s\n", w->path.c_str());
  }

  if (node_func->internal_callees.size()) {
    for (auto c : node_func->internal_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(),
            func_name.size(), func_name.data());
      LOG_INDENT();
      dump_call_graph(c);
      LOG_DEDENT();
    }
  }

  if (node_func->external_callees.size()) {
    for (auto c : node_func->external_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(),
            func_name.size(), func_name.data());
      LOG_INDENT();
      dump_call_graph(c);
      LOG_DEDENT();
    }
  }
}


//------------------------------------------------------------------------------
