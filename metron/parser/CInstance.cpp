#include "CInstance.hpp"

#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

CInstance::CInstance(CInstance* inst_parent, CNodeField* node_field)
    : inst_parent(inst_parent), node_field(node_field) {
  state_stack.push_back(TS_NONE);
}

CInstance::~CInstance() {}

//----------------------------------------

std::string_view CInstance::get_name() const {
  if (node_field) {
    return node_field->get_name();
  }
  else {
    return "<top>";
  }
}

//----------------------------------------

CInstance* CInstance::resolve(CNode* node) {

  if (node->as<CNodeFieldExpression>()) {
    auto cursor = node->child_head;
    CInstance* inst = this;
    while(cursor) {
      if (cursor->as<CNodePunct>()) cursor = cursor->node_next;
      inst = inst->resolve(cursor);
      if (!inst) return inst;
      assert(cursor);
      cursor = cursor->node_next;
    }
    return inst;
  }

  if (node->as<CNodePrefixExp>()) return resolve(node->child("rhs"));
  if (node->as<CNodeSuffixExp>()) return resolve(node->child("lhs"));

  if (auto id = node->as<CNodeIdentifier>()) {
    auto name = id->get_text();

    if (auto it = inst_map.find(name); it != inst_map.end()) {
      return (*it).second;
    }
    else {
      return nullptr;
    }
  }

  auto text = node->get_text();
  LOG_R("Could not resolve %.*s\n", text.size(), text.data());
  assert(false);
  return nullptr;
}

//----------------------------------------

CHECK_RETURN Err CInstance::log_action(CNode* node, TraceAction action) {
  Err err;

  assert(action == ACT_READ || action == ACT_WRITE);

  auto func = node->ancestor<CNodeFunction>();
  assert(func);

  if (action == ACT_READ) {
    func->self_reads.insert(node_field);
  }
  else if (action == ACT_WRITE) {
    func->self_writes.insert(node_field);
  }

  auto old_state = state_stack.back();
  auto new_state = merge_action(old_state, action);

  // LOG_R("%s %s %s\n", to_string(action), to_string(old_state),
  // to_string(new_state));

  auto t = typeid(*this).name();

  state_stack.back() = new_state;

  if (new_state == TS_INVALID) {
    LOG_R("Trace error: state went from %s to %s\n", to_string(old_state),
          to_string(new_state));
    err << ERR("Invalid context state\n");
  }

  return err;
}

//----------------------------------------

void CInstance::push_state() {
  assert(state_stack.size() > 0);
  state_stack.push_back(state_stack.back());
  for (auto pair : inst_map) pair.second->push_state();
}

void CInstance::pop_state() {
  assert(state_stack.size() > 0);
  state_stack.pop_back();
  for (auto pair : inst_map) pair.second->pop_state();
}

void CInstance::swap_state() {
  assert(state_stack.size() >= 2);

  auto s = state_stack.size();
  auto a = state_stack[s-2];
  auto b = state_stack[s-1];

  state_stack[s-2] = b;
  state_stack[s-1] = a;
  for (auto pair : inst_map) pair.second->swap_state();
}

void CInstance::merge_state() {
  assert(state_stack.size() >= 2);
  auto s = state_stack.size();
  auto a = state_stack[s-2];
  auto b = state_stack[s-1];

  state_stack[s-2] = merge_branch(a, b);
  state_stack.pop_back();
  for (auto pair : inst_map) pair.second->merge_state();
}

//------------------------------------------------------------------------------

CInstClass::CInstClass(CInstance* inst_parent, CNodeField* node_field,
                       CNodeClass* node_class)
    : CInstance(inst_parent, node_field), node_class(node_class) {
  for (auto node_field : node_class->all_fields) {
    auto field_name = node_field->get_name();

    if (node_field->node_decl->_type_class) {
      auto inst = new CInstClass(this, node_field, node_field->node_decl->_type_class);
      inst_map[field_name] = inst;
    } else if (node_field->node_decl->_type_struct) {
      auto inst = new CInstStruct(this, node_field, node_field->node_decl->_type_struct);
      inst_map[field_name] = inst;
    } else {
      auto inst = new CInstPrim(this, node_field);
      inst_map[field_name] = inst;
    }
  }
}

//----------------------------------------

void CInstClass::dump_tree() const {
  auto name = node_class->get_name();
  LOG_G("Class %.*s\n", int(name.size()), name.data());

  LOG_INDENT_SCOPE();
  for (auto pair : inst_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }

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

    if (node_field->node_decl->_type_struct) {
      inst_map[field_name] =
          new CInstStruct(this, node_field, node_field->node_decl->_type_struct);
    } else {
      inst_map[field_name] = new CInstPrim(this, node_field);
    }
  }
}

//----------------------------------------

void CInstStruct::dump_tree() const {
  auto name = node_struct->get_name();
  LOG_G("Struct %.*s\n", int(name.size()), name.data());

  LOG_INDENT_SCOPE();
  for (auto pair : inst_map) {
    LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
    pair.second->dump_tree();
  }
}

void CInstStruct::commit_state() {
  auto merged_state = TS_PENDING;
  for (auto pair : inst_map) {
    pair.second->commit_state();
    merged_state = merge_branch(merged_state, pair.second->state_stack.back());
  }
  state_stack.back() = merged_state;
  node_field->field_type = trace_state_to_field_type(merged_state);
}


//------------------------------------------------------------------------------

CInstPrim::CInstPrim(CInstance* inst_parent, CNodeField* node_field)
    : CInstance(inst_parent, node_field) {}

//----------------------------------------

void CInstPrim::dump_tree() const {
  //node_field->dump_debug();
  auto prim_type = node_field->child("decl")->child("type")->get_text();
  LOG_G("Primitive %.*s :", int(prim_type.size()), prim_type.data());

  for (auto state : state_stack) LOG_G(" %s", to_string(state));
  LOG_G("\n");
}

//----------------------------------------

void CInstPrim::commit_state() {
  assert(node_field);

  auto new_type = trace_state_to_field_type(state_stack.back());

  if (node_field->field_type == FT_UNKNOWN) {
    node_field->field_type = new_type;
  }
  assert(node_field->field_type == new_type);
}

//------------------------------------------------------------------------------

CCall::CCall(CInstClass* inst_class, CNodeCall* node_call, CNodeFunction* node_func)
    : inst_class(inst_class), node_call(node_call), node_func(node_func) {
}

//----------------------------------------

void CCall::dump_tree() {
  auto func_name = node_func->get_name();
  auto class_name = inst_class->get_name();
  LOG_G("Call %.*s::%.*s\n", int(class_name.size()), class_name.data(), int(func_name.size()), func_name.data());

  LOG_INDENT_SCOPE();
  for (auto pair : call_map) pair.second->dump_tree();
}

//------------------------------------------------------------------------------
