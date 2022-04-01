#include "MtModule.h"

#include "MtCursor.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "Platform.h"
#include "metron_tools.h"
#include "tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

void log_field_state(FieldState s) {
  switch (s) {
    case CLEAN:
      LOG_G("clean");
      break;
    case MAYBE:
      LOG_Y("maybe");
      break;
    case DIRTY:
      LOG_R("dirty");
      break;
    case ERROR:
      LOG_M("error");
      break;
  }
}

// FIXME
void log_error(MnNode n, const char *fmt, ...) {
  printf("\n########################################\n");

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("@%04d: ", ts_node_start_point(n.node).row + 1);

  /*
{
  auto start = &source[n.start_byte()];

  auto a = start;
  auto b = start;
  while (a > source     && *a != '\n' && *a != '\r') a--;
  while (b < source_end && *b != '\n' && *b != '\r') b++;

  if (*a == '\n' || *a == '\r') a++;

  while (a != b) {
    putc(*a++, stdout);
  }
}

printf("\n");
*/

  // n.error();
  n.dump_tree();

  // printf("halting...\n");
  printf("########################################\n");
}

//------------------------------------------------------------------------------

bool MtField::is_submod() const {
  return (node.source->lib->has_mod(type_name()));
}

//------------------------------------------------------------------------------

MtModule::MtModule(MtSourceFile *source_file, MnTemplateDecl node)
    : source_file(source_file) {
  mod_template = node;
  mod_param_list = MnTemplateParamList(mod_template.child(1));
  mod_struct = MnClassSpecifier(mod_template.child(2));
  mod_name = mod_struct.get_field(field_name).text();
}

//------------------------------------------------------------------------------

MtModule::MtModule(MtSourceFile *source_file, MnClassSpecifier node)
    : source_file(source_file) {
  mod_template = MnNode::null;
  mod_param_list = MnNode::null;
  mod_struct = node;
  mod_name = mod_struct.get_field(field_name).text();
}

//------------------------------------------------------------------------------

MtMethod *MtModule::get_method(const std::string &name) {
  for (auto n : getters)
    if (n->name == name) return n;
  for (auto n : init_methods)
    if (n->name == name) return n;
  for (auto n : tick_methods)
    if (n->name == name) return n;
  for (auto n : tock_methods)
    if (n->name == name) return n;
  for (auto n : task_methods)
    if (n->name == name) return n;
  for (auto n : func_methods)
    if (n->name == name) return n;
  return nullptr;
}

//----------------------------------------

bool MtModule::has_enum(const std::string &name) {
  for (auto n : enums) {
    if (n->name() == name) return true;
  }
  return false;
}

//----------------------------------------

bool MtModule::has_field(const std::string &name) {
  return get_field(name) != nullptr;
}

MtField *MtModule::get_field(const std::string &name) {
  for (auto f : all_fields) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

//----------------------------------------

bool MtModule::has_input(const std::string &name) {
  for (auto f : inputs)
    if (f->name() == name) return true;
  return false;
}

//----------------------------------------

bool MtModule::has_output(const std::string &name) {
  return get_output(name) != nullptr;
}

MtField *MtModule::get_output(const std::string &name) {
  for (auto n : outputs)
    if (n->name() == name) return n;
  return nullptr;
}
//----------------------------------------

bool MtModule::has_register(const std::string &name) {
  for (auto f : registers)
    if (f->name() == name) return true;
  return false;
}

//----------------------------------------

bool MtModule::has_submod(const std::string &name) {
  return get_submod(name) != nullptr;
}

MtField* MtModule::get_submod(const std::string &name) {
  for (auto n : submods) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModule::dump_method_list(std::vector<MtMethod> &methods) {
  for (auto &n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n.name.c_str());

    if (n.params.size()) {
      int param_count = int(n.params.size());
      int param_index = 0;

      for (auto &param : n.params) {
        LOG_R("%s", param.c_str());
        if (param_index++ != param_count - 1) LOG_C(", ");
      }
    }
    LOG_R(")\n");
  }
}

void MtModule::dump_method_list2(const std::vector<MtMethod *> &methods) {
  for (auto n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n->name.c_str());

    if (n->params.size()) {
      int param_count = int(n->params.size());
      int param_index = 0;

      for (auto &param : n->params) {
        LOG_R("%s", param.c_str());
        if (param_index++ != param_count - 1) LOG_C(", ");
      }
    }
    LOG_R(")\n");
  }
}

