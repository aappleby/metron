#pragma once
#include <map>
#include <set>

#include "MtNode.h"
#include "MtTracer.h"
#include "Platform.h"
#include "MtModLibrary.h"

struct MtMethod;
struct MtModLibrary;
struct MtSourceFile;

typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtField {
  static MtField* construct(MtModule* mod, const MnNode& n, bool is_public) {
    return new MtField(mod, n, is_public);
  }


  bool is_component() const;
  bool is_param() const { return node.is_static() && node.is_const(); }
  bool is_public() const { return _public; }

  const char* cname() const { return _name.c_str(); }
  std::string name() { return _name; }
  std::string type_name() const { return _type; }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  FieldState state = FIELD_NONE;
  MnNode node;

  void dump() {
    LOG_INDENT_SCOPE();
    if (is_component()) {
      LOG_R("Component %s : %s\n", cname(), _type.c_str());
    }
    else {
      LOG_G("Field %s : %s\n", cname(), _type.c_str());
    }
  }

  MtModule* _mod;
  std::string _name;
  std::string _type;
  bool _public = false;

  MtField(MtModule* mod, const MnNode& n, bool is_public) : _mod(mod), _public(is_public), node(n) {
    assert(node.sym == sym_field_declaration);
    _name = node.name4();
    _type = node.type5();
  }
};

//------------------------------------------------------------------------------

struct MtEnum {
  MtEnum(const MnNode& n) : node(n) {}

  std::string name() {
    assert (node.sym == sym_field_declaration);
    auto enum_type = node.get_field(field_type);
    auto enum_name = enum_type.get_field(field_name);
    return enum_name.text();
  }

private:
  MnNode node;
};

//------------------------------------------------------------------------------

struct MtParam {
  static MtParam* construct(const std::string& func_name, const MnNode& n) {
    return new MtParam(func_name, n);
  }

  const std::string& name()      const { return _name; }
  const char*        cname()     const { return _name.c_str(); }
  const std::string& type_name() const { return _type; }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  std::string func_name;
  MnNode node;

  void dump() {
    LOG_INDENT_SCOPE();
    LOG_Y("Modparam %s : %s\n", _name.c_str(), _type.c_str());
  }

 private:
  std::string _name;
  std::string _type;
  MtParam(const std::string& func_name, const MnNode& n)
      : func_name(func_name), node(n) {
    _name = node.name4();
    _type = node.type5();
  }
};

//------------------------------------------------------------------------------

struct MtMethod {
  static MtMethod* construct(MnNode n, MtModule* _mod, MtModLibrary* _lib) {
    return new MtMethod(n, _mod, _lib);
  }

  const char* cname() const { return _name.c_str(); }
  std::string name() const { return _name; }

  bool categorized() const {
    return in_init || in_tick || in_tock || in_func;
  }

  // A method must be only 1 of init/tick/tock/func
  bool is_valid() const {
    return (int(in_init) + int(in_tick) + int(in_tock) + int(in_func)) == 1;
  }

  bool is_root()   const { return callers.empty(); }

  bool is_leaf()   const {
    for (auto& m : callees) {
      if (!m->in_func) return false;
    }
    return true;
  }

  bool is_branch() const {
    for (auto& m : callees) {
      if (!m->in_func) return true;
    }
    return false;
  }

  bool has_return() const {
    return _type.text() != "void";
  }

  //----------------------------------------

  MtModLibrary* _lib = nullptr;
  MtModule* _mod = nullptr;
  MnNode _node;
  MnNode _type;
  std::string _name;

  bool in_init = false;
  bool in_tick = false;
  bool in_tock = false;
  bool in_func = false;
  bool is_public = false;

  std::vector<MnNode> param_nodes;

  std::vector<MtMethod*> callers;
  std::vector<MtMethod*> callees;

