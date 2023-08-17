#pragma once
#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

struct CContext;
struct CNodeDeclaration;
struct CNodeFunction;
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
  static TokenSpan match(CContext& ctx, TokenSpan body);
};

//------------------------------------------------------------------------------

struct CNodePreproc : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x00BBBB; }
  virtual Err emit(Cursor& cursor);
};

//==============================================================================
// Data Structures
//==============================================================================

struct CNodeClass : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);

  virtual uint32_t debug_color() const { return 0x00FF00; }
  void init(const char* match_name, SpanType span, uint64_t flags);

  virtual Err emit(Cursor& cursor);

  virtual std::string_view get_name() const {
    return child("name")->get_name();
  }

  Err collect_fields_and_methods();

  std::string name;
  std::vector<CNodeDeclaration*> all_modparams;
  std::vector<CNodeConstructor*> all_constructors;
  std::vector<CNodeDeclaration*> all_fields;
  std::vector<CNodeFunction*>    all_methods;
};

//------------------------------------------------------------------------------

struct CNodeTemplate : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x00FFFF; }
  virtual Err emit(Cursor& cursor);
};

//------------------------------------------------------------------------------

struct CNodeStruct : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFFAAAA; }

  Err collect_fields_and_methods();
};

//------------------------------------------------------------------------------

struct CNodeUnion : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
};

//------------------------------------------------------------------------------

struct CNodeEnum : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
};

//------------------------------------------------------------------------------

struct CNodeFieldList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF8080; }
  virtual Err emit(Cursor& cursor);
};

//==============================================================================
//
//==============================================================================

struct CNodeExpression : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeDeclaration : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);

  virtual std::string_view get_name() const {
    return child("decl_name")->get_name();
  }

  virtual uint32_t debug_color() const { return 0xFF00FF; }
  virtual Err emit(Cursor& cursor) {
    return cursor.emit_replacement(this, "{{CNodeDeclaration}}");
  }
};

//------------------------------------------------------------------------------

struct CNodeType : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeFunction : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0000FF; }

  virtual Err emit(Cursor& cursor) {
    return cursor.emit_replacement(this, "{{CNodeFunction}}");
  }
};

//------------------------------------------------------------------------------

struct CNodeQualifier : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0000FF; }
};

//------------------------------------------------------------------------------

struct CNodeConstant : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0000FF; }
};

//------------------------------------------------------------------------------

struct CNodeConstructor : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0000FF; }

  virtual Err emit(Cursor& cursor) {
    return cursor.emit_replacement(this, "{{CNodeConstructor}}");
  }
};

//------------------------------------------------------------------------------

struct CNodeIdentifier : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x80FF80; }

  virtual std::string_view get_name() const { return as_string_view(); }

  virtual Err emit(Cursor& cursor) {
    return cursor.emit_default(this);
  }
};

//------------------------------------------------------------------------------

struct CNodePunct : public CNode {
  virtual uint32_t debug_color() const { return 0x00FFFF; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeAccess : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF88FF; }
  virtual Err emit(Cursor& cursor) {
    return cursor.comment_out(this);
  }
};

//------------------------------------------------------------------------------

struct CNodeCall : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF0040; }
};

//------------------------------------------------------------------------------

// (int a = 1, int b = 2)
struct CNodeDeclList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

// <int a = 1, int b = 2>
struct CNodeTDeclList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

// (a * 7, b / 2)
struct CNodeExpList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

// <a * 7, b / 2>
struct CNodeTExpList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

// [a/2, b/2]
struct CNodeIndexList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeList : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFFFF00; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeTemplateParams : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0x0088FF; }
};

//------------------------------------------------------------------------------

struct CNodeKeyword : public CNode {
  virtual uint32_t debug_color() const { return 0xFFFF88; }
  virtual Err emit(Cursor& cursor) { return cursor.emit_default(this); }
};

//------------------------------------------------------------------------------

struct CNodeTypedef : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
};

//==============================================================================
// Control flow
//==============================================================================

struct CNodeFor : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeIf : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeReturn : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeSwitch : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeCase : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeDefault : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeWhile : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeCompound : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF8888; }
};

struct CNodeBreak : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

struct CNodeContinue : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);
  virtual uint32_t debug_color() const { return 0xFF00FF; }
};

//==============================================================================
//
//==============================================================================
