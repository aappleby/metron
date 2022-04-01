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

struct MtField {
  static MtField* construct(const MnNode& n, bool is_public) {
    return new MtField(n, is_public);
  }

  bool is_submod() const;
  bool is_param() const { return node.is_static() && node.is_const(); }

  std::string name() { return node.name4(); }

  std::string type_name() const { return node.type5(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  bool is_public = false;
private:
  MtField(const MnNode& n, bool is_public) : is_public(is_public), node(n) {
    assert(node.sym == sym_field_declaration || node.sym == sym_parameter_declaration);
  }
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

private:
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtParam {
  static MtParam* construct(const MnNode& n) { return new MtParam(n); }

  std::string name3() {
    return node.name4();
  }

private:
  MtParam(const MnNode& n) : node(n) {}
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtSourceFile* source_file, MnTemplateDecl node);
  MtModule(MtSourceFile* source_file, MnClassSpecifier node);

  MtEnum*   get_enum(const std::string& name);
  MtField*  get_field(const std::string& name);
  MtField*  get_input(const std::string& name);
  MtField*  get_output(const std::string& name);
  MtField*  get_register(const std::string& name);
  MtField*  get_submod(const std::string& name);
  MtMethod* get_method(const std::string& name);

  void load_pass1();
  void load_pass2();

  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;
  void dump_deltas() const;

  void collect_params();
  void collect_fields();
  void collect_methods();
  void collect_inputs();
  void collect_outputs();
  void collect_registers();
  void collect_submods();

  void build_port_map();
  void build_call_tree();

  void sanity_check();
  void check_dirty_ticks();
  void check_dirty_tocks();

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
  std::vector<MtParam*> localparams; // FIXME not actually doing anything with this yet?

  std::vector<MtEnum*> enums;

  std::vector<MtField*> all_fields;
  std::vector<MtField*> inputs;
  std::vector<MtField*> outputs;
  std::vector<MtField*> registers;
  std::vector<MtField*> submods;

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
