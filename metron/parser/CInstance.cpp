#include "CInstance.hpp"

#include "CNodeCall.hpp"
#include "CNodeClass.hpp"
#include "CNodeDeclaration.hpp"
#include "CNodeExpression.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "CNodeType.hpp"
#include "NodeTypes.hpp"

/*
Err CNode::trace(CInstance* instance, TraceAction action) {
  LOG_R("Don't know how to trace {{%s}}\n", typeid(*this).name());
  exit(-1);
}

Err CNode::trace_children(CInstance* instance, TraceAction action) {
  Err err;
  for (auto c : this) c->trace(instance, action);
  return err;
}

Err CNodeExpression::trace(CInstance* instance, TraceAction action) {
  return trace_children(instance, action);
}

Err CNodeFunction::trace(CInstance* instance, TraceAction action) {
  Err err;
  err << child_is<CNodeCompound>("body")->trace(instance, action);
  return err;
}


//------------------------------------------------------------------------------

Err CNodeCompound::trace(CInstance* instance, TraceAction action) {
  Err err;
  for (auto c : this) err << c->trace(instance, action);
  return err;
}

//------------------------------------------------------------------------------

Err CNodeReturn::trace(CInstance* instance, TraceAction action) {
  Err err;
  if (auto value = child("value")) {
    err << value->trace(instance, action);
  }
  return err;
}

//------------------------------------------------------------------------------

Err CNodeIdentifier::trace(CInstance* instance, TraceAction action) {
  Err err;

  CInstance* field = instance->resolve(get_string());

  return err;
}


*/

//------------------------------------------------------------------------------

std::string_view CInstance::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstance::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstance::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstance::dump_tree() { LOG_R("{{%s}}\n", typeid(*this).name()); }

//----------------------------------------

CHECK_RETURN Err CInstance::log_action(CNode* node, TraceAction action) {
  Err err;
  auto old_state = state_stack.back();
  auto new_state = merge_action(old_state, action);

  //LOG_R("%s %s %s\n", to_string(action), to_string(old_state), to_string(new_state));

  auto t = typeid(*this).name();

  if (old_state != new_state) {
    action_log.push_back({old_state, new_state, action, node});
  }

  state_stack.back() = new_state;

  if (new_state == CTX_INVALID) {
    LOG_R("Trace error: state went from %s to %s\n", to_string(old_state), to_string(new_state));
    dump_tree();
    err << ERR("Invalid context state\n");
  }

  return err;
}


//------------------------------------------------------------------------------

CInstClass::CInstClass(CInstance* parent, CNodeClass* node_class)
    : CInstance(parent), node_class(node_class) {
  for (auto node_func : node_class->all_functions) {
    auto func_inst = new CInstFunction(this, node_func);
    inst_functions.push_back(func_inst);
  }

  for (auto node_field : node_class->all_fields) {
    auto field_inst = new CInstField(this, node_field);
    inst_fields.push_back(field_inst);
  }
}

//----------------------------------------

std::string_view CInstClass::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstClass::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstClass::resolve(std::string_view name) {
  dump_tree();
  /*
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
  */
  for (auto f : inst_functions) {
    if (f->node_function->get_name() == name) return f;
  }
  for (auto f : inst_fields) {
    if (f->node_field->get_name() == name) return f;
  }
  LOG_R("Could not resolve %.*s\n", name.size(), name.data());
  assert(false);
  return nullptr;
}

//----------------------------------------

