#pragma once

#include "metrolib/core/Err.h"
#include "metron/Cursor.hpp"

struct CNodeAccess;
struct CNodeAssignment;
struct CNodeBuiltinType;
struct CNodeCall;
struct CNodeClass;
struct CNodeClassType;

//==============================================================================

struct Emitter {
  Emitter(Cursor& c) : cursor(c) {}

  Err emit(CNodeAccess* node);
  Err emit(CNodeAssignment* node);
  Err emit(CNodeBuiltinType* node);
  Err emit(CNodeCall* node);
  Err emit(CNodeClass* node);
  Err emit(CNodeClassType* node);

  Cursor& cursor;
};

//==============================================================================
