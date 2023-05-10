#include "metron/parser/Parser.h"

#include "metrolib/core/Log.h"

#include "metron/app/MtSourceFile.h"
#include "metron/nodes/MtNode.h"
#include "metron/nodes/TreeSymbols.h"


#include <vector>
#include <span>
#include <functional>
#include <map>

typedef std::function<PNode*(Parser* p, TSNode n)> parse_cb;
typedef std::map<TSSymbol, parse_cb> parse_map;

parse_map root_parse_map = {
  { alias_sym_field_identifier,         &Parser::parse_error },
  { alias_sym_namespace_identifier,     &Parser::parse_error },
  { alias_sym_type_identifier,          &Parser::parse_error },
  { sym_access_specifier,               &Parser::parse_error },
  { sym_argument_list,                  &Parser::parse_error },
  { sym_array_declarator,               &Parser::parse_error },
  { sym_assignment_expression,          &Parser::parse_error },
  { sym_binary_expression,              &Parser::parse_error },
  { sym_break_statement,                &Parser::parse_error },
  { sym_call_expression,                &Parser::parse_error },
  { sym_case_statement,                 &Parser::parse_error },
  { sym_char_literal,                   &Parser::parse_error },
  { sym_class_specifier,                &Parser::parse_error },
  { sym_comment,                        &Parser::parse_error },
  { sym_compound_statement,             &Parser::parse_error },
  { sym_condition_clause,               &Parser::parse_error },
  { sym_conditional_expression,         &Parser::parse_error },
  { sym_declaration,                    &Parser::parse_error },
  { sym_enum_specifier,                 &Parser::parse_error },
  { sym_enumerator_list,                &Parser::parse_error },
  { sym_enumerator,                     &Parser::parse_error },
  { sym_expression_statement,           &Parser::parse_error },
  { sym_field_declaration_list,         &Parser::parse_error },
  { sym_field_declaration,              &Parser::parse_error },
  { sym_field_expression,               &Parser::parse_error },
  { sym_for_statement,                  &Parser::parse_error },
  { sym_function_declarator,            &Parser::parse_error },
  { sym_function_definition,            &Parser::parse_error },
  { sym_identifier,                     &Parser::parse_error },
  { sym_if_statement,                   &Parser::parse_error },
  { sym_init_declarator,                &Parser::parse_error },
  { sym_initializer_list,               &Parser::parse_error },
  { sym_namespace_definition,           &Parser::parse_error },
  { sym_nullptr,                        &Parser::parse_error },
  { sym_number_literal,                 &Parser::parse_error },
  { sym_parameter_declaration,          &Parser::parse_error },
  { sym_parameter_list,                 &Parser::parse_error },
  { sym_parenthesized_expression,       &Parser::parse_error },
  { sym_pointer_declarator,             &Parser::parse_error },
  { sym_preproc_arg,                    &Parser::parse_error },
  { sym_preproc_call,                   &Parser::parse_error },
  { sym_preproc_def,                    &Parser::parse_error },
  { sym_preproc_else,                   &Parser::parse_error },
  { sym_preproc_if,                     &Parser::parse_error },
  { sym_preproc_ifdef,                  &Parser::parse_preproc_ifdef },
  { sym_preproc_include,                &Parser::parse_error },
  { sym_primitive_type,                 &Parser::parse_error },
  { sym_qualified_identifier,           &Parser::parse_error },
  { sym_return_statement,               &Parser::parse_error },
  { sym_sized_type_specifier,           &Parser::parse_error },
  { sym_storage_class_specifier,        &Parser::parse_error },
  { sym_string_literal,                 &Parser::parse_error },
  { sym_struct_specifier,               &Parser::parse_error },
  { sym_subscript_expression,           &Parser::parse_error },
  { sym_switch_statement,               &Parser::parse_error },
  { sym_template_argument_list,         &Parser::parse_error },
  { sym_template_declaration,           &Parser::parse_error },
  { sym_template_type,                  &Parser::parse_error },
  { sym_translation_unit,               &Parser::parse_translation_unit },
  { sym_type_definition,                &Parser::parse_error },
  { sym_type_descriptor,                &Parser::parse_error },
  { sym_unary_expression,               &Parser::parse_error },
  { sym_update_expression,              &Parser::parse_error },
  { sym_using_declaration,              &Parser::parse_error },
};




typedef std::span<const char> cspan;

struct PNode;
struct PGap;

struct PNode {
  PNode* parent;
  cspan  span;
  PNode* gap;
};

struct PExpression;
struct PFieldExpression;
struct PType;
struct PToken;
struct PFieldExpression;
struct PStringLiteral;
struct PTemplateParameterList;
struct PTypeIdentifier;
struct PFieldDeclarationList;

//------------------------------------------------------------------------------

struct PDeclarator {};
struct PExpression {};
struct PStatement {};
struct PDeclaration {};

