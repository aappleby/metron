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

  Cursor& cursor;
};

//==============================================================================
