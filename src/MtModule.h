#pragma once
#include <map>
#include <set>

#include "MtMethod.h"
#include "Platform.h"

struct MtMethod;
struct MtModLibrary;
struct MtSourceFile;

typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtSourceFile* source_file, MtTemplateDecl node);
  MtModule(MtSourceFile* source_file, MtClassSpecifier node);

  bool has_field(const std::string& name);
  bool has_input(const std::string& name);
  bool has_output(const std::string& name);
  bool has_register(const std::string& name);
  bool has_submod(const std::string& name);
  bool has_enum(const std::string& name);

  MtField* get_field(const std::string& name);
  MtField* get_output(const std::string& name);
  MtSubmod* get_submod(const std::string& name);
  MtMethod* get_method(const std::string& name);

  void load_pass1();
  void load_pass2();

  void dump_method_list(std::vector<MtMethod>& methods);
  void dump_method_list2(const std::vector<MtMethod*>& methods);
  void dump_call_list(std::vector<MtCall>& calls);
  void dump_banner();
  void dump_deltas();

  void collect_params();
  void collect_fields();
  void collect_methods();
  void collect_inputs();
  void collect_outputs();
  void collect_registers();
  void collect_submods();
  void build_port_map();
  void build_call_tree();

  void check_dirty_ticks();
  void check_dirty_tocks();

  void sanity_check();

  MtMethod* node_to_method(MtNode n);
  MtCall node_to_call(MtNode n);

  //----------

  MtSourceFile* source_file = nullptr;

  std::string mod_name;

  bool load_error = false;

  bool dirty_check_done = false;
  bool dirty_check_fail = false;

  MtClassSpecifier mod_struct;
  MtTemplateDecl mod_template;
  MtTemplateParamList mod_param_list;

  std::vector<MtParam>* modparams = nullptr;
  std::vector<MtParam>* localparams = nullptr;

  std::vector<MtEnum>* enums = nullptr;

  // inputs + outputs + regs + submods
  std::vector<MtField*>  all_fields;
  std::vector<MtField*>  inputs;
  std::vector<MtField*>  outputs;
  std::vector<MtField*>  registers;

  std::vector<MtSubmod*> submods;

  std::vector<MtMethod*> all_methods;
  std::vector<MtMethod*> getters;
  std::vector<MtMethod*> init_methods;
  std::vector<MtMethod*> tick_methods;
  std::vector<MtMethod*> tock_methods;
  std::vector<MtMethod*> task_methods;
  std::vector<MtMethod*> func_methods;

  std::map<std::string, std::string>* port_map = nullptr;
};

//------------------------------------------------------------------------------