//------------------------------------------------------------------------------

struct PAccessSpecifier : public PNode {
};

struct PArgumentList : public PNode {
  PToken*      lit_lparen;
  PExpression* arg_head;
  PExpression* arg_tail;
  PToken*      lit_rparen;
};

struct PAssignmentExpression : public PNode, public PExpression {
  PExpression* lhs;
  PToken*      op;
  PExpression* rhs;
};

struct PBinaryExpression : public PNode, public PExpression {
  PExpression* lhs;
  PToken*      op;
  PExpression* rhs;
};

struct PCallExpression : public PNode {
  PFieldExpression* field;
  PArgumentList*    args;
};

struct PClassSpecifier : public PNode {
  PToken* lit_class;
  PTypeIdentifier* name;
  PFieldDeclarationList* body;
};

struct PComment : public PNode {};

struct PCompoundStatement : public PNode {
  PToken*     lit_lbrace;
  PStatement* statement_head;
  PStatement* statement_tail;
  PToken*     lit_rbrace;
};

struct PConditionClause : public PNode {};

struct PFieldExpression : public PNode {};

struct PFieldDeclarationList : public PNode {};

struct PFunctionDeclarator : public PNode {
  PType*       type;
  PDeclarator* declarator;
  PStatement*  body;
};

struct PIdentifier : public PNode {};

struct PIfStatement : public PNode {
  PToken*           lit_if;
  PConditionClause* condition;
  PStatement*       consequence;
  PToken*           lit_else;
  PStatement*       alternative;
};

struct PNamespaceIdentifier : public PNode {};

struct PParameterList : public PNode {
  PToken*      lit_lparen;
  PExpression* arg_head;
  PExpression* arg_tail;
  PToken*      lit_rparen;
};

struct PPreprocIfdef : public PNode {
  PToken*      lit_ifdef; // or ifndef, because treesitter...
  PIdentifier* name;
  std::vector<PNode*> children;
  PToken*      lit_endif;
};

struct PPreprocDef : public PNode{};

struct PPreprocInclude : public PNode {
  PToken* lit_include;
  PStringLiteral* path;
};

struct PQualifiedIdentifier : public PNode {
  PNamespaceIdentifier* scope;
  PToken* lit_coloncolon;
  PIdentifier* name;
};

struct PReturnStatement : public PNode {
  PToken* lit_return;
  PExpression* expression;
  PToken* lit_semi;
};

struct PSpace : public PNode {};

struct PStringLiteral : public PNode {};

struct PToken : public PNode {};

struct PTemplateArgumentList : public PNode {};

struct PTemplateDeclaration : public PNode, public PDeclaration {
  PToken* lit_template;
  PTemplateParameterList* parameters;

};

struct PTemplateParameterList : public PNode {};

struct PTemplateType : public PNode {
  PIdentifier*           name;
  PTemplateArgumentList* args;
  PClassSpecifier*       pclass;
};

struct PTranslationUnit : public PNode {
  std::vector<PNode*> children;
};

struct PTypeIdentifier : public PNode {
};

struct PUsingDeclaration : public PNode {
  PToken*      lit_using;
  PToken*      lit_namespace;
  PIdentifier* identifier;
  PToken*      lit_semi;
};

//------------------------------------------------------------------------------

void Parser::parse(MtSourceFile* source) {
  this->source = source;

  LOG_G("Parser::parse()\n");
  TSNode n = ts_tree_root_node(source->tree);

  PNode* root = parse(n);
  printf("Root at %p\n", root);

  LOG_G("Parser::parse() done\n");
}

//------------------------------------------------------------------------------

PNode* Parser::parse(TSNode n) {
  if (auto it = root_parse_map.find(ts_node_symbol(n)); it != root_parse_map.end()) {
    return (*it).second(this, n);
  }
  else {
    parse_error(n);
    return nullptr;
  }
}

//------------------------------------------------------------------------------

PNode* Parser::parse_error(TSNode n) {
  MnNode node(n, ts_node_symbol(n), 0, source);
  node.dump_tree();
  exit(1);
}

//------------------------------------------------------------------------------

PNode* Parser::parse_preproc_ifdef(TSNode n) {

  MnNode node(n, ts_node_symbol(n), 0, source);
  node.dump_tree(0, 0, 1);

  for (int i = 0; i < ts_node_child_count(n); i++) {
    auto c = ts_node_child(n, i);
  }

  return nullptr;
}

//------------------------------------------------------------------------------

PNode* Parser::parse_translation_unit(TSNode n) {

  auto p = new PTranslationUnit();

  for (int i = 0; i < ts_node_child_count(n); i++) {
    auto c = ts_node_child(n, i);
    if (ts_node_is_named(c)) {
      p->children.push_back(parse(c));
    }
    else {
      p->children.push_back(new PToken());
    }
  }

  return p;
}

//------------------------------------------------------------------------------
