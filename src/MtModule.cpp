#include "MtModule.h"

#include "MtCursor.h"
#include "MtModLibrary.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "metron_tools.h"
#include "tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

void log_field_state(FieldState s) {
  /*
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
  */
}

// FIXME
void log_error(MnNode n, const char *fmt, ...) {
  printf("\n########################################\n");

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  printf("@%04d: ", ts_node_start_point(n.node).row + 1);

  // n.error();
  n.dump_tree();

  // printf("halting...\n");
  printf("########################################\n");
}

//------------------------------------------------------------------------------

bool MtField::is_submod() const {
  return (node.source->lib->has_module(type_name()));
}

//------------------------------------------------------------------------------

MtModule::MtModule(MtSourceFile *source_file, MnTemplateDecl node)
    : source_file(source_file) {
  mod_template = node;
  mod_param_list = MnTemplateParamList(mod_template.child(1));
  mod_class = MnClassSpecifier(mod_template.child(2));
  mod_name = mod_class.get_field(field_name).text();
}

//------------------------------------------------------------------------------

MtModule::MtModule(MtSourceFile *source_file, MnClassSpecifier node)
    : source_file(source_file) {
  mod_template = MnNode::null;
  mod_param_list = MnNode::null;
  mod_class = node;
  mod_name = mod_class.get_field(field_name).text();
}

//------------------------------------------------------------------------------

MtMethod *MtModule::get_method(const std::string &name) {
  for (auto n : all_methods)
    if (n->name() == name) return n;
  return nullptr;
}

