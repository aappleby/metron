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

struct MtSubmod {

  static MtSubmod* construct(const MnNode& n) {
    return new MtSubmod(n);
  }

  std::string name() { return node.get_field(field_declarator).text(); }

  MtModule* mod;

private:
  MnNode node;
  MtSubmod(const MnNode& n) : node(n) {
    assert(node.sym == sym_field_declaration);
  }
};

//------------------------------------------------------------------------------

struct MtField {
  MtField(const MnNode& n, bool is_public) : is_public(is_public), node(n) {
    assert(node.sym == sym_field_declaration || node.sym == sym_parameter_declaration);
  }

  bool is_submod() const;
  bool is_param() const { return node.is_static() && node.is_const(); }

  std::string name() { return node.get_field(field_declarator).text(); }

  std::string type_name() const { return node.get_field(field_type).node_to_type(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  bool is_public = false;
//private:
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtEnum {
  MtEnum(const MnNode& n) : node(n) {}

  std::string name() {
    if (node.sym == sym_field_declaration) {
      auto enum_type = node.get_field(field_type);
      auto enum_name = enum_type.get_field(field_name);
      return enum_name.text();
    } else {
      node.dump_tree();
      debugbreak();
      return "";
    }
  }

  MnNode node;
};

//------------------------------------------------------------------------------

struct MtCall {
  static MtCall* construct(const MnNode& n) { return new MtCall(n); }

  MnNode node;
  MtSubmod* submod = nullptr;
  MtMethod* method = nullptr;
  std::vector<std::string>* args = nullptr;

private:

  MtCall(const MnNode& n) : node(n) { assert(node.sym == sym_call_expression); }
};

//------------------------------------------------------------------------------

struct MtParam {
  static MtParam* construct(const MnNode& n) { return new MtParam(n); }

  std::string name() {
    if (node.sym == sym_parameter_declaration) {
      return node.get_field(field_declarator).text();
    } else if (node.sym == sym_optional_parameter_declaration) {
      return node.get_field(field_declarator).text();
    } else if (node.sym == sym_field_declaration) {
      return node.get_field(field_declarator).text();
    } else {
      node.dump_tree();
      debugbreak();
      return "";
    }
  }

private:
  MtParam(const MnNode& n) : node(n) {}
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtSourceFile* source_file, MnTemplateDecl node);
  MtModule(MtSourceFile* source_file, MnClassSpecifier node);

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

  MtMethod* node_to_method(MnNode n);
  MtCall* node_to_call(MnNode n);

  //----------

  std::string mod_name;
  MtSourceFile* source_file = nullptr;

  bool load_error = false;
  bool dirty_check_done = false;
  bool dirty_check_fail = false;

  MnClassSpecifier mod_struct;
  MnTemplateDecl mod_template;
  MnTemplateParamList mod_param_list;

  std::vector<MtParam*> modparams;
  std::vector<MtParam*> localparams;

  std::vector<MtEnum*> enums;

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

  std::map<std::string, std::string> port_map;
};

//------------------------------------------------------------------------------
