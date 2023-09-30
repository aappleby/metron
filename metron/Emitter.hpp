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
struct CNodePrefixExp;

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

  Cursor& cursor;
};

//==============================================================================