MtEnum *MtModule::get_enum(const std::string &name) {
  for (auto n : enums) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

MtField *MtModule::get_field(const std::string &name) {
  for (auto f : all_fields) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

MtField *MtModule::get_input(const std::string &name) {
  for (auto f : inputs)
    if (f->name() == name) return f;
  return nullptr;
}

MtField *MtModule::get_output(const std::string &name) {
  for (auto n : outputs)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_register(const std::string &name) {
  for (auto f : registers)
    if (f->name() == name) return f;
  return nullptr;
}

MtField *MtModule::get_submod(const std::string &name) {
  for (auto n : submods) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModule::dump_method_list(const std::vector<MtMethod *> &methods) const {
  for (auto n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n->name().c_str());

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

void MtModule::dump_banner() const {
  LOG_Y("//----------------------------------------\n");
  if (mod_class.is_null()) {
    LOG_Y("// Package %s\n", source_file->full_path.c_str());
    LOG_Y("\n");
    return;
  }

  LOG_Y("// Module %s, rank %d", mod_name.c_str(), get_rank());
  if (parents.empty()) LOG_Y(" ########## TOP ##########");
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
    dump_method_list(getters);
  }
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Submods:\n");
  for (auto submod : submods) {
    auto submod_mod = source_file->lib->get_module(submod->type_name());
    LOG_G("  %s:%s\n", submod->name().c_str(), submod_mod->mod_name.c_str());
  }

  //----------

  LOG_B("Init methods:\n");
  dump_method_list(init_methods);
  LOG_B("Tick methods:\n");
  dump_method_list(tick_methods);
  LOG_B("Tock methods:\n");
  dump_method_list(tock_methods);
  LOG_B("Tasks:\n");
  dump_method_list(task_methods);
  LOG_B("Functions:\n");
  dump_method_list(func_methods);

  //----------

  LOG_B("Port map:\n");
  for (auto &kv : port_map)
    LOG_G("  %s = %s\n", kv.first.c_str(), kv.second.c_str());

  LOG_B("\n");
}

//------------------------------------------------------------------------------

#if 0
void MtModule::dump_deltas() const {
  if (mod_struct.is_null()) return;

  LOG_G("%s\n", mod_name.c_str());
  {
    LOG_INDENT_SCOPE();

    for (auto tick : tick_methods) {
      LOG_G("%s error %d\n", tick->name().c_str(), tick->delta->error);
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
      LOG_G("%s error %d\n", tock->name().c_str(), tock->delta->error);
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
#endif

//------------------------------------------------------------------------------
// All modules are now in the library, we can resolve references to other
// modules when we're collecting fields.

bool MtModule::load_pass1() {
  bool error = false;

  if (mod_class.is_null()) {
    LOG_R("No class found for module\n");
    error = true;
    return error;
  }

  collect_params();
  collect_fields();
  collect_methods();
  collect_inputs();
  collect_outputs();
  collect_registers();
  collect_submods();

  // enums = new std::vector<MtEnum>();

  auto mod_body = mod_class.get_field(field_body).check_null();
  for (const auto& n : mod_body) {
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

  return sanity_check();
}

//------------------------------------------------------------------------------

bool MtModule::collect_params() {
  bool error = false;

  // modparam = struct template parameter
  if (mod_template) {
    auto params = mod_template.get_field(field_parameters);

    for (int i = 0; i < params.named_child_count(); i++) {
      auto child = params.named_child(i);
      if (child.sym == sym_parameter_declaration ||
          child.sym == sym_optional_parameter_declaration) {
        modparams.push_back(MtParam::construct(child));
      } else {
        LOG_R("Unknown template parameter type %s\n", child.ts_node_type());
        error = true;
      }
    }
  }

  // localparam = static const int
  assert(localparams.empty());
  auto mod_body = mod_class.get_field(field_body).check_null();
  for (const auto& n : mod_body) {
    if (n.sym != sym_field_declaration) continue;

    if (n.is_static() && n.is_const()) {
      localparams.push_back(MtParam::construct(n));
    }
  }

  return error;
}

//------------------------------------------------------------------------------

bool MtModule::collect_fields() {
  bool error = false;

  assert(all_fields.empty());

  auto mod_body = mod_class.get_field(field_body).check_null();
  bool in_public = false;

  for (const auto& n : mod_body) {
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

  return error;
}

//------------------------------------------------------------------------------

bool MtModule::collect_methods() {
  bool error = false;

  assert(all_methods.empty());

  bool in_public = false;
  auto mod_body = mod_class.get_field(field_body).check_null();
  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      if (n.child(0).text() == "public") {
        in_public = true;
      } else if (n.child(0).text() == "protected") {
        in_public = false;
      } else if (n.child(0).text() == "private") {
        in_public = false;
      } else {
        LOG_R("Unknown access specifier %s\n", n.child(0).text().c_str());
        n.dump_source_lines();
        error = true;
      }
    }

    if (n.sym != sym_function_definition) continue;

    MtMethod *m = MtMethod::construct(n, this, source_file->lib);

    auto func_type =
        n.get_field(field_type);  // Can be null for constructor/destructor
    auto func_decl = n.get_field(field_declarator);
    auto func_body = n.get_field(field_body);

    auto func_name = func_decl.get_field(field_declarator).name4();
    auto func_args = func_decl.get_field(field_parameters);

    m->is_public = in_public;
    m->is_task = func_type && func_type.text() == "void";
    m->is_func = func_type && func_type.text() != "void";

    m->is_init = func_type.is_null();
    m->is_tick = func_name.starts_with("tick");
    m->is_tock = func_name.starts_with("tock");

    m->is_const = false;
    for (const auto& n : func_decl) {
      if (n.sym == sym_type_qualifier && n.text() == "const") {
        m->is_const = true;
        break;
      }
    }

    for (int i = 0; i < func_args.named_child_count(); i++) {
      auto param = func_args.named_child(i);
      assert(param.sym == sym_parameter_declaration);
      auto param_name = param.get_field(field_declarator).text();
      m->params.push_back(param_name);
    }

    all_methods.push_back(m);

    if (m->is_init) {
      if (m->is_const || !m->is_public) {
        LOG_R("CONST INIT BAD / PRIVATE INIT BAD\n");
        error = true;
      }
      init_methods.push_back(m);
    } else if (m->is_tick) {
      if (m->is_public) {
        // FIXME do we still care about this?
        // LOG_R("PUBLIC TICK METHOD BAD!\n");
        // error = true;
      }
      tick_methods.push_back(m);
    } else if (m->is_tock) {
      if (m->is_const) {
        LOG_R("CONST TOCK METHOD BAD!\n");
        error = true;
      }
      tock_methods.push_back(m);
    } else if (m->is_task) {
      if (m->is_const) {
        LOG_R("CONST TASK FUNCTION BAD!\n");
        error = true;
      }
      task_methods.push_back(m);
    } else if (m->is_public && m->is_const && m->is_func) {
      getters.push_back(m);
    } else {
      assert(m->is_func);
      func_methods.push_back(m);
    }
  }

  return error;
}

//------------------------------------------------------------------------------

FieldState merge_delta(FieldState a, FieldDelta b);
bool merge_series(state_map& ma, state_map& mb, state_map& out);

bool MtModule::trace() {
  bool error = false;
  LOG_G("Tracing %s\n", name().c_str());
  LOG_INDENT_SCOPE();

  // Field state produced by evaluating all public methods in the module in
  // lexical order.
  state_map state_mod;

  for (auto m : all_methods) {

    // Don't trace construtors and private methods.

    if (m->is_init || !m->is_public) {
      continue;
    }

    // Trace the method.

    MtTracer tracer;
    state_map state_method;
    tracer._mod_stack.push_back(this);
    tracer._method_stack.push_back(m);
    tracer._state_stack.push_back(&state_method);
    error |= tracer.trace_dispatch(m->node.get_field(field_body));

    if (error) {
      LOG_R("Tracing %s.%s failed\n", name().c_str(), m->name().c_str());
      break;
    }

    // Lock all states touched by the method.

    for (auto& pair : state_method) {
      auto old_state = pair.second;
      auto new_state = merge_delta(old_state, DELTA_EF);

      if (new_state == FIELD_INVALID) {
        LOG_R("Field %s was %s, now %s!\n", pair.first.c_str(), to_string(old_state), to_string(new_state));
        error = true;
        break;
      }

      pair.second = new_state;
    }

    // Merge this method's state with the module-wide state.

    error |= merge_series(state_mod, state_method, state_mod);
    if (error) {
      LOG_R("MtModule::trace - Cannot merge state_mod->state_method\n");

      LOG_R("state_mod:\n");
      LOG_INDENT();
      MtTracer::dump_trace(state_mod);
      LOG_DEDENT();

      LOG_R("state_method:\n");
      LOG_INDENT();
      MtTracer::dump_trace(state_method);
      LOG_DEDENT();
    }
  }

  if (error) return error;

  // Check that all sigs and regs ended up in a valid state.
  for (auto& pair : state_mod) {
    switch(pair.second) {
    case FIELD________:
      LOG_R("Field %s was never read or written!\n", pair.first.c_str());
      error = true;
      break;
    case FIELD____WR__:
      LOG_R("Register %s was written but never read or locked - internal error?\n", pair.first.c_str());
      error = true;
      break;
    case FIELD____WR_L:
      LOG_Y("Register %s was written and locked but never read - is it redundant?\n", pair.first.c_str());
      break;
    case FIELD_RD_____:
      LOG_Y("Field %s was read but never written - should it be const?\n", pair.first.c_str());
      break;
    case FIELD_RD_WR__:
      LOG_R("Register %s was written but never locked - internal error?\n", pair.first.c_str());
      error = true;
      break;
    case FIELD_RD_WR_L:
      break;
    case FIELD____WS__:
      LOG_Y("Signal %s was written but never read - is it an output?\n", pair.first.c_str());
      break;
    case FIELD____WS_L:
      break;
    default:
      LOG_R("Field %s has an invalid state %d\n", pair.first.c_str(), pair.second);
      error = true;
      break;
    }
  }

  // Check that all sigs and regs are represented in the final state map.
  for (auto f : all_fields) {
    if (f->is_submod()) continue;
    if (f->is_param()) continue;
    if (!state_mod.contains(f->name())) {
      LOG_R("No method in the public interface of %s touched field %s!\n", name().c_str(), f->name().c_str());
      error = true;
    }
  }

  return error;
}

//------------------------------------------------------------------------------
// Collect all inputs to all tock and getter methods and merge them into a list
// of input ports. Input ports can be declared in multiple tick/tock methods,
// but we don't want duplicates in the Verilog port list.

bool MtModule::collect_inputs() {
  bool error = false;

  assert(inputs.empty());

  std::set<std::string> dedup;

  for (auto m : tock_methods) {
    auto params =
        m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      if (!dedup.contains(param.name4())) {
        MtField *new_input = MtField::construct(param, true);
        inputs.push_back(new_input);
        dedup.insert(new_input->name());
      }
    }
  }

  for (auto m : getters) {
    auto params =
        m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      if (!dedup.contains(param.name4())) {
        MtField *new_input = MtField::construct(param, true);
        inputs.push_back(new_input);
        dedup.insert(new_input->name());
      }
    }
  }

  return error;
}

//------------------------------------------------------------------------------
// All fields written to in a tock method are outputs.

bool MtModule::collect_outputs() {
  bool error = false;

  assert(outputs.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {
    if (f->is_public() && !f->is_submod() && !f->is_param()) {
      outputs.push_back(f);
    }
  }

  return error;
}

//------------------------------------------------------------------------------
// All fields written to in a tick method are registers.

bool MtModule::collect_registers() {
  bool error = false;

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
        LOG_R("Unknown type on left side of assignment - %s\n", lhs.ts_node_type());
        lhs.dump_source_lines();
        error = true;
        return;
      }

      if (dedup.contains(lhs_name)) return;
      dedup.insert(lhs_name);

      auto field = get_field(lhs_name);
      if (field && !field->is_public()) registers.push_back(field);
    });
  }

  return error;
}

//------------------------------------------------------------------------------

bool MtModule::collect_submods() {
  bool error = 0;

  assert(submods.empty());

  for (auto f : all_fields) {
    if (source_file->lib->has_module(f->type_name())) {
      submods.push_back(f);
    } else {
      if (f->type_name() == "logic") {
      } else if (f->type_name() == "int") {
      } else if (get_enum(f->type_name())) {
        LOG_R("???");
        // FIXME this isn't working
      } else {
        LOG_R("Could not find module for submod %s\n", f->type_name().c_str());

        // FIXME don't make this an error yet
        //error = true;
      }
    }
  }

  return error;
}

//------------------------------------------------------------------------------
// All modules have populated their fields, match up tick/tock calls with their
// corresponding methods.

bool MtModule::load_pass2() {
  bool error = false;
  assert (!mod_class.is_null());
  error |= build_port_map();
  error |= sanity_check();
  return error;
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

bool MtModule::build_port_map() {
  assert(port_map.empty());

  bool error = false;

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_call_expression) return;
    if (child.get_field(field_function).sym != sym_field_expression) return;

    auto node_call = child;
    auto node_func = node_call.get_field(field_function);
    auto node_args = node_call.get_field(field_arguments);

    MtField *call_member = nullptr;
    MtModule *call_submod = nullptr;
    MtMethod *call_method = nullptr;

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
        if (call_member) {
          call_submod = source_file->lib->get_module(call_member->type_name());
          call_method = call_submod->get_method(node_method.text());
        } else {
          LOG_R("Submodule %s not found!\n", node_this.text().c_str());
          error = true;
          return;
        }
      }
    }

    auto arg_count = node_args.named_child_count();

    for (auto i = 0; i < arg_count; i++) {
      // auto key = call_member->name() + "." + call_method->params[i];
      auto key = call_member->name() + "_" + call_method->name() + "_" +
                 call_method->params[i];

      // std::string val = node_args.named_child(i).text();

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

  return error;
}

//------------------------------------------------------------------------------

bool MtModule::sanity_check() {
  bool error = false;

  // Inputs, outputs, registers, and submods must not overlap.

  std::set<std::string> field_names;

  for (auto n : inputs) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate input name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : outputs) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate output name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : registers) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate register name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : submods) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate submod name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  return error;
}

//------------------------------------------------------------------------------
#if 0
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
#endif
