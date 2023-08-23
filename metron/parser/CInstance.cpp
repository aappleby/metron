#include "CInstance.hpp"

#include "CNodeClass.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "CNodeType.hpp"
#include "CNodeCall.hpp"
#include "CNodeExpression.hpp"

#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

void CInstance::dump() {
  LOG_R("{{%s}}\n", typeid(*this).name());
}

//------------------------------------------------------------------------------

CInstClass::CInstClass(CNodeClass* node_class) : node_class(node_class) {
  for (auto node_func : node_class->all_functions) {
    auto func_inst = new CInstFunction(node_func);
    functions.push_back(func_inst);
  }

  for (auto node_field : node_class->all_fields) {
    auto field_inst = new CInstField(node_field);
    fields.push_back(field_inst);
  }
}

void CInstClass::dump() {
  auto name = node_class->get_name();
  LOG_G("Class %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto f : functions) f->dump();
  for (auto f : fields) f->dump();
}

//------------------------------------------------------------------------------

CInstStruct::CInstStruct(CNodeStruct* node_struct) : node_struct(node_struct) {
  assert(node_struct);
  for (auto node_field : node_struct->all_fields) {
    auto inst_field = new CInstField(node_field);
    inst_fields.push_back(inst_field);
  }
}

void CInstStruct::dump() {
  auto name = node_struct->get_name();
  LOG_G("Struct %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto f : inst_fields) f->dump();
}

//------------------------------------------------------------------------------

CInstField::CInstField(CNodeField* node_field) : node_field(node_field) {
  if (node_field->_type_class) {
    inst_decl = new CInstClass(node_field->_type_class);
  }
  else if (node_field->_type_struct) {
    inst_decl = new CInstStruct(node_field->_type_struct);
  }
  else if (node_field->is_array()) {
    auto node_type = node_field->child_as<CNodeType>("decl_type");
    auto node_array = node_field->child_as<CNode>("decl_array");
    inst_decl = new CInstArray(node_type, node_array);
  }
  else {
    auto node_type = node_field->child_as<CNodeType>("decl_type");
    inst_decl = new CInstPrimitive(node_type);
  }
}

void CInstField::dump() {
  auto name = node_field->get_name();
  LOG_G("Field %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  inst_decl->dump();
}

//------------------------------------------------------------------------------

CInstFunction::CInstFunction(CNodeFunction* node_function) : node_function(node_function) {
  auto node_params = node_function->child("params");
  for (auto n : node_params) {
    auto inst_param = new CInstParam(n->is_a<CNodeDeclaration>());
    inst_params.push_back(inst_param);
  }

  auto node_body = node_function->child("body");

  visit(node_body, [&](CNode* n) {
    auto call = n->as_a<CNodeCall>();
    if (!call) return;

    auto inst_call = new CInstCall(call);
    inst_calls.push_back(inst_call);

  });

  inst_return = new CInstReturn();

}

void CInstFunction::dump() {
  auto name = node_function->get_name();
  LOG_G("Function %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  for (auto p : inst_params) p->dump();
  for (auto c : inst_calls) c->dump();
  inst_return->dump();
}

//------------------------------------------------------------------------------

CInstParam::CInstParam(CNodeDeclaration* node_decl) : node_decl(node_decl) {
  if (node_decl->is_array()) {
    auto node_type = node_decl->child_as<CNodeType>("decl_type");
    auto node_array = node_decl->child_as<CNode>("decl_array");

    inst_decl = new CInstArray(node_type, node_array);
  }
  else {
    auto node_type = node_decl->child_as<CNodeType>("decl_type");

    if (node_type->child("builtin_name")) {
      inst_decl = new CInstPrimitive(node_type);
    }
    else {
      inst_decl = new CInstStruct(node_decl->_type_struct);
    }
  }
}

void CInstParam::dump() {
  auto name = node_decl->get_name();
  LOG_G("Param %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();
  inst_decl->dump();
}

//------------------------------------------------------------------------------

CInstReturn::CInstReturn() {
}

//------------------------------------------------------------------------------

CInstCall::CInstCall() {
}

CInstCall::CInstCall(CNodeCall* node_call) : node_call(node_call) {
  auto node_args = node_call->child("func_args");
  for (auto n : node_args) {
    auto exp = n->is_a<CNodeExpression>();
    inst_args.push_back(new CInstArg(exp));
  }
  inst_return = new CInstReturn();
}

void CInstCall::dump() {
  if (node_call) {
    auto name = node_call->get_name();
    LOG_G("Call %.*s\n", int(name.size()), name.data());
  }
  else {
    LOG_G("Call <top>\n");
  }
  LOG_INDENT_SCOPE();
  for (auto a : inst_args) a->dump();
  inst_return->dump();
}

//------------------------------------------------------------------------------

CInstArg::CInstArg() {
}

CInstArg::CInstArg(CNodeExpression* node_arg) : node_arg(node_arg) {
}

void CInstArg::dump() {
  if (node_arg) {
    auto text = node_arg->get_text();
    LOG_G("Arg {{ %.*s }}\n", int(text.size()), text.data());
  }
  else {
    LOG_G("Arg <top>\n");
  }
}

//------------------------------------------------------------------------------

CInstPrimitive::CInstPrimitive(CNodeType* node_type) : node_type(node_type) {
}

void CInstPrimitive::dump() {
  auto prim_type = node_type->get_text();
  LOG_G("Primitive %.*s\n", int(prim_type.size()), prim_type.data());
}

//------------------------------------------------------------------------------

CInstArray::CInstArray(CNodeType* node_type, CNode* node_array) : node_type(node_type), node_array(node_array) {
}

void CInstArray::dump() {
  auto text_type = node_type->get_text();
  auto text_array = node_array->get_text();
  LOG_G("Array %.*s%.*s\n", int(text_type.size()), text_type.data(), int(text_array.size()), text_array.data());
}

//------------------------------------------------------------------------------
