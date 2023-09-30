#pragma once

#include "metrolib/core/Err.h"
#include "metron/Cursor.hpp"

struct CNodeAccess;
struct CNodeAssignment;
struct CNodeBuiltinType;
struct CNodeCall;
struct CNodeClass;
struct CNodeClassType;
struct CNodeCompound;
struct CNodeConstant;
struct CNodeConstructor;
struct CNodeDeclaration;
struct CNodeDoWhile;
struct CNodeEnum;
struct CNodeEnumerator;
struct CNodeEnumType;
struct CNodeExpression;
struct CNodeExpStatement;
struct CNodeField;
struct CNodeFieldExpression;
struct CNodeFor;
struct CNodeFunction;
struct CNodeIdentifier;
struct CNodeIf;
struct CNodeKeyword;
struct CNodeList;
struct CNodeLValue;
struct CNodeNamespace;
struct CNodeOperator;
struct CNodePrefixExp;
struct CNodePreproc;
struct CNodePunct;
struct CNodeSuffixExp;
struct CNodeQualifiedIdentifier;
struct CNodeReturn;

//==============================================================================

struct Emitter {
  Emitter(Cursor& c) : cursor(c) {}

  Err emit(CNodeAccess* node);
  Err emit(CNodeAssignment* node);
  Err emit(CNodeBuiltinType* node);
  Err emit(CNodeCall* node);
  Err emit(CNodeClass* node);
  Err emit(CNodeClassType* node);
  Err emit(CNodeCompound* node);
  Err emit(CNodeConstant* node);
  Err emit(CNodeConstructor* node);
  Err emit(CNodeDeclaration* node);
  Err emit(CNodeDoWhile* node);
  Err emit(CNodeEnum* node);
  Err emit(CNodeEnumerator* node);
  Err emit(CNodeEnumType* node);
  Err emit(CNodeExpression* node);
  Err emit(CNodePrefixExp* node);
  Err emit(CNodeSuffixExp* node);
  Err emit(CNodeOperator* node);
  Err emit(CNodeExpStatement* node);
  Err emit(CNodeField* node);
  Err emit(CNodeFieldExpression* node);
  Err emit(CNodeFor* node);
  Err emit(CNodeFunction* node);
  Err emit(CNodeIdentifier* node);
  Err emit(CNodeIf* node);
  Err emit(CNodeKeyword* node);
  Err emit(CNodeList* node);
  Err emit(CNodeLValue* node);
  Err emit(CNodeNamespace* node);
  Err emit(CNodePreproc* node);
  Err emit(CNodePunct* node);
  Err emit(CNodeQualifiedIdentifier* node);
  Err emit(CNodeReturn* node);

  Cursor& cursor;
};

//==============================================================================
