#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"
#include "metron/tools/MtUtils.h"

//------------------------------------------------------------------------------

struct CNodeTranslationUnit : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeNamespace : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeFieldExpression : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeQualifiedIdentifier : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeText : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;
  virtual Err emit(Cursor& c) override;
  virtual Err trace(IContext* context) override;
};

//------------------------------------------------------------------------------
