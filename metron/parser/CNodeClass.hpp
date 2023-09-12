#pragma once
#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

struct CNodeDeclaration;
struct CNodeConstructor;
struct CNodeFunction;
struct CNodeEnum;
struct CSourceRepo;
struct CSourceFile;
struct CNodeField;
struct CNodeKeyword;
struct CNodeList;
struct CNodeIdentifier;

//------------------------------------------------------------------------------

struct CNodeAccess : public CNode {
  uint32_t debug_color() const override { return COL_VIOLET; }
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//------------------------------------------------------------------------------

struct CNodeTemplate : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  uint32_t debug_color() const override { return 0x00FFFF; }
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_template = nullptr;
  CNodeList*    node_params = nullptr;
  CNodeClass*   node_class = nullptr;

  std::vector<CNodeDeclaration*> params;
};

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  //----------

  uint32_t debug_color() const override;
  std::string_view get_name() const override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
  virtual void dump();

  CSourceRepo* get_repo() override {
    return repo;
  }

  //----------

  CNodeField*       get_field(std::string_view name);
  CNodeFunction*    get_function(std::string_view name);
  CNodeDeclaration* get_modparam(std::string_view name);
  CNodeEnum*        get_enum(std::string_view name);

  bool needs_tick();
  bool needs_tock();

  Err collect_fields_and_methods();
  Err build_call_graph(CSourceRepo* repo);

  Err emit_module_ports(Cursor& cursor);
  Err emit_template_parameter_list(Cursor& cursor);
  Err emit_function_ports(CNodeFunction* f, Cursor& cursor);
  Err emit_field_ports(CNodeField* f, Cursor& cursor);

  void dump_fields();

  //----------------------------------------

  CNodeKeyword*    node_class = nullptr;
  CNodeIdentifier* node_name = nullptr;
  CNodeList*       node_body = nullptr;

  CSourceRepo* repo = nullptr;
  CSourceFile* file = nullptr;
  int refcount = 0;

  std::vector<CNodeFunction*>    top_functions;

  CNodeConstructor* constructor = nullptr;
  std::vector<CNodeFunction*>    all_functions;
  std::vector<CNodeField*>       all_fields;
  std::vector<CNodeDeclaration*> all_modparams;
  std::vector<CNodeField*>       all_localparams;
  std::vector<CNodeEnum*>        all_enums;

  std::vector<CNodeField*> input_signals;
  std::vector<CNodeField*> output_signals;
  std::vector<CNodeField*> output_registers;

  std::vector<CNodeField*> components;
  std::vector<CNodeField*> private_signals;
  std::vector<CNodeField*> private_registers;
  std::vector<CNodeField*> dead_fields;

  /*
  std::vector<MtFuncParam*> input_method_params;
  std::vector<MtMethod*>    output_method_returns;
  */


};

//------------------------------------------------------------------------------
