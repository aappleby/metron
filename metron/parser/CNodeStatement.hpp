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
  virtual uint32_t debug_color() const;
protected:
  CNodeStatement() {}
};

//------------------------------------------------------------------------------

struct CNodeExpStatement : public CNodeStatement {
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

struct CNodeAssignment : public CNodeStatement {
  virtual Err emit(Cursor& c);
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeFor : public CNodeStatement {
};

struct CNodeIf : public CNodeStatement {
};

struct CNodeReturn : public CNodeStatement {
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------

struct CNodeSwitch : public CNodeStatement {
};

struct CNodeCase : public CNodeStatement {
};

struct CNodeDefault : public CNodeStatement {
};

//------------------------------------------------------------------------------

struct CNodeDoWhile : public CNodeStatement {
};

struct CNodeWhile : public CNodeStatement {
};

struct CNodeCompound : public CNodeStatement {
  virtual Err trace(CInstance* instance, TraceAction action);
};

//------------------------------------------------------------------------------
