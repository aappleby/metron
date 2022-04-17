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
  static MtField* construct(const MnNode& n, bool is_public) {
    return new MtField(n, is_public);
  }

  bool is_register() const {
    return state == FIELD_REGISTER;
  }

  bool is_input_signal() const {
    return is_public() && state == FIELD_INPUT;
  }

  bool is_output_signal() const {
    return is_public() && (state == FIELD_OUTPUT || state == FIELD_SIGNAL);
  }

  bool is_output_register() const {
    return is_public() && is_register();
  }

  /*
  bool is_signal() const { debugbreak(); return false; }
  bool is_register() const { debugbreak(); return false; }
  bool is_input_signal() const { debugbreak(); return false; }
  bool is_output_signal() const { debugbreak(); return false; }
  bool is_output_register() const { debugbreak(); return false; }
  */

  bool is_component() const;
  bool is_param() const { return node.is_static() && node.is_const(); }
  bool is_public() const { return _public; }

  std::string name() { return node.name4(); }
  std::string type_name() const { return node.type5(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  bool written_in_tick = false;
  bool written_in_tock = false;

  FieldState state = FIELD_NONE;
  MnNode node;
  std::string _name;
  bool _public = false;

 private:
  MtField(const MnNode& n, bool is_public) : _public(is_public), node(n) {
    assert(node.sym == sym_field_declaration ||
           node.sym == sym_parameter_declaration);
    _name = node.name4();
  }
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

  std::string name() { return node.name4(); }
  std::string type_name() const { return node.type5(); }

  MnNode get_type_node() const { return node.get_field(field_type); }
  MnNode get_decl_node() const { return node.get_field(field_declarator); }

  std::string func_name;
  MnNode node;

 private:
  MtParam(const std::string& func_name, const MnNode& n)
      : func_name(func_name), node(n) {}
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
  std::vector<MnNode> param_nodes;
  // MtDelta delta = nullptr;

  bool is_constructor = false;
  bool is_init = false;
  bool is_tick = false;
  bool is_tock = false;
  bool is_task = false;
  bool is_func = false;
  bool is_root = false;
  bool is_public = false;
  bool is_private = false;
  bool is_const = false;
  bool has_return = false;
  bool is_triggered = false; // true if this is a tick and a tock has called it during a trace

  std::set<MtMethod*> callers;
  std::set<MtMethod*> callees;

  int get_rank() const { return 0; }

  StateMap method_state;

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
  MtModule();

  CHECK_RETURN Err init(MtSourceFile* source_file, MnTemplateDecl node);
  CHECK_RETURN Err init(MtSourceFile* source_file, MnClassSpecifier node);

  MtField* get_enum(const std::string& name);
  MtField* get_field(const std::string& name);
  MtField* get_input_field(const std::string& name);
  MtParam* get_input_param(const std::string& name);
  MtField* get_output_signal(const std::string& name);
  MtField* get_output_register(const std::string& name);
  MtMethod* get_output_return(const std::string& name);
  MtField* get_component(const std::string& name);
  MtMethod* get_method(const std::string& name);

  CHECK_RETURN Err load_pass1();
  CHECK_RETURN Err load_pass2();

  void dump_method_list(const std::vector<MtMethod*>& methods) const;
  void dump_banner() const;
  // void dump_deltas() const;

  CHECK_RETURN Err collect_modparams();
  CHECK_RETURN Err collect_fields_and_components();
  CHECK_RETURN Err collect_methods();
  CHECK_RETURN Err categorize_fields();

  // CHECK_RETURN Err collect_registers();

  CHECK_RETURN Err build_port_map();

  CHECK_RETURN Err trace();

  CHECK_RETURN Err sanity_check();

#if 0
  void check_dirty_ticks();
  void check_dirty_tocks();
#endif

  std::string name() const { return mod_name; }

  //----------

  std::string mod_name;
  MtSourceFile* source_file = nullptr;

  bool load_error = false;
  bool dirty_check_done = false;
  bool dirty_check_fail = false;

  int mark = 0;

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

  std::map<std::string, std::string> port_map;
};

//------------------------------------------------------------------------------
