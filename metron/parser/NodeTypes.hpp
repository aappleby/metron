#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

struct CContext;
struct CNodeConstructor;
struct CNodeTemplateParams;
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

struct CNodeTemplate : public CNode {
  virtual uint32_t debug_color() const { return 0x00FFFF; }
  virtual Err emit(Cursor& cursor);
};

//------------------------------------------------------------------------------

struct CNodeUnion : public CNode {
};

//------------------------------------------------------------------------------

struct CNodeEnum : public CNode {
};

//==============================================================================
//
//==============================================================================

struct CNodeExpression : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeDeclaration : public CNode {
  virtual std::string_view get_name() const {
    return child("decl_name")->get_name();
  }

  virtual uint32_t debug_color() const { return 0xFF00FF; }
  virtual Err emit(Cursor& cursor);
};

//------------------------------------------------------------------------------

struct CNodeType : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeConstant : public CNode {
  virtual uint32_t debug_color() const { return 0x0000FF; }
};

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNode {
  virtual uint32_t debug_color() const { return 0x0000FF; }

  virtual Err emit(Cursor& cursor) {
    return cursor.emit_replacement(this, "{{CNodeConstructor}}");
  }
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
  virtual std::string_view get_name() const { return get_text(); }
  virtual Err emit(Cursor& cursor) {
    return cursor.emit_default(this);
  }
};

struct CNodeFieldExpression : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
};

struct CNodeQualifiedIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
};

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  virtual uint32_t debug_color() const { return 0xFF0040; }
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF88; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
};

//------------------------------------------------------------------------------

struct CNodeAccess : public CNode {
  virtual Err emit(Cursor& cursor) override {
    return cursor.comment_out(this);
  }
};


//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual Err emit(Cursor& cursor) override {
    return cursor.emit_default(this);
  }
};

//==============================================================================
// Control flow
//==============================================================================

struct CNodeFor : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeIf : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeReturn : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeSwitch : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeCase : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeDefault : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeWhile : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeCompound : public CNode {
  virtual uint32_t debug_color() const { return 0xFF8888; }
};

struct CNodeBreak : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeContinue : public CNode {
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//==============================================================================
//
//==============================================================================
