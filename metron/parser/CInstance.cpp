#include "CInstance.hpp"

#include "CNodeCall.hpp"
#include "CNodeClass.hpp"
#include "CNodeDeclaration.hpp"
#include "CNodeExpression.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStatement.hpp"
#include "CNodeStruct.hpp"
#include "CNodeType.hpp"
#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

CInstance::CInstance(CInstance* inst_parent, CNodeField* node_field)
    : inst_parent(inst_parent), node_field(node_field) {
  state_stack.push_back(CTX_NONE);
}

CInstance::~CInstance() {}

//----------------------------------------

std::string_view CInstance::get_name() const {
  NODE_ERR("Base class can't get_name()");
  return "";
}

//----------------------------------------

CInstance* CInstance::resolve(CNode* node) {
  NODE_ERR("Base class can't resolve()");
  return nullptr;
}

//----------------------------------------

void CInstance::dump_tree() const { NODE_ERR("Base class can't dump_tree()"); }

//----------------------------------------

/*
  void CInstance::push_state() {
    assert(state_stack.size() > 0);
    state_stack.push_back(state_stack.back());
  }

  void CInstance::pop_state() {
    assert(state_stack.size() > 0);
    state_stack.pop_back();
  }

  void CInstance::swap_state() {
    assert(state_stack.size() >= 2);

    auto s = state_stack.size();
    auto a = state_stack[s-2];
    auto b = state_stack[s-1];

    state_stack[s-2] = b;
    state_stack[s-1] = a;
  }

  void CInstance::merge_state() {
    assert(state_stack.size() >= 2);
    auto s = state_stack.size();
    auto a = state_stack[s-2];
    auto b = state_stack[s-1];

    state_stack[s-2] = merge_branch(a, b);
    state_stack.pop_back();
  }
*/

CHECK_RETURN Err CInstance::log_action(CNode* node, TraceAction action) {
  Err err;

  assert(action == ACT_READ || action == ACT_WRITE);

  auto old_state = state_stack.back();
  auto new_state = merge_action(old_state, action);

  // LOG_R("%s %s %s\n", to_string(action), to_string(old_state),
  // to_string(new_state));

  auto t = typeid(*this).name();

  state_stack.back() = new_state;

  if (new_state == CTX_INVALID) {
    LOG_R("Trace error: state went from %s to %s\n", to_string(old_state),
          to_string(new_state));
    err << ERR("Invalid context state\n");
  }

  return err;
}

//------------------------------------------------------------------------------

CInstClass::CInstClass(CInstance* inst_parent, CNodeField* node_field,
                       CNodeClass* node_class)
    : CInstance(inst_parent, node_field), node_class(node_class) {
  for (auto node_field : node_class->all_fields) {
    auto field_name = node_field->get_name();

    if (node_field->_type_class) {
      auto inst = new CInstClass(this, node_field, node_field->_type_class);
      inst_map[field_name] = inst;
      //class_map[field_name] = inst;
    } else if (node_field->_type_struct) {
      auto inst = new CInstStruct(this, node_field, node_field->_type_struct);
      inst_map[field_name] = inst;
      //struct_map[field_name] = inst;
    } else {
      auto inst = new CInstPrim(this, node_field);
      inst_map[field_name] = inst;
      //prim_map[field_name] = inst;
    }
  }
}

//----------------------------------------

std::string_view CInstClass::get_name() const {
  if (node_field) {
    return node_field->get_name();
  }
  else {
    return "<top>";
  }
}

//----------------------------------------

CInstance* CInstClass::resolve(CNode* node) {

  if (node->as_a<CNodeFieldExpression>()) {
    auto cursor = node->child_head;
    CInstance* inst = this;
    while(cursor) {
      if (cursor->as_a<CNodePunct>()) cursor = cursor->node_next;
      inst = inst->resolve(cursor);
      assert(cursor);
      cursor = cursor->node_next;
    }
    return inst;
  }

  if (node->as_a<CNodePrefixExp>()) return resolve(node->child("rhs"));
  if (node->as_a<CNodeSuffixExp>()) return resolve(node->child("lhs"));

  if (auto id = node->as_a<CNodeIdentifier>()) {
    auto name = id->get_text();

    if (auto it = inst_map.find(name); it != inst_map.end()) {
      return (*it).second;
    }
    else {
      return nullptr;
    }

    /*
    if (auto it = class_map.find(name); it != class_map.end()) {
      return (*it).second;
    }
    else if (auto it = struct_map.find(name); it != struct_map.end()) {
      return (*it).second;
    }
    else if (auto it = prim_map.find(name); it != prim_map.end()) {
      return (*it).second;
    }
    else {
      return nullptr;
    }
    */
  }

  auto name2 = node->get_text();
  LOG_R("Could not resolve %.*s\n", name2.size(), name2.data());
  assert(false);
  return nullptr;
}