void CInstClass::dump_tree() {
  auto name = node_class->get_name();
  LOG_G("Class %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto f : inst_functions) f->dump_tree();
  for (auto f : inst_fields) f->dump_tree();
}

//------------------------------------------------------------------------------

CInstStruct::CInstStruct(CInstance* parent, CNodeStruct* node_struct)
    : CInstance(parent), node_struct(node_struct) {
  assert(node_struct);
  for (auto node_field : node_struct->all_fields) {
    auto inst_field = new CInstField(this, node_field);
    inst_fields.push_back(inst_field);
  }
}

//----------------------------------------

std::string_view CInstStruct::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstStruct::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstStruct::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstStruct::dump_tree() {
  auto name = node_struct->get_name();
  LOG_G("Struct %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto f : inst_fields) f->dump_tree();
}

//------------------------------------------------------------------------------

CInstField::CInstField(CInstance* parent, CNodeField* node_field)
    : CInstance(parent), node_field(node_field) {
  if (node_field->_type_class) {
    inst_decl = new CInstClass(this, node_field->_type_class);
  } else if (node_field->_type_struct) {
    inst_decl = new CInstStruct(this, node_field->_type_struct);
  } else if (node_field->is_array()) {
    auto node_type = node_field->child<CNodeType>();
    auto node_array = node_field->child("array");
    inst_decl = new CInstArray(this, node_type, node_array);
  } else {
    auto node_type = node_field->child<CNodeType>();
    inst_decl = new CInstPrimitive(this, node_type);
  }
}

//----------------------------------------

std::string_view CInstField::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstField::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstField::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstField::dump_tree() {
  auto name = node_field->get_name();
  LOG_G("Field %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  inst_decl->dump_tree();
}

//------------------------------------------------------------------------------

CInstFunction::CInstFunction(CInstance* parent, CNodeFunction* node_function)
    : CInstance(parent), node_function(node_function) {
  auto node_params = node_function->child("params");
  for (auto p : node_params) {
    if (auto param = p->as_a<CNodeDeclaration>()) {
      auto inst_param = new CInstParam(this, param);
      inst_params.push_back(inst_param);
    }
  }

  auto node_body = node_function->child("body");

  visit(node_body, [&](CNode* n) {
    auto call = n->as_a<CNodeCall>();
    if (!call) return;

    auto inst_call = new CInstCall(this, call);
    inst_calls.push_back(inst_call);
  });

  inst_return = new CInstReturn(this);
}

//----------------------------------------

std::string_view CInstFunction::get_name() const {
  return node_function->get_name();
}

//----------------------------------------

Err CInstFunction::trace(TraceAction action) {
  return node_function->trace(this, action);
}

//----------------------------------------

CInstance* CInstFunction::resolve(std::string_view name) {

  for (auto p : inst_params) {
    if (p->get_name() == name) return p;
  }

  assert(parent);
  return parent->resolve(name);
}

//----------------------------------------

void CInstFunction::dump_tree() {
  auto name = node_function->get_name();
  LOG_G("Function %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto p : inst_params) p->dump_tree();
  for (auto c : inst_calls) c->dump_tree();
  inst_return->dump_tree();
}

//------------------------------------------------------------------------------

CInstParam::CInstParam(CInstance* parent, CNodeDeclaration* node_decl)
    : CInstance(parent), node_decl(node_decl) {
  if (node_decl->is_array()) {
    auto node_type = node_decl->child_as<CNodeType>("type");
    auto node_array = node_decl->child_as<CNode>("array");

    inst_decl = new CInstArray(this, node_type, node_array);
  } else {
    auto node_type = node_decl->child_as<CNodeType>("type");

    if (node_type->is_a<CNodeBuiltinType>()) {
      inst_decl = new CInstPrimitive(this, node_type);
    } else {
      inst_decl = new CInstStruct(this, node_decl->_type_struct);
    }
  }
}

//----------------------------------------

std::string_view CInstParam::get_name() const {
  return node_decl->get_name();
}

//----------------------------------------

Err CInstParam::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstParam::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstParam::dump_tree() {
  auto name = node_decl->get_name();
  LOG_G("Param %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  inst_decl->dump_tree();
}

//------------------------------------------------------------------------------

CInstReturn::CInstReturn(CInstance* parent) : CInstance(parent) {}

//----------------------------------------

std::string_view CInstReturn::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstReturn::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstReturn::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstReturn::dump_tree() {
  LOG_G("Return\n");
}

//------------------------------------------------------------------------------

CInstCall::CInstCall(CInstance* parent, CNodeCall* node_call)
    : CInstance(parent), node_call(node_call) {
  auto node_args = node_call->child("func_args");
  for (auto n : node_args) {
    if (auto exp = n->as_a<CNodeExpression>()) {
      inst_args.push_back(new CInstArg(this, exp));
    }
  }
  inst_return = new CInstReturn(this);
}

//----------------------------------------

std::string_view CInstCall::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstCall::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstCall::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstCall::dump_tree() {
  if (node_call) {
    auto name = node_call->get_name();
    LOG_G("Call %.*s\n", int(name.size()), name.data());
  } else {
    LOG_G("Call <top>\n");
  }
  LOG_INDENT_SCOPE();
  for (auto a : inst_args) a->dump_tree();
  inst_return->dump_tree();
}

//------------------------------------------------------------------------------

CInstArg::CInstArg(CInstance* parent, CNodeExpression* node_arg)
    : CInstance(parent), node_arg(node_arg) {}

//----------------------------------------

std::string_view CInstArg::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstArg::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstArg::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstArg::dump_tree() {
  if (node_arg) {
    auto text = node_arg->get_text();
    LOG_G("Arg {{ %.*s }}\n", int(text.size()), text.data());
  } else {
    LOG_G("Arg <top>\n");
  }
}

//------------------------------------------------------------------------------

CInstPrimitive::CInstPrimitive(CInstance* parent, CNodeType* node_type)
    : CInstance(parent), node_type(node_type) {}

//----------------------------------------

std::string_view CInstPrimitive::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstPrimitive::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstPrimitive::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstPrimitive::dump_tree() {
  auto prim_type = node_type->get_text();
  LOG_G("Primitive %.*s\n", int(prim_type.size()), prim_type.data());
}

//------------------------------------------------------------------------------

CInstArray::CInstArray(CInstance* parent, CNodeType* node_type,
                       CNode* node_array)
    : CInstance(parent), node_type(node_type), node_array(node_array) {}

//----------------------------------------

std::string_view CInstArray::get_name() const {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return "";
}

//----------------------------------------

Err CInstArray::trace(TraceAction action) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return ERR("Can't trace CInstance base class\n");
}

//----------------------------------------

CInstance* CInstArray::resolve(std::string_view name) {
  LOG_R("###UNIMPLEMENTED### %s\n", __PRETTY_FUNCTION__);
  assert(false); exit(-1);
  return nullptr;
}

//----------------------------------------

void CInstArray::dump_tree() {
  auto text_type = node_type->get_text();
  auto text_array = node_array->get_text();
  LOG_G("Array %.*s%.*s\n", int(text_type.size()), text_type.data(),
        int(text_array.size()), text_array.data());
}

//------------------------------------------------------------------------------
