#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "metrolib/core/Log.h"
#include "metron/tools/MtUtils.h"

//------------------------------------------------------------------------------

struct CNodeTranslationUnit : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeNamespace : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeFieldExpression : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeQualifiedIdentifier : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeText : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  virtual uint32_t debug_color() const;
  virtual std::string_view get_name() const;
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------
