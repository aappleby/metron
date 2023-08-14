#pragma once

#include "CNode.hpp"

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  void init(const char* match_name, SpanType span, uint64_t flags) {
    CNode::init(match_name, span, flags);
  }

  std::string class_name() {
    return child("name")->as_string();
  }
};

struct CNodeDeclaration : public CNode {};
struct CNodeEnum : public CNode {};
struct CNodeFunction : public CNode {};
struct CNodeNamespace : public CNode {};
struct CNodePreproc : public CNode {};
struct CNodeStruct : public CNode {};
struct CNodeTemplate : public CNode {};
struct CNodeTypedef : public CNode {};
struct CNodeUnion : public CNode {};

struct CNodeTranslationUnit : public CNode {
  virtual Err emit(Cursor& c) {
    //return c.emit_print("Hello World\n");
    printf("hello world\n");
    return Err();
  }
};

//------------------------------------------------------------------------------
