#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

struct CNodeDeclaration;
struct CNodeConstructor;
struct CNodeFunction;
struct CSourceRepo;
struct CSourceFile;
struct CNodeField;

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  void dump();

  virtual uint32_t debug_color() const override;
  virtual std::string_view get_name() const override;

  CNodeField*       get_field(std::string_view name);
  CNodeFunction*    get_method(std::string_view name);
  CNodeDeclaration* get_modparam(std::string_view name);

  bool needs_tick();
  bool needs_tock();

  Err collect_fields_and_methods();
  Err build_call_graph(CSourceRepo* repo);

  CNode* resolve(CNode* name, CSourceRepo* repo);

  CNode* resolve_scope(CNode* name);

  virtual Err emit(Cursor& cursor) override;

  //----------------------------------------

  CSourceRepo* repo;
  CSourceFile* file;
  int refcount = 0;

  std::vector<CNodeConstructor*> all_constructors;
  std::vector<CNodeFunction*>    all_methods;
  std::vector<CNodeField*>       all_fields;
  std::vector<CNodeDeclaration*> all_modparams;

  /*
  std::vector<MtField*> input_signals;
  std::vector<MtField*> output_signals;
  std::vector<MtField*> output_registers;

  std::vector<MtFuncParam*> input_method_params;
  std::vector<MtMethod*>    output_method_returns;

  std::vector<MtField*> components;
  std::vector<MtField*> private_signals;
  std::vector<MtField*> private_registers;
  std::vector<MtField*> dead_fields;
  */


};

//------------------------------------------------------------------------------