//------------------------------------------------------------------------------

void MtModule::dump_banner() {
  LOG_Y("//----------------------------------------\n");
  if (mod_struct.is_null()) {
    LOG_Y("// Package %s\n", source_file->full_path.c_str());
    LOG_Y("\n");
    return;
  }

  LOG_Y("// Module %s\n", mod_name.c_str());
  LOG_Y("\n");

  //----------

  LOG_B("Modparams:\n")
  for (auto param : modparams) LOG_G("  %s\n", param->name3().c_str());
  LOG_B("Localparams:\n");
  for (auto param : localparams) LOG_G("  %s\n", param->name3().c_str());
  LOG_B("Enums:\n");
  for (auto n : enums) LOG_G("  %s\n", n->name().c_str());
  LOG_B("Inputs:\n");
  for (auto n : inputs)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Outputs:\n");
  for (auto n : outputs)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  if (!getters.empty()) {
    LOG_B("Getters:\n");
    dump_method_list2(getters);
  }
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Submods:\n");
  for (auto submod : submods) {
    auto submod_mod = source_file->lib->get_mod(submod->type_name());
    LOG_G("  %s:%s\n", submod->name().c_str(), submod_mod->mod_name.c_str());
  }

  //----------

  LOG_B("Init methods:\n");
  dump_method_list2(init_methods);
  LOG_B("Tick methods:\n");
  dump_method_list2(tick_methods);
  LOG_B("Tock methods:\n");
  dump_method_list2(tock_methods);
  LOG_B("Tasks:\n");
  dump_method_list2(task_methods);
  LOG_B("Functions:\n");
  dump_method_list2(func_methods);

  //----------

  LOG_B("Port map:\n");
  for (auto &kv : port_map)
    LOG_G("  %s = %s\n", kv.first.c_str(), kv.second.c_str());

  LOG_B("\n");
}

//------------------------------------------------------------------------------

