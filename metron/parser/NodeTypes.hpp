#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "CNodeType.hpp"

#include "metrolib/core/Log.h"
#include "metron/tools/MtUtils.h"

#include <assert.h>
#include <vector>

struct CContext;
struct CNodeConstructor;
struct CNodeDeclaration;

//==============================================================================
// Toplevel stuff
//==============================================================================

struct CNodeTranslationUnit : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF00; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeNamespace : public CNode {
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  virtual uint32_t debug_color() const { return 0x00BBBB; }
  virtual Err emit(Cursor& cursor);
};

//==============================================================================
// Data Structures
//==============================================================================

struct CNodeIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
  virtual std::string_view get_name() const { return get_text(); }
  virtual Err emit(Cursor& cursor) {
    return cursor.emit_default(this);
  }

  virtual Err trace(CInstance* instance, TraceAction action);
};

struct CNodeFieldExpression : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
  virtual Err trace(CInstance* instance, TraceAction action) {
    dump_tree();
    return CNode::trace(instance, action);
  }
};

struct CNodeQualifiedIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
};

//------------------------------------------------------------------------------

struct CNodeText : public CNode {
  virtual uint32_t debug_color() const { return 0x888888; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF88; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
};

struct CNodeList : public CNode {
  virtual uint32_t debug_color() const { return 0xCCCCCC; }
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const { return 0x88FF88; }
  virtual Err emit(Cursor& cursor) override {
    return cursor.emit_default(this);
  }
  virtual Err trace(CInstance* instance, TraceAction action) { return Err(); }
};

//==============================================================================
//
//==============================================================================
