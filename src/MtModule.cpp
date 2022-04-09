#include "MtModule.h"

#include "MtCursor.h"
#include "MtModLibrary.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "metron_tools.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

//------------------------------------------------------------------------------

bool MtField::is_submod() const {
  return (node.source->lib->has_module(type_name()));
}

//------------------------------------------------------------------------------

MtModule::MtModule() {
}

CHECK_RETURN bool MtModule::init(MtSourceFile *_source_file, MnTemplateDecl _node) {
  bool error = false;

  source_file = _source_file;
  mod_template = _node;

  for (int i = 0; i < mod_template.child_count(); i++) {
    auto child = mod_template.child(i);

    if (child.sym == sym_template_parameter_list) {
      mod_param_list = MnTemplateParamList(child);
    }

    if (child.sym == sym_class_specifier) {
      mod_class = MnClassSpecifier(child);
    }
  }

  if (mod_param_list.is_null()) {
    LOG_R("No template parameter list found under template\n");
    error = true;
  }

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  }
  else {
    LOG_R("No class node found under template\n");
    error = true;
  }


  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN bool MtModule::init(MtSourceFile *_source_file, MnClassSpecifier _node) {
  bool error = false;

  source_file = _source_file;
  mod_template = MnNode::null;
  mod_param_list = MnNode::null;
  mod_class = _node;

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  }
  else {
    LOG_R("mod_class is null\n");
    error = true;
  }

  return error;
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

MtField *MtModule::get_input_field(const std::string &name) {
  for (auto f : input_fields)
    if (f->name() == name) return f;
  return nullptr;
}

MtParam *MtModule::get_input_param(const std::string &name) {
  for (auto p : input_params)
    if (p->name() == name) return p;
  return nullptr;
}

MtField *MtModule::get_output_field(const std::string &name) {
  for (auto n : output_fields)
    if (n->name() == name) return n;
  return nullptr;
}

