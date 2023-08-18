#include "CNode.hpp"
#include "Cursor.hpp"

#include <assert.h>
#include <vector>

struct CNodeDeclaration;
struct CNodeConstructor;
struct CNodeFunction;
struct CSourceRepo;
struct CSourceFile;

//------------------------------------------------------------------------------

struct CNodeClass : public CNode {
  static TokenSpan match(CContext& ctx, TokenSpan body);

  void init(const char* match_name, SpanType span, uint64_t flags);

  virtual uint32_t debug_color() const override { return 0x00FF00; }
  virtual Err emit(Cursor& cursor) override;
  virtual std::string_view get_name() const override;

  bool needs_tick();
  bool needs_tock();

  Err collect_fields_and_methods();
  Err build_call_graph();

  //----------------------------------------

  CSourceRepo* repo;
  CSourceFile* file;
  int refcount = 0;

  std::vector<CNodeDeclaration*> all_modparams;
  std::vector<CNodeConstructor*> all_constructors;
  std::vector<CNodeDeclaration*> all_fields;
  std::vector<CNodeFunction*>    all_methods;

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
