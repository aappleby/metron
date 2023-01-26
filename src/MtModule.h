#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Err.h"
#include "MtNode.h"

struct MtMethod;
struct MtModLibrary;
struct MtSourceFile;
struct MtField;
struct MtModParam;
struct MtFuncParam;
struct MtContext;

typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtModLibrary* lib) : lib(lib) {}
  ~MtModule();

  const char* cname() const { return mod_name.c_str(); }
  std::string name() const { return mod_name; }

  CHECK_RETURN Err init(MtSourceFile* source_file, MnNode node);

  MtMethod* get_method(const std::string& name);
  MtField* get_field(const std::string& name);
  MtField* get_field(MnNode node);
  MtField* get_component(const std::string& name);
  MtField* get_enum(const std::string& name);

  MtField* get_input_signal(const std::string& name);
  MtField* get_output_signal(const std::string& name);
  MtField* get_output_register(const std::string& name);

  void dump();
  void dump_method_list(const std::vector<MtMethod*>& methods) const;

  CHECK_RETURN Err collect_parts();
  CHECK_RETURN Err build_call_graph();
  CHECK_RETURN Err categorize_fields(bool verbose);

  bool needs_tick() const;
  bool needs_tock() const;
  bool is_port(const std::string& name) const;

  //--------------------

  MtModLibrary* lib = nullptr;
  MtSourceFile* source_file = nullptr;
  std::string mod_name;
  MtMethod* constructor = nullptr;
  int refcount = 0;
  MtContext* ctx = nullptr;

  MnNode root_node;
  MnNode mod_class;
  MnNode mod_template;
  MnNode mod_param_list;

  //----------

  std::vector<MtModParam*> all_modparams;
  std::vector<MtField*> all_fields;
  std::vector<MtField*> all_enums;
  std::vector<MtMethod*> all_methods;
  std::vector<MtFuncParam*> input_method_params;

  //----------

  // Categorized fields

  std::vector<MtField*> input_signals;

  std::vector<MtField*> output_signals;
  std::vector<MtField*> output_registers;

  std::vector<MtMethod*> output_method_returns;

  std::vector<MtField*> components;
  std::vector<MtField*> private_signals;
  std::vector<MtField*> private_registers;

  std::vector<MtField*> dead_fields;
};

//------------------------------------------------------------------------------
