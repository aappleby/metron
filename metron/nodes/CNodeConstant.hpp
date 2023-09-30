#pragma once

#include "metron/nodes/CNodeExpression.hpp"

//==============================================================================

struct CNodeConstant : public CNodeExpression {
  CHECK_RETURN Err emit(Cursor& cursor) override;
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;

protected:
  CNodeConstant() {}
};

struct CNodeConstInt    : public CNodeConstant {};
struct CNodeConstFloat  : public CNodeConstant {};
struct CNodeConstChar   : public CNodeConstant {};
struct CNodeConstString : public CNodeConstant {};

//==============================================================================
