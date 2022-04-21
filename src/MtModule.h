#pragma once
#include <map>
#include <set>

#include "MtNode.h"
#include "MtTracer.h"
#include "Platform.h"

struct MtMethod;
struct MtModLibrary;
struct MtSourceFile;

typedef std::vector<uint8_t> blob;

//------------------------------------------------------------------------------

struct MtField {
  static MtField* construct(MtModule* mod, const MnNode& n, bool is_public) {
    return new MtField(mod, n, is_public);
  }

  bool is_register() const {
    return state == FIELD_REGISTER;
  }

  bool is_input_signal() const {
    return state == FIELD_INPUT;
  }

  bool is_output_signal() const {
    return state == FIELD_OUTPUT || state == FIELD_SIGNAL;
  }

  bool is_output_register() const {
    return is_public() && is_register();
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

private:
  MtModule* _mod;
  std::string _name;
  std::string _type;
  bool _public = false;

  MtField(MtModule* mod, const MnNode& n, bool is_public) : _mod(mod), _public(is_public), node(n) {
    assert(node.sym == sym_field_declaration ||
           node.sym == sym_parameter_declaration);
    _name = node.name4();
    _type = node.type5();
  }
};

//------------------------------------------------------------------------------

struct FieldRef {

  FieldRef(MtField* field, MtField* subfield) : field1(field), subfield(subfield) {}

  MtField* field1;
  MtField* subfield;
};

//------------------------------------------------------------------------------

struct MethodRef {
  MtModule* mod;
  MtMethod* method;
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
      debugbreak();
      return "";
    }
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
// MtMethodPort represents an implicit port that holds parameters or return
// values used by a method. Only tick & tock methods in a module have ports.

struct MtMethodPort {
  MtMethod* method;
  std::string name;

  bool is_input() const {
    return !is_return;
  }

  bool is_output() const {
    return is_return;
  }

  bool is_return = false;
};

//------------------------------------------------------------------------------

struct MtMethod {
  static MtMethod* construct(MnNode n, MtModule* _mod, MtModLibrary* _lib) {
    return new MtMethod(n, _mod, _lib);
  }

  const char* cname() const { return _name.c_str(); }
  std::string name() const { return _name; }

  int categorize();

  bool categorized() const {
    return in_init || in_tick || in_tock || is_func;
  }

  bool is_valid() const {
    // A method must be only 1 of init/tick/tock/func
    return (int(in_init) + int(in_tick) + int(in_tock) + int(is_func)) == 1;
  }

  bool is_root()   const { return callers.empty(); }

  bool is_leaf()   const {
    for (auto& ref : callees) {
      if (!ref.method->is_func) return false;
    }
    return true;
  }

  bool is_branch() const {
    for (auto& ref : callees) {
      if (!ref.method->is_func) return true;
    }
    return false;
  }

  bool is_writer() const { return !fields_written.empty(); }

  //----------------------------------------

  bool in_init = false;
  bool in_tick = false;
  bool in_tock = false;
  bool is_func = false;

  std::vector<std::string> params;
  std::vector<MnNode> param_nodes;

  bool is_constructor = false;
  bool is_tick = false;
  bool is_tock = false;
  bool is_task = false;
  bool is_public = false;
  bool has_return = false;

  std::vector<MethodRef> callers; // collect_methods
  std::vector<MethodRef> callees; // collect_methods

  std::vector<FieldRef> fields_read;
  std::vector<FieldRef> fields_written;

  std::vector<MtMethodPort*> input_ports;
  std::vector<MtMethodPort*> output_ports;

  StateMap method_state;
  MnNode node;

private:
  MtModule* mod = nullptr;
  MtModLibrary* lib = nullptr;

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
  MtModule();

  CHECK_RETURN Err init(MtSourceFile* source_file, MnTemplateDecl node);
  CHECK_RETURN Err init(MtSourceFile* source_file, MnClassSpecifier node);

  MtField*  get_enum(const std::string& name);
  MtField*  get_field(const std::string& name);
  MtField*  get_input_field(const std::string& name);
  MtParam*  get_input_param(const std::string& name);
  MtField*  get_output_signal(const std::string& name);
  MtField*  get_output_register(const std::string& name);
  MtMethod* get_output_return(const std::string& name);
  MtField*  get_component(const std::string& name);
  MtMethod* get_method(const std::string& name);

  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;

  CHECK_RETURN Err build_call_graph();

  CHECK_RETURN Err collect_modparams();
  CHECK_RETURN Err collect_fields_and_components();
  CHECK_RETURN Err collect_methods();

  CHECK_RETURN Err sort_fields();

  CHECK_RETURN Err trace();

  const char* cname() const { return mod_name.c_str(); }
  std::string name() const { return mod_name; }

  //--------------------

  std::string mod_name;
  MtSourceFile* source_file = nullptr;
  MtModLibrary* lib = nullptr;

  // Field state produced by evaluating all public methods in the module in
  // lexical order.
  StateMap mod_state;

  std::vector<MtModule*> parents;

  MnClassSpecifier mod_class;
  MnTemplateDecl mod_template;
  MnTemplateParamList mod_param_list;

  //----------
  // Populated by load_pass1, these collections are required by trace().

  std::vector<MtField*>  all_fields;
  std::vector<MtField*>  all_enums;
  std::vector<MtMethod*> all_methods;
  std::vector<MtField*>  all_components;

  MtMethod* constructor = nullptr;

  //----------
  // Populated by load_pass2 using the results from trace().

  std::vector<MtParam*> modparams;

  // FIXME not actually doing anything with this yet?
  std::vector<MtParam*> localparams;


  std::vector<MtField*> input_signals;
  std::vector<MtField*> output_signals;
  std::vector<MtField*> public_registers;

  std::vector<MtParam*> input_arguments;
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
};

//------------------------------------------------------------------------------