//----------------------------------------

CHECK_RETURN Err CInstClass::log_action(CNode* node, TraceAction action) {
  return CInstance::log_action(node, action);
};

//----------------------------------------

void CInstClass::dump_tree() const {
  auto name = node_class->get_name();
  LOG_G("Class %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();

  for (auto pair : inst_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }

  /*
  for (auto pair : class_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }

  for (auto pair : struct_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }

  for (auto pair : prim_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }
  */

  for (auto call : entry_points) {
    LOG_G("Entry point ");
    call->dump_tree();
  }
}

//------------------------------------------------------------------------------

CInstStruct::CInstStruct(CInstance* inst_parent, CNodeField* node_field,
                         CNodeStruct* node_struct)
    : CInstance(inst_parent, node_field), node_struct(node_struct) {
  assert(node_struct);
  for (auto node_field : node_struct->all_fields) {
    auto field_name = node_field->get_name();

    if (node_field->_type_struct) {
      struct_map[field_name] =
          new CInstStruct(this, node_field, node_field->_type_struct);
    } else {
      prim_map[field_name] = new CInstPrim(this, node_field);
    }
  }
}

//----------------------------------------

std::string_view CInstStruct::get_name() const {
  return node_field->get_name();
}

//----------------------------------------

CInstance* CInstStruct::resolve(CNode* node) {

  if (node->as_a<CNodeFieldExpression>()) {
    auto cursor = node->child_head;
    CInstance* inst = this;
    while(cursor) {
      if (cursor->as_a<CNodePunct>()) cursor = cursor->node_next;
      inst = inst->resolve(cursor);
      assert(cursor);
      cursor = cursor->node_next;
    }
    return inst;
  }

  if (node->as_a<CNodePrefixExp>()) return resolve(node->child("rhs"));
  if (node->as_a<CNodeSuffixExp>()) return resolve(node->child("lhs"));

  if (auto id = node->as_a<CNodeIdentifier>()) {
    auto name = id->get_text();

    if (auto it = struct_map.find(name); it != struct_map.end()) {
      return (*it).second;
    }
    else if (auto it = prim_map.find(name); it != prim_map.end()) {
      return (*it).second;
    }
    else {
      return nullptr;
    }
  }

  assert(false);
  return nullptr;
}

//----------------------------------------

Err CInstStruct::log_action(CNode* node, TraceAction action) {
  Err err;
  for (auto pair : struct_map) {
    err << pair.second->log_action(node, action);
  }
  for (auto pair : prim_map) {
    err << pair.second->log_action(node, action);
  }
  return err;
}

//----------------------------------------

void CInstStruct::dump_tree() const {
  auto name = node_struct->get_name();
  LOG_G("Struct %.*s\n", int(name.size()), name.data());
  LOG_INDENT_SCOPE();

  for (auto pair : struct_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }

  for (auto pair : prim_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }
}

//------------------------------------------------------------------------------

CInstPrim::CInstPrim(CInstance* inst_parent, CNodeField* node_field)
    : CInstance(inst_parent, node_field) {}

//----------------------------------------

CInstance* CInstPrim::resolve(CNode* node) {
  if (auto id = node->as_a<CNodeIdentifier>()) {
    if (node_field->get_name() == id->get_text()) {
      return this;
    }
  }
  return nullptr;
}

//----------------------------------------

CHECK_RETURN Err CInstPrim::log_action(CNode* node, TraceAction action) {
  return CInstance::log_action(node, action);
};

//----------------------------------------

void CInstPrim::dump_tree() const {
  auto prim_type = node_field->child("type")->get_text();
  LOG_G("Primitive %.*s :", int(prim_type.size()), prim_type.data());
  for (auto state : state_stack) {
    LOG_G(" %s", to_string(state));
  }
  LOG_G("\n");
}

//------------------------------------------------------------------------------

#if 0
CInstReturn::CInstReturn(CNodeType* node_type) : node_type(node_type) {
  if (auto builtin = node_type->as_a<CNodeBuiltinType>()) {
    inst_value = new CInstPrimitive(node_type);
  }
  else if (auto node_struct = node_type->as_a<CNodeStructType>()) {
    auto top = node_type->ancestor<CNodeTranslationUnit>();

    printf("%p\n", top);

    //inst_value = new CInstStruct(node_type);
  }
}

void CInstReturn::dump_tree() {
  LOG_G("Return : ");
  inst_value->dump_tree();
}
#endif

//------------------------------------------------------------------------------

CCall::CCall(CInstClass* inst_class, CNodeCall* node_call, CNodeFunction* node_func)
    : inst_class(inst_class), node_call(node_call), node_func(node_func) {

  /*
  auto node_params = node_func->child("params");
  for (auto p : node_params) {
    if (auto node_param = p->as_a<CNodeDeclaration>()) {
      CInstance* inst_value = nullptr;

      auto node_type = node_param->child_as<CNodeType>("type");
      if (node_type->as_a<CNodeBuiltinType>()) {
        inst_value = new CInstPrim(inst_parent, node_type);
      } else {
        inst_value = new CInstStruct(inst_parent, node_param->_type_struct);
      }

      assert(inst_value);
      inst_arg_map[node_param->get_name()] = inst_value;

      //auto inst_arg = new CInstArg(param);
      //inst_args.push_back(inst_arg);
    }
  }
  */

#if 0
  auto node_return = node_func->child_as<CNodeType>("return_type");
  assert(node_return);

  if (node_return->get_name() != "void") {

    //inst_return = new CInstReturn(node_return);

    if (auto builtin_type = node_return->as_a<CNodeBuiltinType>()) {
      inst_return = new CInstPrimitive(builtin_type);
    } else if (auto struct_type = node_return->as_a<CNodeStructType>()) {
      CNodeStruct* node_struct = node_call->resolve_struct(struct_type->get_name());

      inst_return = new CInstStruct(node_struct);
    }
  }
#endif
}

//----------------------------------------

#if 0
CInstance* CCall::resolve(CNode* node) {
  /*
  if (node->as_a<CNodeReturn>()) {
    return inst_return;
  }
  */

  /*
  if (auto field_exp = node->as_a<CNodeFieldExpression>()) {
    return parent->resolve_field(node);
  }
  */

  /*
  if (auto id = node->as_a<CNodeIdentifier>()) {
    if (auto arg = inst_arg_map[node->get_name()]) {
      return arg;
    }
  }
  */

  /*
  for (auto p : inst_args) {
    if (p->get_name() == node->get_name()) return p;
  }
  */

  assert(false);
  return nullptr;

  // assert(parent);
  return inst_parent->resolve(node);
}
#endif

//----------------------------------------

void CCall::dump_tree() {
  auto func_name = node_func->get_name();
  auto class_name = inst_class->get_name();
  LOG_G("Call %.*s::%.*s\n", int(class_name.size()), class_name.data(), int(func_name.size()), func_name.data());
  LOG_INDENT_SCOPE();

  /*
  for (auto p : inst_arg_map) {
    LOG_G("Arg %.*s : ", int(p.first.size()), p.first.data());
    p.second->dump_tree();
  }
  if (inst_return) inst_return->dump_tree();
  */
  for (auto c : inst_call_map) {
    c.second->dump_tree();
  }
}

//------------------------------------------------------------------------------

#if 0
CInstArg::CInstArg(CNodeDeclaration* node_param)
    : node_param(node_param) {
  if (node_param->is_array()) {
    auto node_type  = node_param->child_as<CNodeType>("type");
    auto node_array = node_param->child_as<CNode>("array");

    inst_value = new CInstArray(node_type, node_array);
  } else {
    auto node_type = node_param->child_as<CNodeType>("type");

    if (node_type->as_a<CNodeBuiltinType>()) {
      inst_value = new CInstPrimitive(node_type);
    } else {
      inst_value = new CInstStruct(node_param->_type_struct);
    }
  }
}

//----------------------------------------

std::string_view CInstArg::get_name() const { return node_param->get_name(); }

//----------------------------------------

void CInstArg::dump_tree() {
  auto name = node_param->get_name();
  LOG_G("Param %.*s : ", int(name.size()), name.data());
  inst_value->dump_tree();
}
#endif

//------------------------------------------------------------------------------
