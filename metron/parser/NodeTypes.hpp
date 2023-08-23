#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include "CNodeType.hpp"

#include "metrolib/core/Log.h"

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

struct CNodeDeclaration : public CNode {
  virtual std::string_view get_name() const {
    return child("decl_name")->get_name();
  }

  std::string_view get_type_name() const {
    auto decl_type = child<CNodeType>();
    return decl_type->child_head->get_text();
  }

  bool is_array() const {
    return child("decl_array") != nullptr;
  }


  virtual uint32_t debug_color() const { return 0xFF00FF; }
  virtual Err emit(Cursor& cursor);

  CNodeClass*  _type_class = nullptr;
  CNodeStruct* _type_struct = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeConstant : public CNode {
  virtual uint32_t debug_color() const { return COL_SKY; }
  virtual Err emit(Cursor& cursor) {
    return cursor.emit_raw(this);
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
  virtual Err trace(CInstance* instance) {
    dump_tree();
    return CNode::trace(instance);
  }
};

struct CNodeQualifiedIdentifier : public CNode {
  virtual uint32_t debug_color() const { return 0x80FF80; }
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF88; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeOperator : public CNode {
  virtual uint32_t debug_color() const { return COL_BLUE; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeAssignment : public CNode {
  virtual uint32_t debug_color() const { return COL_TEAL; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
};

struct CNodeTypeName : public CNode {
  virtual uint32_t debug_color() const { return COL_ORANGE; }
};

struct CNodeList : public CNode {
  virtual uint32_t debug_color() const { return 0xCCCCCC; }
};

//------------------------------------------------------------------------------

struct CNodeAccess : public CNode {
  virtual uint32_t debug_color() const { return COL_VIOLET; }
  virtual Err emit(Cursor& cursor) override;
};

struct CNodeText : public CNode {
  virtual uint32_t debug_color() const { return 0x888888; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

struct CNodeCast : public CNode {
  virtual uint32_t debug_color() const { return 0x888888; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const { return 0x88FF88; }
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
  virtual uint32_t debug_color() const;
  virtual Err trace(CInstance* instance);
};

//==============================================================================
//
//==============================================================================