MtMethod *MtModule::get_output_return(const std::string &name) {
  for (auto n : output_returns)
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
  for (auto param : modparams) LOG_G("  %s\n", param->name().c_str());
  LOG_B("Localparams:\n");
  for (auto param : localparams) LOG_G("  %s\n", param->name().c_str());
  LOG_B("Enums:\n");
  for (auto n : enums) LOG_G("  %s\n", n->name().c_str());
  LOG_B("All Fields:\n");
  for (auto n : all_fields) {
    LOG_G("  %s:%s %s\n",
      n->name().c_str(),
      n->type_name().c_str(),
      to_string(n->state)
    );
  }
  LOG_B("Input Fields:\n");
  for (auto n : input_fields)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Input Params:\n");
  for (auto n : input_params)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Output Fields:\n");
  for (auto n : output_fields)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  LOG_B("Output Returns:\n");
  for (auto n : output_returns)
    LOG_G("  %s:%s\n", n->name().c_str(), n->node.get_field(field_type).text().c_str());
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

  LOG_B("State map:\n");
  for (auto &kv : mod_states)
    LOG_G("  %s = %s\n", kv.first.c_str(), to_string(kv.second));

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

CHECK_RETURN Err MtModule::load_pass1() {
  Err error;

  error |= collect_params();
  error |= collect_methods();
  error |= collect_field_and_submods();

#if 0
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
#endif

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_params() {
  Err error;

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

CHECK_RETURN Err MtModule::collect_field_and_submods() {
  Err error;

  assert(all_fields.empty());
  assert(submods.empty());


  auto mod_body = mod_class.get_field(field_body).check_null();
  bool in_public = false;

  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym != sym_field_declaration) {
      continue;
    }

    // FIXME why are we excluding enums here? because they're not a "field"?
    if (n.get_field(field_type).sym == sym_enum_specifier) continue;

    auto new_field = MtField::construct(n, in_public);

    if (source_file->lib->has_module(new_field->type_name())) {
      submods.push_back(new_field);
    } else {
      all_fields.push_back(new_field);
    }
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_methods() {
  Err error;

  assert(all_methods.empty());

  bool in_public = false;
  auto mod_body = mod_class.get_field(field_body).check_null();
  for (const auto& n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
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

    m->is_const = false;
    for (const auto& n : func_decl) {
      if (n.sym == sym_type_qualifier && n.text() == "const") {
        m->is_const = true;
        break;
      }
    }

    m->has_return = func_type && func_type.text() != "void";


    m->is_init = func_name.starts_with("init") || func_type.is_null();
    m->is_tick = func_name.starts_with("tick");
    m->is_tock = func_name.starts_with("tock");

    m->is_task = !m->has_return;
    m->is_func = m->has_return;

    if (m->is_init || m->is_tick || m->is_tock) {
      m->is_task = false;
      m->is_func = false;
    }

    for (int i = 0; i < func_args.named_child_count(); i++) {
      auto param = func_args.named_child(i);
      assert(param.sym == sym_parameter_declaration);
      auto param_name = param.get_field(field_declarator).text();
      m->params.push_back(param_name);
    }

    all_methods.push_back(m);

    if (m->is_tick && m->has_return) {
      LOG_R("Tick method %s has a return value\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_public && !m->is_const && !(m->is_tick || m->is_tock || m->is_init)) {
      LOG_R("Public non-init/tick/tock method %s is not const\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_init && !m->is_public) {
      LOG_R("Init method %s is not public\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_init && m->is_const) {
      LOG_R("Init method %s is const\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_tick && m->is_const) {
      LOG_R("Tick method %s is const\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_tock && m->is_const) {
      LOG_R("Tock method %s is const\n", m->name().c_str());
      error = true;
      break;
    }

    if (m->is_init) {
      init_methods.push_back(m);
    } else if (m->is_tick) {
      tick_methods.push_back(m);
    } else if (m->is_tock) {
      tock_methods.push_back(m);
    } else if (m->is_task) {
      if (m->is_const) {
        LOG_R("CONST TASK FUNCTION BAD!\n");
        error = true;
      }
      task_methods.push_back(m);
    } else {
      assert(m->is_func);
      func_methods.push_back(m);
    }
  }

  return error;
}

//------------------------------------------------------------------------------

FieldState merge_delta(FieldState a, FieldDelta b);
CHECK_RETURN Err merge_series(state_map& ma, state_map& mb, state_map& out);

CHECK_RETURN Err MtModule::trace() {
  Err error;
  LOG_G("Tracing %s\n", name().c_str());
  LOG_INDENT_SCOPE();

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

    error |= merge_series(mod_states, state_method, mod_states);
    if (error) {
      LOG_R("MtModule::trace - Cannot merge state_mod->state_method\n");

      LOG_R("state_mod:\n");
      LOG_INDENT();
      MtTracer::dump_trace(mod_states);
      LOG_DEDENT();

      LOG_R("state_method:\n");
      LOG_INDENT();
      MtTracer::dump_trace(state_method);
      LOG_DEDENT();
    }
  }

  if (error) return error;

  // Check that all sigs and regs ended up in a valid state.
  for (auto& pair : mod_states) {

    auto field = get_field(pair.first);
    if (field) {
      field->state = pair.second;
    }


    switch(pair.second) {

    case FIELD_RD_WR_L:
    case FIELD____WS_L:
      // Yay written and locked regs/sigs!
      break;

    case FIELD________:
      LOG_R("Field %s was never read or written!\n", pair.first.c_str());
      error = true;
      break;
    case FIELD____WR__:
      LOG_R("Register %s was written but never read or locked - internal error?\n", pair.first.c_str());
      error = true;
      break;
    case FIELD____WR_L:
      if (field && !field->is_public()) {
        LOG_Y("Private register %s was written and locked but never read - is it redundant?\n", pair.first.c_str());
      }
      break;
    case FIELD_RD_____:
      if (field && !field->is_public()) {
        LOG_Y("Private field %s was read but never written - should it be const?\n", pair.first.c_str());
      }
      break;
    case FIELD_RD_WR__:
      LOG_R("Register %s was written but never locked - internal error?\n", pair.first.c_str());
      error = true;
      break;
    case FIELD____WS__:
      LOG_Y("Signal %s was written but never read - is it an output?\n", pair.first.c_str());
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
    if (!mod_states.contains(f->name())) {
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

CHECK_RETURN Err MtModule::collect_input_params() {
  Err error;

  assert(input_params.empty());

  std::set<std::string> dedup;

  for (auto m : all_methods) {
    if (!m->is_public) continue;

    auto params =
        m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      if (!dedup.contains(param.name4())) {
        /*
        MtField *new_input = MtField::construct(param, true);
        inputs.push_back(new_input);
        dedup.insert(new_input->name());
        */
        MtParam* new_input = MtParam::construct(param);
        input_params.push_back(new_input);
        dedup.insert(new_input->name());
      }
    }
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_input_fields() {
  Err error;

  assert(input_fields.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {
    if (f->is_public() && !f->is_submod() && !f->is_param()) {
      if (f->state == FIELD_RD_____) {
        input_fields.push_back(f);
      }
    }
  }

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_output_fields() {
  Err error;

  assert(output_fields.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {
    if (f->is_public() && !f->is_submod() && !f->is_param()) {
      if (f->state == FIELD____WS_L || f->state == FIELD____WR_L || f->state == FIELD_RD_WR_L) {
        output_fields.push_back(f);
      }
    }
  }

  for (auto m : all_methods) {
    if (m->is_public && m->has_return) {
      output_returns.push_back(m);
    }
  }


  /*
  for (auto m : submod_mod->all_methods) {
    if (!m->is_public || !m->has_return) continue;

    auto getter_type = m->node.get_field(field_type);
    auto getter_decl = m->node.get_field(field_declarator);
    auto getter_name = getter_decl.get_field(field_declarator);

    MtCursor sub_cursor(lib, submod_mod->source_file, str_out);
    sub_cursor.echo = echo;
    sub_cursor.in_ports = true;
    sub_cursor.id_replacements = replacements;

    sub_cursor.cursor = getter_type.start();

    err << emit_indent();
    err << sub_cursor.skip_ws();
    err << sub_cursor.emit_dispatch(getter_type);
    err << sub_cursor.emit_ws();
    err << emit_printf("%s_", submod_decl.text().c_str());
    err << sub_cursor.emit_dispatch(getter_name);
    err << emit_printf(";");
    err << emit_newline();
  }
  */

  return error;
}

//------------------------------------------------------------------------------
// All fields written to in a tick method are registers.

CHECK_RETURN Err MtModule::collect_registers() {
  Err error;

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
// All modules have populated their fields, match up tick/tock calls with their
// corresponding methods.

CHECK_RETURN Err MtModule::load_pass2() {
  Err error;
  assert (!mod_class.is_null());

  error |= collect_input_params();
  error |= collect_registers();
  error |= collect_input_fields();
  error |= collect_output_fields();

  error |= build_port_map();
  error |= sanity_check();
  return error;
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

CHECK_RETURN Err MtModule::build_port_map() {
  assert(port_map.empty());

  Err error;

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_assignment_expression) return;

    auto node_lhs = child.get_field(field_left);
    auto node_rhs = child.get_field(field_right);

    if (node_lhs.sym != sym_field_expression) return;

    auto key = node_lhs.text();
    auto val = node_rhs.text();

    port_map[key] = val;

    //child.dump_source_lines();
    //child.dump_tree();
    //printf("%s\n", child.text().c_str());
  });


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
      auto key = call_member->name() + "." + call_method->params[i];

      //auto key = call_member->name() + "_" + call_method->name() + "_" +
      //           call_method->params[i];

      // std::string val = node_args.named_child(i).text();

      std::string val;
      MtCursor cursor(source_file->lib, source_file, &val);
      auto arg_node = node_args.named_child(i);
      cursor.cursor = arg_node.start();
      error |= cursor.emit_dispatch(arg_node);

      // FIXME - multiple port bindings are OK? I'm not sure.

      auto it = port_map.find(key);
      if (it != port_map.end()) {
        if ((*it).second != val) {
          /*
          LOG_R("Error, got multiple different values for %s: '%s' and '%s'\n",
                key.c_str(), (*it).second.c_str(), val.c_str());
          error = true;
          */
        }
      } else {
        port_map.insert({key, val});
      }
    }
  });

  // Verify that all input ports of all submods are bound.

  for (const auto& submod : submods) {
    auto submod_mod = source_file->lib->get_module(submod->type_name());

    for (int i = 0; i < submod_mod->input_fields.size(); i++) {
      auto key = submod->name() + "." + submod_mod->input_fields[i]->name();

      if (!port_map.contains(key)) {
        LOG_R("No input bound to submod port %s\n", key.c_str());
        error |= true;
      }
    }

    for (int i = 0; i < submod_mod->input_params.size(); i++) {
      auto key = submod->name() + "." + submod_mod->input_params[i]->name();

      if (!port_map.contains(key)) {
        LOG_R("No input bound to submod port %s\n", key.c_str());
        error |= true;
      }
    }
  }
    
  /*
  for (const auto& submod : submods) {
    auto submod_mod = source_file->lib->get_module(submod->type_name());

    for (int i = 0; i < submod_mod->inputs.size(); i++) {
      auto key = submod->name() + "." + submod_mod->inputs[i]->name();

      if (!port_map.contains(key)) {
        LOG_R("No input bound to submod port %s\n", key.c_str());
        error |= true;
      }
    }
  }
  */

  return error;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::sanity_check() {
  Err error;

  // Inputs, outputs, registers, and submods must not overlap.

  std::set<std::string> field_names;

  for (auto n : input_fields) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate input name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : input_params) {
    if (field_names.contains(n->name())) {
      LOG_R("Duplicate input name %s\n", n->name().c_str());
      error = true;
    } else {
      field_names.insert(n->name());
    }
  }

  for (auto n : output_fields) {
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
