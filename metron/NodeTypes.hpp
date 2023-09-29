#pragma once

#include "CNode.hpp"

struct CNodeCall;
struct CNodeClass;
struct CNodeCompound;
struct CNodeConstructor;
struct CNodeDeclaration;
struct CNodeEnum;
struct CNodeExpression;
struct CNodeField;
struct CNodeFieldExpression;
struct CNodeFunction;
struct CNodeIdentifier;
struct CNodeKeyword;
struct CNodeList;
struct CNodeNamespace;
struct CNodePreproc;
struct CNodePunct;
struct CNodeQualifiedIdentifier;
struct CNodeStatement;
struct CNodeStruct;
struct CNodeText;
struct CNodeTranslationUnit;
struct CNodeType;
struct CNodeTypedef;

struct Cursor;

#include "CNodeCall.hpp"
#include "CNodeClass.hpp"
#include "CNodeDeclaration.hpp"
#include "CNodeExpression.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStatement.hpp"
#include "CNodeStruct.hpp"
#include "CNodeType.hpp"

#include "metrolib/core/Log.h"
#include "metron/MtUtils.h"

//------------------------------------------------------------------------------

struct CNodeTranslationUnit : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeNamespace : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  Err collect_fields_and_methods();

  CNodeField* get_field(std::string_view name);

  void dump() const override;

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeField*> all_fields;
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  void dump() const override {
    auto text = get_text();
    LOG_B("CNodePunct \"%.*s\"\n", text.size(), text.data());
  }
};

//------------------------------------------------------------------------------

struct CNodeFieldExpression : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  CNodeIdentifier* node_path = nullptr;
  CNodeIdentifier* node_name = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeQualifiedIdentifier : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeText : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

  std::vector<CNode*> items;
};

//------------------------------------------------------------------------------