void MtModule::dump_deltas() {
  if (mod_struct.is_null()) return;

  LOG_G("%s\n", mod_name.c_str());
  {
    LOG_INDENT_SCOPE();

    for (auto tick : tick_methods) {
      LOG_G("%s error %d\n", tick->name.c_str(), tick->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tick->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tick->delta->state_new.contains(s.first)) {
            auto s2 = tick->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }

    for (auto tock : tock_methods) {
      LOG_G("%s error %d\n", tock->name.c_str(), tock->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tock->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tock->delta->state_new.contains(s.first)) {
            auto s2 = tock->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }
  }
  LOG_G("\n");
}

//------------------------------------------------------------------------------
// All modules are now in the library, we can resolve references to other
// modules when we're collecting fields.

void MtModule::load_pass1() {
  if (mod_struct.is_null()) return;

  collect_params();
  collect_fields();
  collect_methods();
  collect_inputs();
  collect_outputs();
  collect_registers();
  collect_submods();

  // enums = new std::vector<MtEnum>();

  auto mod_body = mod_struct.get_field(field_body).check_null();
  for (auto n : mod_body) {
    if (n.sym != sym_field_declaration) continue;

    // MtField f(n);

    // enum class
    if (n.get_field(::field_type).sym == sym_enum_specifier) {
      auto new_enum = new MtEnum(n);
      enums.push_back(new_enum);
      continue;
    }

    // Everything not an enum shoul have 1 or more declarator fields that
    // contain the field name(s).

    // int field_count = 0;

    /*
    for (auto c : n) {
      if (c.field != field_declarator) continue;
      field_count++;
      auto name =
          c.sym == sym_array_declarator ? c.get_field(field_declarator) : c;

      MtField f(n);

      auto type_name = n.node_to_type();
      if (source_file->lib->has_mod(type_name)) {
        MtSubmod submod(n);
        submod.mod = source_file->lib->get_mod(type_name);
        submod.name = n.get_field(field_declarator).text();
        submods.push_back(submod);
      }
    }
    */
  }

  sanity_check();
}

//------------------------------------------------------------------------------

void MtModule::collect_params() {
  assert(modparams.empty());

  if (mod_template) {
    auto params = mod_template.get_field(field_parameters);

    for (int i = 0; i < params.named_child_count(); i++) {
      auto child = params.named_child(i);
      if (child.sym == sym_parameter_declaration ||
          child.sym == sym_optional_parameter_declaration) {
        modparams.push_back(MtParam::construct(child));
      } else {
        debugbreak();
      }
    }
  }

  // localparam = static const int
  assert(localparams.empty());
  auto mod_body = mod_struct.get_field(field_body).check_null();
  for (auto n : mod_body) {
    if (n.sym != sym_field_declaration) continue;

    if (n.is_static() && n.is_const()) {
      localparams.push_back(MtParam::construct(n));
    }
  }
}

//------------------------------------------------------------------------------

void MtModule::collect_fields() {
  assert(all_fields.empty());

  auto mod_body = mod_struct.get_field(field_body).check_null();
  bool in_public = false;

  for (auto n : mod_body) {
    if (n.sym == sym_access_specifier) {
      if (n.child(0).text() == "public") {
        in_public = true;
      } else if (n.child(0).text() == "protected") {
        in_public = false;
      } else if (n.child(0).text() == "private") {
        in_public = false;
      } else {
        n.dump_tree();
        debugbreak();
      }
    }

    if (n.sym != sym_field_declaration) {
      continue;
    }

    // FIXME why are we excluding enums here? because they're not a "field"?
    if (n.get_field(field_type).sym == sym_enum_specifier) continue;

    auto new_field = MtField::construct(n, in_public);

    all_fields.push_back(new_field);
  }
}

//------------------------------------------------------------------------------

void MtModule::collect_methods() {
  assert(all_methods.empty());

  bool in_public = false;
  auto mod_body = mod_struct.get_field(field_body).check_null();
  for (auto n : mod_body) {
    if (n.sym == sym_access_specifier) {
      if (n.child(0).text() == "public") {
        in_public = true;
      } else if (n.child(0).text() == "protected") {
        in_public = false;
      } else if (n.child(0).text() == "private") {
        in_public = false;
      } else {
        n.dump_tree();
        debugbreak();
      }
    }

    if (n.sym != sym_function_definition) continue;
    auto func_decl = n.get_field(field_declarator);
    auto func_name = func_decl.get_field(field_declarator).name4();
    auto func_args = n.get_field(field_declarator).get_field(field_parameters);
    auto func_type = n.get_field(field_type);

    bool is_task = func_type.match("void");
    bool is_init = is_task && func_name.starts_with("init");
    bool is_tick = is_task && func_name.starts_with("tick");
    bool is_tock = is_task && func_name.starts_with("tock");

    MtMethod *new_method2 = MtMethod::construct(n, this, source_file->lib);
    new_method2->name = n.get_field(field_declarator).get_field(field_declarator).text();
    auto method_params = n.get_field(field_declarator).get_field(field_parameters);
    for (int i = 0; i < method_params.named_child_count(); i++) {
      auto param = method_params.named_child(i);
      assert (param.sym == sym_parameter_declaration);
      auto param_name = param.get_field(field_declarator).text();
      new_method2->params.push_back(param_name);
    }

    all_methods.push_back(new_method2);

    if (is_init) {
      init_methods.push_back(new_method2);
    } else if (is_tick) {
      tick_methods.push_back(new_method2);
    } else if (is_tock) {
      tock_methods.push_back(new_method2);
    } else if (is_task) {
      task_methods.push_back(new_method2);
    } else if (in_public) {
      getters.push_back(new_method2);
    } else {
      func_methods.push_back(new_method2);
    }
  }

  for (auto n : tick_methods) n->is_tick = true;
  for (auto n : tock_methods) n->is_tock = true;
}

//------------------------------------------------------------------------------

void MtModule::build_call_tree() {}

//------------------------------------------------------------------------------
// Collect all inputs to all tick and tock methods and merge them into a list of
// input ports. Input ports can be declared in multiple tick/tock methods, but
// we don't want duplicates in the Verilog port list.

/*
void MtModule::build_port_map() {

  port_map = new std::map<std::string, std::string>();

  mod_struct.visit_tree([&](MtNode child) {
    if (child.sym != sym_call_expression) return;
    if (child.get_field(field_function).sym != sym_field_expression) return;

    auto call = node_to_call(child);

    if (!call.method) {
      child.dump_tree();
    }
    assert(call.method);


    for (auto i = 0; i < call.args->size(); i++) {
      auto key = call.submod->name() + "." + call.method->params->at(i);
      auto val = call.args->at(i);
      auto it = port_map->find(key);
      if (it != port_map->end()) {
        assert((*it).second == val);
      } else {
        port_map->insert({key, val});
      }
    }
  });
}
*/

void MtModule::collect_inputs() {
  assert(inputs.empty());

  std::set<std::string> dedup;

  bool in_public = false;
  auto mod_body = mod_struct.get_field(field_body).check_null();
  for (auto n : mod_body) {
    if (n.sym == sym_access_specifier) {
      if (n.child(0).text() == "public") {
        in_public = true;
      } else if (n.child(0).text() == "protected") {
        in_public = false;
      } else if (n.child(0).text() == "private") {
        in_public = false;
      } else {
        n.dump_tree();
        debugbreak();
      }
      continue;
    }

    if (n.sym == sym_function_definition) {
      auto params = n.get_field(field_declarator).get_field(field_parameters);

      for (auto param : params) {
        if (param.sym != sym_parameter_declaration) continue;
        MtField *new_input = MtField::construct(param, true);
        if (!dedup.contains(new_input->name())) {
          inputs.push_back(new_input);
          dedup.insert(new_input->name());
        } else {
          delete new_input;
        }
      }
      continue;
    }
  }
}

//------------------------------------------------------------------------------
// All fields written to in a tock method are outputs.

void MtModule::collect_outputs() {
  assert(outputs.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {
    if (f->is_public && !f->is_submod() && !f->is_param()) {
      outputs.push_back(f);
    }
  }

  /*
  for (auto n : *tock_methods) {
    n.visit_tree([&](MtNode child) {
      if (child.sym != sym_assignment_expression) return;

      auto lhs = child.get_field(field_left);
      assert(lhs.sym == sym_identifier);
      auto lhs_name = lhs.text();

      if (dedup.contains(lhs_name)) return;
      dedup.insert(lhs_name);

      auto field = get_field(lhs_name);
      if (field) outputs->push_back(*field);
    });
  }
  */
}

//------------------------------------------------------------------------------
// All fields written to in a tick method are registers.

void MtModule::collect_registers() {
  assert(registers.empty());

  std::set<std::string> dedup;

  for (auto n : tick_methods) {
    n->node.visit_tree([&](MnNode child) {
      if (child.sym != sym_assignment_expression) return;

      auto lhs = child.get_field(field_left);
      std::string lhs_name;

      if (lhs.sym == sym_identifier) {
        lhs_name = lhs.text();
      } else if (lhs.sym == sym_subscript_expression) {
        lhs_name = lhs.get_field(field_argument).text();
      } else {
        lhs.dump_tree();
        debugbreak();
      }

      if (dedup.contains(lhs_name)) return;
      dedup.insert(lhs_name);

      auto field = get_field(lhs_name);
      if (field && !field->is_public) registers.push_back(field);
    });
  }
}

//------------------------------------------------------------------------------

void MtModule::collect_submods() {
  assert(submods.empty());

  for (auto f : all_fields) {
    if (source_file->lib->has_mod(f->type_name())) {
      submods.push_back(f);
    } else {
      if (f->type_name() != "logic") {
        LOG_R("Could not find module for submod %s\n", f->type_name().c_str());
      }
    }
  }
}

//------------------------------------------------------------------------------
// All modules have populated their fields, match up tick/tock calls with their
// corresponding methods.

void MtModule::load_pass2() {
  if (mod_struct.is_null()) return;

  build_port_map();
  sanity_check();
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

void MtModule::build_port_map() {
  assert(port_map.empty());

  mod_struct.visit_tree([&](MnNode child) {
    if (child.sym != sym_call_expression) return;
    if (child.get_field(field_function).sym != sym_field_expression) return;

    auto node_call = child;
    auto node_func = node_call.get_field(field_function);
    auto node_args = node_call.get_field(field_arguments);

    MtField*  call_member = nullptr;
    MtModule* call_submod = nullptr;
    MtMethod* call_method = nullptr;

    if (node_func.sym == sym_field_expression) {
      auto node_member = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);
      if (node_method.text() == "as_signed") {
        return;
      }
    }

    if (node_func.sym == sym_field_expression) {
      auto node_this = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);

      if (node_method.text() == "as_signed") {
      } else {
        call_member = get_submod(node_this.text());
        call_submod = source_file->lib->get_mod(call_member->type_name());
        call_method = call_submod->get_method(node_method.text());
      }
    }

    auto arg_count = node_args.named_child_count();

    for (auto i = 0; i < arg_count; i++) {
      //auto key = call_member->name() + "." + call_method->params[i];
      auto key = call_member->name() + "_" + call_method->name + "_" + call_method->params[i];
      
      //std::string val = node_args.named_child(i).text();

      std::string val;
      MtCursor cursor(source_file->lib, source_file, &val);
      auto arg_node = node_args.named_child(i);
      cursor.cursor = arg_node.start();
      cursor.emit_dispatch(arg_node);


      auto it = port_map.find(key);
      if (it != port_map.end()) {
        if ((*it).second != val) {
          LOG_R("Error, got multiple different values for %s: '%s' and '%s'\n",
                key.c_str(), (*it).second.c_str(), val.c_str());
          debugbreak();
        }
      } else {
        port_map.insert({key, val});
      }
    }
  });
}

//------------------------------------------------------------------------------

void MtModule::sanity_check() {
  // Inputs, outputs, registers, and submods must not overlap.

  std::set<std::string> field_names;

  for (auto n : inputs) {
    assert(!field_names.contains(n->name()));
    field_names.insert(n->name());
  }

  for (auto n : outputs) {
    assert(!field_names.contains(n->name()));
    field_names.insert(n->name());
  }

  for (auto n : registers) {
    auto name = n->name();
    assert(!field_names.contains(name));
    field_names.insert(n->name());
  }

  for (auto n : submods) {
    assert(!field_names.contains(n->name()));
    field_names.insert(n->name());
  }
}

//------------------------------------------------------------------------------

void MtModule::check_dirty_ticks() {
  for (auto tick : tick_methods) {
    tick->update_delta();
    dirty_check_fail |= tick->delta->error;
  }
}

//------------------------------------------------------------------------------

void MtModule::check_dirty_tocks() {
  for (auto tock : tock_methods) {
    tock->update_delta();
    dirty_check_fail |= tock->delta->error;
  }
}

//------------------------------------------------------------------------------
