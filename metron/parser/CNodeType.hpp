#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeType : public CNode {
  virtual uint32_t debug_color() const;
  virtual Err emit(Cursor& cursor);

protected:
  CNodeType() {}
};

struct CNodeBuiltinType : public CNodeType {
};

struct CNodeStructType : public CNodeType {
};

struct CNodeClassType : public CNodeType {
};

struct CNodeUnionType : public CNodeType {
};

struct CNodeEnumType : public CNodeType {
};

struct CNodeTypedefType : public CNodeType {
};

/*
struct CNodeTypeName : public CNode {
  virtual uint32_t debug_color() const { return COL_ORANGE; }
};
*/

//------------------------------------------------------------------------------
