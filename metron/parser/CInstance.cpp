#include "CInstance.hpp"

#include "NodeTypes.hpp"

//------------------------------------------------------------------------------

CInstance::CInstance(std::string name, bool is_public, CInstance* inst_parent)
    : name(name), is_public(is_public), inst_parent(inst_parent) {
  state_stack.push_back(TS_NONE);
}

CInstance::~CInstance() {}

//----------------------------------------

std::string_view CInstance::get_name() const {
  return name;
}

std::string CInstance::get_path() const {
  if (inst_parent) {
    return inst_parent->get_path() + "." + name;
  }
  else {
    return name;
  }
}

//----------------------------------------

CInstance* CInstance::resolve(CNode* node) {

  //LOG_R("----------\n");
  //LOG_R("text %s\n", node->get_textstr().c_str());
  //dump_tree();
  //LOG_R("----------\n");

  if (node->as<CNodeLValue>()) {
    return resolve(node->child("name"));
  }

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
    auto name = id->get_textstr();
    if (auto inst = resolve(name)) return inst;
  }

  if (inst_parent) {
    return inst_parent->resolve(node);
  }

  auto text = node->get_text();
  LOG_R("Could not resolve %.*s\n", text.size(), text.data());
  //assert(false);

  return nullptr;
}

//----------------------------------------

CInstance* CInstance::resolve(std::string name) {
  for (auto child : children) {
    if (child->name == name) return child;
  }
  return inst_parent ? inst_parent->resolve(name) : nullptr;
}

//----------------------------------------

