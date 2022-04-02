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
  return (node.source->lib->has_module(type_name()));
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
  if (mod_struct.is_null()) {
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
  // modparam = struct template parameter
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
    m->is_tick = m->is_task && func_name.starts_with("tick");
    m->is_tock = m->is_task && func_name.starts_with("tock");

    m->is_const = false;
    for (auto n : func_decl) {
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
        printf("CONST INIT BAD / PRIVATE INIT BAD\n");
        exit(-1);
      }
      init_methods.push_back(m);
    } else if (m->is_tick) {
      if (m->is_public) {
        printf("PUBLIC TICK METHOD BAD!\n");
        exit(-1);
      }
      tick_methods.push_back(m);
    } else if (m->is_tock) {
      if (m->is_const) {
        printf("CONST TOCK METHOD BAD!\n");
        exit(-1);
      }
      tock_methods.push_back(m);
    } else if (m->is_task) {
      if (m->is_const) {
        printf("CONST TASK FUNCTION BAD!\n");
        exit(-1);
      }
      task_methods.push_back(m);
    } else if (m->is_public && m->is_const && m->is_func) {
      getters.push_back(m);
    } else {
      assert(m->is_func);
      func_methods.push_back(m);
    }
  }
}

//------------------------------------------------------------------------------

void MtModule::build_call_tree() {
  // Hook up callee->caller method pointers

  printf("\n");
  printf("MODULE! %s %d\n", name().c_str(), get_rank());
  for (auto m : all_methods) {
    int depth = 1;

    // Only trace public non-const methods that aren't constructors.
    if (m->is_init || !m->is_public || m->is_const) continue;

    for (int i = 0; i < depth; i++) printf(" ");
    printf("METHOD! %s\n", m->name().c_str());
    auto node_body = m->node.get_field(field_body);
    MtDelta delta;
    build_call_tree(m, node_body, depth + 1, delta);
  }
}

void MtModule::build_call_tree(MtMethod *method, MnNode n, int depth, MtDelta& delta) {

  if (n.sym == sym_identifier) {
    if (get_field(n.text())) {
      for (int i = 0; i < depth; i++) printf(" ");
      printf("id %s\n", n.text().c_str());
    }
    return;
  }

  if (n.sym == sym_call_expression) {
    auto node_func = n.get_field(field_function);

    if (node_func.sym == sym_field_expression) {
      for (int i = 0; i < depth; i++) printf(" ");
      printf("FCALL! %s.%s -> %s\n",
        name().c_str(), method->name().c_str(),
        node_func.text().c_str());

      // Field call. Pull up the submodule and traverse into the method.

      //node_func.dump_tree();
      auto submod_name = node_func.get_field(field_argument).text();
      auto submod = get_submod(submod_name);
      assert(submod);
      auto submod_type = submod->type_name();
      auto submod_mod = source_file->lib->get_module(submod_type);
      assert(submod_mod);
      auto submod_method = submod_mod->get_method(node_func.get_field(field_field).text());
      assert(submod_method);

      for (int i = 0; i < depth + 1; i++) printf(" ");
      printf("TO SUBMOD!\n");
      submod_mod->build_call_tree(submod_method, submod_method->node.get_field(field_body), depth + 1, delta);

    } else if (node_func.sym == sym_identifier) {
      //node_func.dump_tree();

      auto sibling_method = get_method(node_func.text());

      if (sibling_method) {
        for (int i = 0; i < depth; i++) printf(" ");
        printf("*** MCALL! %s.%s -> %s\n",
          name().c_str(), method->name().c_str(),
          node_func.text().c_str());
        build_call_tree(sibling_method, sibling_method->node.get_field(field_body), depth + 1, delta);
      }
      else {
        // Utility method call like bN()
        for (int i = 0; i < depth; i++) printf(" ");
        printf("XCALL! %s.%s -> %s\n",
          name().c_str(), method->name().c_str(),
          node_func.text().c_str());
      }

    } else if (node_func.sym == sym_template_function) {

      auto sibling_method = get_method(node_func.get_field(field_name).text());

      if (sibling_method) {
        // Should probably not see any of these yet?
        debugbreak();
        for (int i = 0; i < depth; i++) printf(" ");
        printf("TCALL! %s.%s -> %s\n",
          name().c_str(), method->name().c_str(),
          node_func.text().c_str());
      }
      else {
        // Templated utility method call like bx<>, dup<>
      }

      //node_func.dump_tree();

    } else {
      // printf("  CALL! %s\n", c.text().c_str());
      // printf("  CALL! %s\n", c.get_field(field_function).text().c_str());

      LOG_R("MtModule::build_call_tree - don't know what to do with %s\n",
        n.ts_node_type());
      n.dump_tree();
      debugbreak();
    }
    return;
  }

  for (auto c : n) {
    build_call_tree(method, c, depth + 1, delta);
  }
}

//------------------------------------------------------------------------------
// Collect all inputs to all tock and getter methods and merge them into a list
// of input ports. Input ports can be declared in multiple tick/tock methods,
// but we don't want duplicates in the Verilog port list.

void MtModule::collect_inputs() {
  assert(inputs.empty());

  std::set<std::string> dedup;

  for (auto m : tock_methods) {
    auto params =
        m->node.get_field(field_declarator).get_field(field_parameters);

    for (auto param : params) {
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

    for (auto param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      if (!dedup.contains(param.name4())) {
        MtField *new_input = MtField::construct(param, true);
        inputs.push_back(new_input);
        dedup.insert(new_input->name());
      }
    }
  }
}

//------------------------------------------------------------------------------
// All fields written to in a tock method are outputs.

void MtModule::collect_outputs() {
  assert(outputs.empty());

  std::set<std::string> dedup;

  for (auto f : all_fields) {
    if (f->is_public() && !f->is_submod() && !f->is_param()) {
      outputs.push_back(f);
    }
  }
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
      if (field && !field->is_public()) registers.push_back(field);
    });
  }
}

//------------------------------------------------------------------------------

void MtModule::collect_submods() {
  assert(submods.empty());

  for (auto f : all_fields) {
    if (source_file->lib->has_module(f->type_name())) {
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
        call_submod = source_file->lib->get_module(call_member->type_name());
        call_method = call_submod->get_method(node_method.text());
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
//------------------------------------------------------------------------------

void MtModule::check_temporal() {
  for (auto m : all_methods) {
    // if (m->rank() == 0) {
    // }
  }
}

void MtModule::check_temporal_root(MtMethod *root) {
  MtDelta delta;

  root->node.dump_tree();

  for (auto n : root->node) {
  }
}

//------------------------------------------------------------------------------
