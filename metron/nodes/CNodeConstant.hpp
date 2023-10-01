#pragma once

#include "metron/nodes/CNodeExpression.hpp"

//==============================================================================

struct CNodeConstant : public CNodeExpression {
};

struct CNodeConstInt    : public CNodeConstant {};
struct CNodeConstFloat  : public CNodeConstant {};
struct CNodeConstChar   : public CNodeConstant {};
struct CNodeConstString : public CNodeConstant {};

//==============================================================================
