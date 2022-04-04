#pragma once
#include <map>
#include <set>

#include "MtNode.h"
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
  bool is_public() const { return _public; }

  std::string name() { return node.name4(); }
  std::string type_name() const { return node.type5(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

private:
  MtField(const MnNode& n, bool is_public) : _public(is_public), node(n) {
    assert(node.sym == sym_field_declaration || node.sym == sym_parameter_declaration);
    _name = node.name4();
  }

  std::string _name;

  MnNode node;
  bool _public = false;
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

struct MtMethod {

  static MtMethod* construct(MnNode n, MtModule* _mod, MtModLibrary* _lib) {
    return new MtMethod(n, _mod, _lib);
  }

  std::string name() const { return node.name4(); }

  MnNode node;
  MtModule* mod = nullptr;
  MtModLibrary* lib = nullptr;

  std::vector<std::string> params;
  //MtDelta delta = nullptr;

  bool is_init = false;
  bool is_tick = false;
  bool is_tock = false;
  bool is_task = false;
  bool is_func = false;
  bool is_root = false;
  bool is_public = false;
  bool is_const = false;

  std::vector<MtMethod*> callers;

  int get_rank() const {
    return 0;
  }

private:

  std::string _name;

  MtMethod(MnNode n, MtModule* _mod, MtModLibrary* _lib)
    : node(n), mod(_mod), lib(_lib) {
    assert(mod);
    assert(lib);
    _name = node.name4();
  }
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

  bool load_pass1();
  bool load_pass2();

  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;
  //void dump_deltas() const;

  bool collect_params();
  bool collect_fields();
  bool collect_methods();
  bool collect_inputs();
  bool collect_outputs();
  bool collect_registers();
  bool collect_submods();

  bool build_port_map();

  bool trace();

  bool sanity_check();

#if 0
  void check_dirty_ticks();
  void check_dirty_tocks();
#endif

  std::string name() const {
    return mod_name;
  }

  int get_rank() const {
    if (parents.empty()) return 0;
    int min_rank = 1000000;
    for (auto p : parents) {
      int r = p->get_rank();
      if (r < min_rank) min_rank = r;
    }
    return min_rank + 1;
  }

  //----------

  std::string mod_name;
  MtSourceFile* source_file = nullptr;

  bool load_error = false;
  bool dirty_check_done = false;
  bool dirty_check_fail = false;

  int mark = 0;

  std::vector<MtModule*> parents;

  MnClassSpecifier mod_class;
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
