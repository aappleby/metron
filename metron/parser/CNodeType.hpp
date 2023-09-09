#pragma once

#include "CNode.hpp"
#include "Cursor.hpp"

//------------------------------------------------------------------------------

struct CNodeType : public CNode {
  uint32_t debug_color() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

protected:
  CNodeType() {}
};

struct CNodeBuiltinType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }

  Err trace(CCall* call) override {
    return Err();
  }

};

struct CNodeStructType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

struct CNodeClassType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
};

struct CNodeUnionType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
};

struct CNodeEnumType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
};

struct CNodeTypedefType : public CNodeType {
  std::string_view get_name() const override {
    return get_text();
  }
};

/*
struct CNodeTypeName : public CNode {
  virtual uint32_t debug_color() const { return COL_ORANGE; }
};
*/

//------------------------------------------------------------------------------