CHECK_RETURN Err CInstance::log_action(CNode* node, TraceAction action, call_stack& stack) {
  Err err;

  if (name == "@return") {
    LOG("wat going on %d\n", action);
    return err;
  }

  assert(action == ACT_READ || action == ACT_WRITE);

  //auto constructor = node->ancestor<CNodeConstructor>();

  auto func = node->ancestor<CNodeFunction>();
  assert(func);

  if (action == ACT_READ) {
    func->self_reads.insert(this);
  }
  else if (action == ACT_WRITE) {
    func->self_writes.insert(this);
  }

  if (auto constructor = stack[0]->as<CNodeConstructor>()) {
    //LOG_R("not recording action because we're inside init()\n");
    return err;
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
  //for (auto pair : inst_map) pair.second->push_state();
  for (auto child : children) child->push_state();
}

//----------------------------------------

void CInstance::pop_state() {
  assert(state_stack.size() > 0);
  state_stack.pop_back();
  //for (auto pair : inst_map) pair.second->pop_state();
  for (auto child : children) child->pop_state();
}

//----------------------------------------

void CInstance::swap_state() {
  assert(state_stack.size() >= 2);

  auto s = state_stack.size();
  auto a = state_stack[s-2];
  auto b = state_stack[s-1];

  state_stack[s-2] = b;
  state_stack[s-1] = a;
  //for (auto pair : inst_map) pair.second->swap_state();
  for (auto child : children) child->swap_state();
}

//----------------------------------------

void CInstance::merge_state() {
  assert(state_stack.size() >= 2);
  auto s = state_stack.size();
  auto a = state_stack[s-2];
  auto b = state_stack[s-1];

  state_stack[s-2] = merge_branch(a, b);
  state_stack.pop_back();
  //for (auto pair : inst_map) pair.second->merge_state();
  for (auto child : children) child->merge_state();
}

//----------------------------------------

bool CInstance::check_port_directions(CInstance* b) {
  auto a = this;
  auto sa = a->state_stack.back();
  auto sb = b->state_stack.back();

  LOG("Checking %s:%s vs %s:%s\n",
    a->get_path().c_str(), to_string(sa),
    b->get_path().c_str(), to_string(sb)
  );

  assert(sa >= TS_NONE || sa <= TS_REGISTER);
  assert(sa >= TS_NONE || sb <= TS_REGISTER);

  bool a_drives = (sa != TS_NONE) && (sa != TS_INPUT);
  bool b_drives = (sb != TS_NONE) && (sb != TS_INPUT);

  // Floating inputs are invalid.
  if (!a_drives && sb == TS_INPUT) return false;
  if (!b_drives && sa == TS_INPUT) return false;

  // Port collisions are invalid.
  if (a_drives && b_drives) return false;

  // Mismatched port sizes (how?) are invalid.
  assert(a->children.size() == b->children.size());
  if (a->children.size() != b->children.size()) return false;

  // All child port directions must match as well.
  for (int i = 0; i < a->children.size(); i++) {
    auto ca = a->children[i];
    auto cb = b->children[i];

    assert(ca->is_public == cb->is_public);
    if (ca->is_public) {
      if (!ca->check_port_directions(cb)) return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------

CInstClass::CInstClass(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field, CNodeClass* node_class)
: CInstance(name, is_public, inst_parent), node_field(node_field), node_class(node_class)
{

  bool child_is_public = false;

  for (auto child : node_class->child("body")) {
    if (auto access = child->as<CNodeAccess>()) {
      child_is_public = child->get_text() == "public:";
      continue;
    }

    if (auto node_field = child->as<CNodeField>()) {
      auto field_name = node_field->get_namestr();

      if (node_field->node_decl->_type_class) {
        // FIXME add binding fields here
        auto inst = new CInstClass(field_name, child_is_public, this, node_field, node_field->node_decl->_type_class);
        children.push_back(inst);
      } else if (node_field->node_decl->_type_struct) {
        auto inst = new CInstStruct(field_name, child_is_public, this, node_field, node_field->node_decl->_type_struct);
        children.push_back(inst);
      } else {
        auto inst = new CInstPrim(field_name, child_is_public, this, node_field);
        children.push_back(inst);
      }
    }

    /*if (child_is_public)*/ {
      if (auto node_func = child->as<CNodeFunction>()) {
        auto func_name = node_func->get_namestr();

        auto inst_func = new CInstFunc(func_name, child_is_public, this, node_func);

        if (auto node_constructor = child->as<CNodeConstructor>()) {
          inst_func->is_constructor = true;
        }

        children.push_back(inst_func);
      }
    }
  }
}

//----------------------------------------

CHECK_RETURN Err CInstClass::log_action(CNode* node, TraceAction action, call_stack& stack) {
  Err err;
  //for (auto pair : inst_map) err << pair.second->log_action(node, action);
  return err;
}

//----------------------------------------

void CInstClass::commit_state() {
  for (auto child : children) {
    // Don't cross module boundaries when committing traced state
    //if (!child->as<CInstClass>()) {
      child->commit_state();
    //}
  }
}

//----------------------------------------

void CInstClass::dump_tree() const {
  if (is_public) {
    LOG_G("Class %s", get_path().c_str());
  }
  else {
    LOG_R("Class %s", get_path().c_str());
  }

  for (auto state : state_stack) LOG(" %s", to_string(state));
  LOG("\n");

  LOG_INDENT_SCOPE();
  for (auto child : children) child->dump_tree();

  //auto name = node_class->get_name();
  //LOG_G("Class %.*s\n", int(name.size()), name.data());

  //LOG_INDENT_SCOPE();
  //for (auto pair : inst_map) {
  //  LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
  //  pair.second->dump_tree();
  //}

  //for (auto call : entry_points) {
  //  LOG_G("Entry point ");
  //  call->dump_tree();
  //}
}

//------------------------------------------------------------------------------

//

CInstStruct::CInstStruct(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field, CNodeStruct* node_struct)
    : CInstance(name, is_public, inst_parent), node_field(node_field), node_struct(node_struct) {
  assert(node_struct);
  //assert(node_field);
  //node_struct = node_field->node_decl->_type_struct;

  for (auto struct_field : node_struct->all_fields) {
    auto field_name = struct_field->get_namestr();

    if (struct_field->node_decl->_type_struct) {
      auto inst_field = new CInstStruct(field_name, is_public, this, struct_field, struct_field->node_decl->_type_struct);
      children.push_back(inst_field);
    } else {
      auto inst_field = new CInstPrim(field_name, is_public, this, struct_field);
      children.push_back(inst_field);
    }
  }
}

//----------------------------------------

void CInstStruct::dump_tree() const {
  if (is_public) {
    LOG_G("Struct %s", get_path().c_str());
  }
  else {
    LOG_R("Struct %s", get_path().c_str());
  }

  for (auto state : state_stack) LOG(" %s", to_string(state));
  LOG("\n");

  LOG_INDENT_SCOPE();
  for (auto child : children) child->dump_tree();

  ///auto name = node_struct->get_name();
  ///LOG_G("Struct %.*s :", int(name.size()), name.data());
  ///LOG_G("\n");

  ///LOG_INDENT_SCOPE();
  ///for (auto pair : inst_map) {
  ///  LOG_G("Field %.*s : ", int(pair.first.size()), pair.first.data());
  ///  pair.second->dump_tree();
  ///}
}

CHECK_RETURN Err CInstStruct::log_action(CNode* node, TraceAction action, call_stack& stack) {
  Err err;
  //for (auto pair : inst_map) err << pair.second->log_action(node, action);
  for (auto child : children) err << child->log_action(node, action, stack);
  return err;
}

void CInstStruct::commit_state() {
  for (auto child : children) {
    child->commit_state();
  }

  auto merged_state = TS_PENDING;
  for (auto child : children) {
    merged_state = merge_branch(merged_state, child->state_stack.back());
  }
  state_stack.back() = merged_state;
  //node_field->field_type = trace_state_to_field_type(merged_state);
}


//------------------------------------------------------------------------------

CInstPrim::CInstPrim(std::string name, bool is_public, CInstance* inst_parent, CNodeField* node_field)
    : CInstance(name, is_public, inst_parent), node_field(node_field) {}

//----------------------------------------

void CInstPrim::dump_tree() const {
  if (is_public) {
    LOG_G("Prim %s", get_path().c_str());
  }
  else {
    LOG_R("Prim %s", get_path().c_str());
  }

  for (auto state : state_stack) LOG(" %s", to_string(state));
  LOG("\n");

  //auto prim_type = node_field->child("decl")->child("type")->get_text();
  //LOG_G("Primitive %.*s :", int(prim_type.size()), prim_type.data());

  //for (auto state : state_stack) LOG(" %s", to_string(state));
  //LOG_G("\n");
}

//----------------------------------------

CHECK_RETURN Err CInstPrim::log_action(CNode* node, TraceAction action, call_stack& stack) {
  return CInstance::log_action(node, action, stack);
}

//----------------------------------------

void CInstPrim::commit_state() {
  assert(node_field);

  /*
  auto new_type = trace_state_to_field_type(state_stack.back());

  if (node_field->field_type == FT_UNKNOWN) {
    node_field->field_type = new_type;
  }

  assert(node_field->field_type == new_type);
  */
}

//------------------------------------------------------------------------------

CInstFunc::CInstFunc(std::string name, bool is_public, CInstance* inst_parent, CNodeFunction* node_func)
: CInstance(name, is_public, inst_parent), node_func(node_func) {

  auto repo = node_func->ancestor<CNodeClass>()->repo;

  for (auto param : node_func->params) {
    auto param_name = param->get_namestr();
    if (param->_type_struct) {
      auto inst_field = new CInstStruct(param_name, is_public, this, nullptr, param->_type_struct);
      children.push_back(inst_field);
    } else {
      auto inst_field = new CInstPrim(param_name, is_public, this, nullptr);
      children.push_back(inst_field);
    }
  }

  auto ret_type = node_func->child("return_type");
  if (ret_type && ret_type->get_text() != "void") {
    if (auto struct_type = ret_type->as<CNodeStructType>()) {
      auto node_struct = repo->get_struct(struct_type->get_text());
      auto inst = new CInstStruct("@return", is_public, this, nullptr, node_struct);
      children.push_back(inst);
    }
    else if (auto builtin_type = ret_type->as<CNodeBuiltinType>()) {
      auto inst = new CInstPrim("@return", is_public, this, nullptr);
      children.push_back(inst);
    }
    else {
      assert(false);
    }
  }
}

//----------------------------------------

void CInstFunc::dump_tree() const {
  if (is_public) {
    LOG_G("Func %s", get_path().c_str());
  }
  else {
    LOG_R("Func %s", get_path().c_str());
  }

  for (auto state : state_stack) LOG(" %s", to_string(state));
  LOG("\n");

  LOG_INDENT_SCOPE();
  for (auto child : children) child->dump_tree();
}

//------------------------------------------------------------------------------
#if 0
CInstCall::CInstCall(std::string name, CInstClass* inst_parent, CNodeCall* node_call, CNodeFunction* node_func)
    : CInstance(name, inst_parent, node_func), node_call(node_call) {
}

//----------------------------------------

void CInstCall::dump_tree() const {
  LOG_G("Call %s\n", name.c_str());

  //auto func_name = node_func->get_name();
  //auto class_name = inst_parent->get_name();
  //LOG_G("Call %.*s::%.*s\n", int(class_name.size()), class_name.data(), int(func_name.size()), func_name.data());

  //LOG_INDENT_SCOPE();
  //for (auto pair : call_map) pair.second->dump_tree();
}
#endif

//------------------------------------------------------------------------------
