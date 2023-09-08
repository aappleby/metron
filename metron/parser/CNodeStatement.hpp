#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"
#include "metron/tools/MtUtils.h"

//==============================================================================

#if 0

statement:
  labeled-statement
  compound-statement
  expression-statement
  selection-statement
  iteration-statement
  jump-statement

labeled-statement:
  identifier : statement
  case constant-expression : statement
  default : statement

compound-statement:
  { block-item-listopt }

block-item-list:
  block-item
  block-item-list block-item

block-item:
  declaration
  statement

expression-statement:
  expressionopt ;

selection-statement:
  if ( expression ) statement
  if ( expression ) statement else statement
  switch ( expression ) statement

iteration-statement:
  while ( expression ) statement
  do statement while ( expression ) ;
  for ( expressionopt ; expressionopt ; expressionopt ) statement
  for ( declaration expressionopt ; expressionopt ) statement

jump-statement:
  goto identifier ;
  continue ;
  break ;
  return expressionopt ;

#endif

//==============================================================================

struct CNodeStatement : public CNode {
  uint32_t debug_color() const override;
protected:
  CNodeStatement() {}
};

//------------------------------------------------------------------------------

struct CNodeExpStatement : public CNodeStatement {
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(CCall* call) override;
};

struct CNodeAssignment : public CNodeStatement {
  CHECK_RETURN Err emit(Cursor& c) override;
  CHECK_RETURN Err trace(CCall* call) override;
};

//------------------------------------------------------------------------------

struct CNodeFor : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

struct CNodeIf : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

struct CNodeReturn : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

//------------------------------------------------------------------------------

struct CNodeSwitch : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

struct CNodeCase : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

struct CNodeDefault : public CNodeStatement {
  CHECK_RETURN Err trace(CCall* call) override;
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNodeStatement {
};

struct CNodeWhile : public CNodeStatement {
};

struct CNodeCompound : public CNodeStatement {
  CHECK_RETURN Err emit_block(Cursor& c, std::string ldelim, std::string rdelim);

  CHECK_RETURN Err trace(CCall* call) override;
};

//------------------------------------------------------------------------------
