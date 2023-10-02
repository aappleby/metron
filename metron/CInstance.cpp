#include "metron/CInstance.hpp"

#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeStruct.hpp"

bool check_port_directions(TraceState sa, TraceState sb) {
  assert(sa >= TS_NONE || sa <= TS_REGISTER);
  assert(sa >= TS_NONE || sb <= TS_REGISTER);

  bool a_drives = (sa != TS_NONE) && (sa != TS_INPUT);
  bool b_drives = (sb != TS_NONE) && (sb != TS_INPUT);

  // Floating inputs are invalid.
  if (!a_drives && sb == TS_INPUT) return false;
  if (!b_drives && sa == TS_INPUT) return false;

  // Port collisions are invalid.
  if (a_drives && b_drives) return false;

  return true;
}

// #############################################################################
// #############################################################################

CInstance::CInstance(std::string name, CInstance* inst_parent)
    : name(name), inst_parent(inst_parent) {
  path = inst_parent ? inst_parent->name + "." + name : name;
}

CInstance::~CInstance() {}

//----------------------------------------

CInstance* CInstance::resolve(CNode* node) {
  if (node->as<CNodeLValue>()) {
    return resolve(node->child("name"));
  }

  if (node->as<CNodeFieldExpression>()) {
    auto cursor = node->child_head;
    CInstance* inst = this;
    while (cursor) {
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

  return nullptr;
}

// ##############################################################################
// ##############################################################################

// ##############################################################################
// ##############################################################################

CInstClass* instantiate_class(std::string name, CInstance* inst_parent,
                              CNodeField* node_field, CNodeClass* node_class,
                              int depth) {
  auto inst_class = new CInstClass(name, inst_parent, node_field, node_class);

  bool child_is_public = false;

  for (auto child : node_class->child("body")) {
    if (auto access = child->as<CNodeAccess>()) {
      child_is_public = child->get_text() == "public:";
      continue;
    }

    if (auto node_field = child->as<CNodeField>()) {
      auto field_name = node_field->name;

      if (node_field->node_decl->_type_class) {
        auto inst =
            instantiate_class(field_name, inst_class, node_field,
                              node_field->node_decl->_type_class, depth);
        // inst_class->children.push_back(inst);
        inst_class->parts.push_back(inst);
      } else if (node_field->node_decl->_type_struct) {
        auto inst = new CInstStruct(field_name, inst_class, node_field,
                                    node_field->node_decl->_type_struct);
        // inst_class->children.push_back(inst);

        if (child_is_public) {
          inst_class->ports.push_back(inst);
        } else {
          inst_class->parts.push_back(inst);
        }

      } else {
        auto inst = new CInstPrim(field_name, inst_class, node_field);
        // inst_class->children.push_back(inst);

        if (child_is_public) {
          inst_class->ports.push_back(inst);
        } else {
          inst_class->parts.push_back(inst);
        }
      }
    }

    if (auto node_func = child->as<CNodeFunction>()) {
      auto func_name = node_func->name;

      auto inst_func = new CInstFunc(func_name, inst_class, node_func);

      if (child_is_public) {
        inst_class->ports.push_back(inst_func);
      } else {
        inst_class->parts.push_back(inst_func);
      }
    }
  }

  return inst_class;
}

//------------------------------------------------------------------------------

CInstClass::CInstClass(std::string name, CInstance* inst_parent,
                       CNodeField* node_field, CNodeClass* node_class)
    : CInstance(name, inst_parent),
      node_field(node_field),
      node_class(node_class) {}

//------------------------------------------------------------------------------

bool CInstClass::check_port_directions(CInstClass* b) {
  auto a = this;
  LOG("Checking %s vs %s\n", a->path.c_str(), b->path.c_str());

  // Mismatched port sizes (how?) are invalid.
  assert(a->ports.size() == b->ports.size());
  if (a->ports.size() != b->ports.size()) return false;

  for (int i = 0; i < a->ports.size(); i++) {
    auto pa = a->ports[i];
    auto pb = b->ports[i];

    if (auto fa = pa->as<CInstFunc>()) {
      auto fb = pb->as<CInstFunc>();
      if (!fa->check_port_directions(fb)) return false;
      continue;
    }

    if (!::check_port_directions(pa->get_state(), pb->get_state()))
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------

TraceState CInstClass::get_state() const {
  assert(false);
  return TS_INVALID;
}

//----------------------------------------

CInstance* CInstClass::resolve(std::string name) {
  for (auto child : ports)
    if (child->name == name) return child;
  for (auto child : parts)
    if (child->name == name) return child;
  return inst_parent ? inst_parent->resolve(name) : nullptr;
}

//----------------------------------------

void CInstClass::push_state() {
  for (auto child : ports) child->push_state();
  for (auto child : parts) child->push_state();
}

void CInstClass::pop_state() {
  for (auto child : ports) child->pop_state();
  for (auto child : parts) child->pop_state();
}

void CInstClass::swap_state() {
  for (auto child : ports) child->swap_state();
  for (auto child : parts) child->swap_state();
}

void CInstClass::merge_state() {
  for (auto child : ports) child->merge_state();
  for (auto child : parts) child->merge_state();
}

// ##############################################################################
// ##############################################################################

// ##############################################################################
// ##############################################################################

CInstStruct::CInstStruct(std::string name, CInstance* inst_parent,
                         CNodeField* node_field, CNodeStruct* node_struct)
    : CInstance(name, inst_parent),
      node_field(node_field),
      node_struct(node_struct) {
  assert(node_struct);

  for (auto struct_field : node_struct->all_fields) {
    auto field_name = struct_field->name;

    if (struct_field->node_decl->_type_struct) {
      auto inst_field = new CInstStruct(field_name, this, struct_field,
                                        struct_field->node_decl->_type_struct);
      parts.push_back(inst_field);
    } else {
      auto inst_field = new CInstPrim(field_name, this, struct_field);
      parts.push_back(inst_field);
    }
  }
}

//------------------------------------------------------------------------------

TraceState CInstStruct::get_state() const {
  auto merged_state = TS_PENDING;
  for (auto child : parts) {
    merged_state = merge_branch(merged_state, child->get_state());
  }
  return merged_state;
}

//----------------------------------------

CInstance* CInstStruct::resolve(std::string name) {
  for (auto child : parts)
    if (child->name == name) return child;
  return inst_parent ? inst_parent->resolve(name) : nullptr;
}

//----------------------------------------

void CInstStruct::push_state() {
  for (auto child : parts) child->push_state();
}

void CInstStruct::pop_state() {
  for (auto child : parts) child->pop_state();
}

void CInstStruct::swap_state() {
  for (auto child : parts) child->swap_state();
}

void CInstStruct::merge_state() {
  for (auto child : parts) child->merge_state();
}

// ##############################################################################
// ##############################################################################

// ##############################################################################
// ##############################################################################

CInstPrim::CInstPrim(std::string name, CInstance* inst_parent,
                     CNodeField* node_field)
    : CInstance(name, inst_parent), node_field(node_field) {
  state_stack.push_back(TS_NONE);
}

//----------------------------------------

TraceState CInstPrim::get_state() const { return state_stack.back(); }

//----------------------------------------

CInstance* CInstPrim::resolve(std::string name) {
  assert(false);
  return nullptr;
}

//----------------------------------------

void CInstPrim::push_state() {
  assert(state_stack.size() > 0);
  state_stack.push_back(state_stack.back());
}

//----------------------------------------

void CInstPrim::pop_state() {
  assert(state_stack.size() > 0);
  state_stack.pop_back();
}

//----------------------------------------

void CInstPrim::swap_state() {
  assert(state_stack.size() >= 2);

  auto s = state_stack.size();
  auto a = state_stack[s - 2];
  auto b = state_stack[s - 1];

  state_stack[s - 2] = b;
  state_stack[s - 1] = a;
}

//----------------------------------------

void CInstPrim::merge_state() {
  assert(state_stack.size() >= 2);
  auto s = state_stack.size();
  auto a = state_stack[s - 2];
  auto b = state_stack[s - 1];

  state_stack[s - 2] = merge_branch(a, b);
  state_stack.pop_back();
}

// ##############################################################################
// ##############################################################################











// ##############################################################################
// ##############################################################################

CInstFunc::CInstFunc(std::string name, CInstance* inst_parent,
                     CNodeFunction* node_func)
    : CInstance(name, inst_parent), node_func(node_func) {
  auto repo = node_func->ancestor<CNodeClass>()->repo;

  for (auto param : node_func->params) {
    auto param_name = param->name;
    if (param->_type_struct) {
      auto inst_field =
          new CInstStruct(param_name, this, nullptr, param->_type_struct);
      params.push_back(inst_field);
    } else {
      auto inst_field = new CInstPrim(param_name, this, nullptr);
      params.push_back(inst_field);
    }
  }

  auto ret_type = node_func->child("return_type");
  if (ret_type && ret_type->get_text() != "void") {
    if (auto struct_type = ret_type->as<CNodeStructType>()) {
      auto node_struct = repo->get_struct(struct_type->get_text());
      auto inst = new CInstStruct("@return", this, nullptr, node_struct);
      inst_return = inst;
    } else if (auto builtin_type = ret_type->as<CNodeBuiltinType>()) {
      auto inst = new CInstPrim("@return", this, nullptr);
      inst_return = inst;
    } else {
      assert(false);
    }
  }
}

//----------------------------------------

bool CInstFunc::check_port_directions(CInstFunc* b) {
  auto a = this;
  assert(a->params.size() == b->params.size());

  bool ok = true;

  if (a->inst_return) {
    ok &= ::check_port_directions(a->inst_return->get_state(),
                                  b->inst_return->get_state());
  }

  for (int i = 0; i < a->params.size(); i++) {
    ok &= ::check_port_directions(a->params[i]->get_state(),
                                  b->params[i]->get_state());
  }

  return ok;
}

//----------------------------------------

TraceState CInstFunc::get_state() const {
  assert(false);
  return TS_INVALID;
}

//----------------------------------------

CInstance* CInstFunc::resolve(std::string name) {
  for (auto param : params)
    if (param->name == name) return param;
  if (inst_return && inst_return->name == name) return inst_return;
  return inst_parent->resolve(name);
}

//----------------------------------------

void CInstFunc::push_state() {
  for (auto p : params) p->push_state();
  if (inst_return) inst_return->push_state();
}

void CInstFunc::pop_state() {
  for (auto p : params) p->pop_state();
  if (inst_return) inst_return->pop_state();
}

void CInstFunc::swap_state() {
  for (auto p : params) p->swap_state();
  if (inst_return) inst_return->swap_state();
}

void CInstFunc::merge_state() {
  for (auto p : params) p->merge_state();
  if (inst_return) inst_return->merge_state();
}

// ##############################################################################
// ##############################################################################
