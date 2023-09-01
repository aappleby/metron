#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"
#include "metron/tools/MtUtils.h"

//------------------------------------------------------------------------------

struct CNodeTranslationUnit : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeNamespace : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeFieldExpression : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeQualifiedIdentifier : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeText : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------
