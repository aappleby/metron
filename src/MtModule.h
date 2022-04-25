#pragma once
#include <map>
#include <set>
#include <string>

#include "Err.h"
#include "MtNode.h"

struct MtMethod;
struct MtModLibrary;
struct MtSourceFile;
struct MtField;
struct MtModParam;

typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtModLibrary* lib) : lib(lib) {}

  const char* cname() const { return mod_name.c_str(); }
  std::string name() const { return mod_name; }

  CHECK_RETURN Err init(MtSourceFile* source_file, MnTemplateDecl node);
  CHECK_RETURN Err init(MtSourceFile* source_file, MnClassSpecifier node);

  MtMethod* get_method(const std::string& name);
  MtField* get_field(const std::string& name);

  void dump();
  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;

  CHECK_RETURN Err collect_parts();
  CHECK_RETURN Err trace();
  CHECK_RETURN Err build_call_graph();

  //--------------------

  MtModLibrary* lib = nullptr;
  MtSourceFile* source_file = nullptr;
  std::string mod_name;
  MnNode root_node;
  MnClassSpecifier mod_class;
  MnTemplateDecl mod_template;
  MnTemplateParamList mod_param_list;
  // MtMethod* constructor = nullptr;
  int refcount = 0;

  //----------

  std::vector<MtModParam*> all_modparams;
  std::vector<MtField*> all_fields;
  std::vector<MtField*> all_enums;
  std::vector<MtMethod*> all_methods;

  //----------

#if 0
  std::vector<MtField*> inputs;
  std::vector<MtField*> outputs;
  std::vector<MtField*> signals;
  std::vector<MtField*> registers;
  std::vector<MtField*> components;

  //----------

  // FIXME not actually doing anything with this yet?
  std::vector<MtFuncParam*> localparams;

  std::vector<MtField*> input_signals;
  std::vector<MtField*> output_signals;
  std::vector<MtField*> public_registers;

  std::vector<MtFuncParam*> input_arguments;
  std::vector<MtMethod*> output_returns;

  std::vector<MtField*> private_signals;
  std::vector<MtField*> private_registers;

  //----------

  std::vector<MtMethod*> init_methods;
  std::vector<MtMethod*> tick_methods;
  std::vector<MtMethod*> tock_methods;
  std::vector<MtMethod*> task_methods;
  std::vector<MtMethod*> func_methods;
#endif
};

//------------------------------------------------------------------------------