  MtMethod(MnNode n, MtModule* _mod, MtModLibrary* _lib)
      : _node(n), _mod(_mod), _lib(_lib) {
    _name = n.name4();
    _type = n.get_field(field_type);
  }

  void dump() {
    LOG_INDENT_SCOPE();
    LOG_B("Method %s\n", cname());

    for (auto p : param_nodes) {
      LOG_INDENT_SCOPE();
      LOG_R("Param %s\n", p.text().c_str());
    }
  }
};

//------------------------------------------------------------------------------

struct MtModule {
  MtModule(MtModLibrary* lib) : lib(lib) {}

  CHECK_RETURN Err init(MtSourceFile* source_file, MnTemplateDecl node);
  CHECK_RETURN Err init(MtSourceFile* source_file, MnClassSpecifier node);

  MtMethod* get_method(const std::string& name);
  MtField*  get_field(const std::string& name);

  /*
  MtField*  get_enum(const std::string& name);
  MtField*  get_input_field(const std::string& name);
  MtParam*  get_input_param(const std::string& name);
  MtField*  get_output_signal(const std::string& name);
  MtField*  get_output_register(const std::string& name);
  MtMethod* get_output_return(const std::string& name);
  MtField*  get_component(const std::string& name);
  */

  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;

  CHECK_RETURN Err build_call_graph();

  CHECK_RETURN Err collect_modparams();
  CHECK_RETURN Err collect_fields();
  CHECK_RETURN Err collect_methods();

  CHECK_RETURN Err sort_fields();

  CHECK_RETURN Err trace();

  const char* cname() const { return mod_name.c_str(); }
  std::string name() const { return mod_name; }

  //--------------------

  std::string mod_name;
  MtSourceFile* source_file = nullptr;
  MtModLibrary* lib;

  // Field state produced by evaluating all public methods in the module in
  // lexical order.
  StateMap mod_state;

  std::vector<MtModule*> parents;
  std::vector<MtModule*> children;

  MnClassSpecifier    mod_class;
  MnTemplateDecl      mod_template;
  MnTemplateParamList mod_param_list;

  //----------
  // Populated by load_pass1, these collections are required by trace().

  std::vector<MtParam*>  all_modparams;
  std::vector<MtField*>  all_fields;
  std::vector<MtField*>  all_enums;
  std::vector<MtMethod*> all_methods;

  MtMethod* constructor = nullptr;

  std::vector<MtField*>  inputs;
  std::vector<MtField*>  outputs;
  std::vector<MtField*>  signals;
  std::vector<MtField*>  registers;
  std::vector<MtField*>  components;

  // FIXME not actually doing anything with this yet?
  std::vector<MtParam*> localparams;

  std::vector<MtField*> input_signals;
  std::vector<MtField*> output_signals;
  std::vector<MtField*> public_registers;

  std::vector<MtParam*>  input_arguments;
  std::vector<MtMethod*> output_returns;

  std::vector<MtField*> private_signals;
  std::vector<MtField*> private_registers;

  //----------

  std::vector<MtMethod*> init_methods;
  std::vector<MtMethod*> tick_methods;
  std::vector<MtMethod*> tock_methods;
  std::vector<MtMethod*> task_methods;
  std::vector<MtMethod*> func_methods;

  //CHECK_RETURN Err build_port_map();
  //std::map<std::string, std::string> port_map;

  void dump_mod_tree() {
    LOG_Y(": %s\n", cname());
    LOG_INDENT_SCOPE();
    //for (auto c : children) c->dump_mod_tree();
    for (auto f : all_fields) {
      if (f->is_component()) {
        LOG_G("%s ", f->cname());
        lib->get_module(f->type_name())->dump_mod_tree();
      }
    }
  }

  void dump() {
    LOG_G("Module %s\n", cname());
    for (auto m : all_modparams) m->dump();
    for (auto m : all_methods)   m->dump();
    for (auto f : all_fields)    f->dump();
    LOG("\n");
  }
};

//------------------------------------------------------------------------------
