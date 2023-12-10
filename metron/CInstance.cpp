#include "metron/CInstance.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeType.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeDeclaration.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodePunct.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeUnion.hpp"

//------------------------------------------------------------------------------

bool belongs_to_func(CInstance* inst) {
  if (inst->inst_parent == nullptr) return false;

  if (auto func = inst->inst_parent->as<CInstFunc>()) {
    return true;
  }
  else {
    return belongs_to_func(inst->inst_parent);
  }
}

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

//------------------------------------------------------------------------------

bool CInstance::is_sig() const {
  if (this->as<CInstClass>()) return false;
  if (this->as<CInstFunc>()) return false;
  return !is_reg();
}

//----------------------------------------

bool CInstance::is_reg() const {

  if (this->as<CInstClass>()) return false;
  if (this->as<CInstFunc>()) return false;

  const CInstance* inst = this;
  while (inst->inst_parent && !inst->inst_parent->as<CInstClass>()) {
    inst = inst->inst_parent;
  }

  return inst->name.ends_with("_");

  return false;
}

//----------------------------------------

bool CInstance::is_array() const {
  if (auto inst_prim = this->as<CInstPrim>()) {
    return inst_prim->node_field && inst_prim->node_field->is_array();
  }
  return false;
}

//----------------------------------------

bool CInstance::is_public() const {
  if (auto p = this->as<CInstPrim>())   return p->node_field && p->node_field->is_public;
  if (auto s = this->as<CInstStruct>()) return s->node_field && s->node_field->is_public;
  if (auto u = this->as<CInstUnion>())  return u->node_field && u->node_field->is_public;
  return false;
}

//------------------------------------------------------------------------------

CInstance* CInstance::resolve(CNode* node) {
  if (auto node_call = node->as<CNodeCall>()) {
    return resolve(node_call->node_args);
  }

  if (auto node_list = node->as<CNodeList>()) {
    assert(node_list->items.size() == 1);
    return resolve(node_list->items[0]);
  }

  if (node->as<CNodeLValue>()) {
    return resolve(node->name);
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

  if (auto node_prefix = node->as<CNodePrefixExp>()) {
    return resolve(node_prefix->node_rhs);
  }

  if (auto node_suffix = node->as<CNodeSuffixExp>()) {
    return resolve(node_suffix->node_lhs);
  }

  if (auto id = node->as<CNodeIdentifier>()) {
    auto name = id->get_textstr();
    if (auto inst = resolve(name)) return inst;
  }

  if (inst_parent) {
    return inst_parent->resolve(node);
  }

  //auto text = node->get_text();
  //LOG_R("Could not resolve %.*s\n", text.size(), text.data());

  return nullptr;
}

//------------------------------------------------------------------------------

CNodeClass* CInstance::get_owner() {
  return ancestor<CInstClass>()->node_class;
}

CNodeField* CInstance::get_field() {
  if (auto p = this->as<CInstPrim>())   return p->node_field;
  if (auto s = this->as<CInstStruct>()) return s->node_field;
  if (auto u = this->as<CInstUnion>())  return u->node_field;
  return nullptr;
}

// ##############################################################################
// ##############################################################################









































// ##############################################################################
// ##############################################################################

CInstClass* instantiate_class(
  CSourceRepo* repo,
  std::string name,
  CInstance* inst_parent,
  CNodeField* node_field,
  CNodeClass* node_class,
  int depth)
{
  auto inst_class = new CInstClass(name, inst_parent, node_field, node_class);

  bool in_public_section = false;

  for (auto child : node_class->node_body) {
    if (auto access = child->as<CNodeAccess>()) {
      in_public_section = child->get_text() == "public:";
      continue;
    }
    bool child_is_public = in_public_section && !child->tag_internal();

    if (auto node_field = child->as<CNodeField>()) {

      if (node_field->node_decl->is_param()) {
        continue;
      }

      auto field_name = node_field->name;

      auto type_class = repo->get_class(node_field->node_decl->node_type->name);
      auto type_struct = repo->get_struct(node_field->node_decl->node_type->name);
      auto type_union = repo->get_union(node_field->node_decl->node_type->name);

      if (type_class) {
        auto inst = instantiate_class(repo, field_name, inst_class, node_field, type_class, depth);
        inst_class->parts.push_back(inst);
      }
      else if (type_struct) {
        auto inst = new CInstStruct(field_name, inst_class, node_field, type_struct);
        if (child_is_public) {
          inst_class->ports.push_back(inst);
        } else {
          inst_class->parts.push_back(inst);
        }
      } else if (type_union) {
        auto inst = new CInstUnion(field_name, inst_class, node_field, type_union);
        if (child_is_public) {
          inst_class->ports.push_back(inst);
        } else {
          inst_class->parts.push_back(inst);
        }
      } else {
        auto inst = new CInstPrim(field_name, inst_class, node_field);
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

TraceState CInstClass::get_trace_state() const {
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

void CInstClass::push_trace_state() {
  for (auto child : ports) child->push_trace_state();
  for (auto child : parts) child->push_trace_state();
}

void CInstClass::pop_trace_state() {
  for (auto child : ports) child->pop_trace_state();
  for (auto child : parts) child->pop_trace_state();
}

void CInstClass::swap_trace_state() {
  for (auto child : ports) child->swap_trace_state();
  for (auto child : parts) child->swap_trace_state();
}

void CInstClass::merge_state() {
  for (auto child : ports) child->merge_state();
  for (auto child : parts) child->merge_state();
}

//------------------------------------------------------------------------------

bool CInstClass::check_port_directions(CInstClass* b) {
  auto a = this;
  LOG("Checking %s vs %s\n", a->path.c_str(), b->path.c_str());

  // Mismatched port sizes (how?) are invalid.
  assert(a->ports.size() == b->ports.size());
  if (a->ports.size() != b->ports.size()) {
    LOG_R("Mismatched port sizes\n");
    return false;
  }

  for (int i = 0; i < a->ports.size(); i++) {
    auto pa = a->ports[i];
    auto pb = b->ports[i];

    if (auto fa = pa->as<CInstFunc>()) {
      auto fb = pb->as<CInstFunc>();
      if (!fa->check_port_directions(fb)) {
        fa->check_port_directions(fb);
        return false;
      }
      continue;
    }

    if (!::check_port_directions(pa->get_trace_state(), pb->get_trace_state()))
      return false;
  }

  return true;
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

  auto repo = node_struct->get_repo();

  for (auto struct_field : node_struct->all_fields) {
    auto field_name = struct_field->name;

    if (auto type_struct = repo->get_struct(struct_field->node_decl->node_type->name)) {
      auto inst_field = new CInstStruct(field_name, this, struct_field, type_struct);
      parts.push_back(inst_field);
    }
        else if (auto type_union = repo->get_union(struct_field->node_decl->node_type->name)) {
      auto inst_field = new CInstUnion(field_name, this, struct_field, type_union);
      parts.push_back(inst_field);
    }
    else {
      auto inst_field = new CInstPrim(field_name, this, struct_field);
      parts.push_back(inst_field);
    }
  }
}

//------------------------------------------------------------------------------

TraceState CInstStruct::get_trace_state() const {
  auto merged_state = TS_PENDING;
  for (auto child : parts) {
    merged_state = merge_branch(merged_state, child->get_trace_state());
  }
  return merged_state;
}

//----------------------------------------

CInstance* CInstStruct::resolve(std::string name) {
  for (auto child : parts)
    if (child->name == name) return child;
  return nullptr;
}

//----------------------------------------

void CInstStruct::push_trace_state() {
  for (auto child : parts) child->push_trace_state();
}

void CInstStruct::pop_trace_state() {
  for (auto child : parts) child->pop_trace_state();
}

void CInstStruct::swap_trace_state() {
  for (auto child : parts) child->swap_trace_state();
}

void CInstStruct::merge_state() {
  for (auto child : parts) child->merge_state();
}

// ##############################################################################
// ##############################################################################





































// ##############################################################################
// ##############################################################################

CInstUnion::CInstUnion(std::string name, CInstance* inst_parent,
                       CNodeField* node_field, CNodeUnion* node_union)
    : CInstance(name, inst_parent),
      node_field(node_field),
      node_union(node_union) {
  assert(node_union);

  auto repo = node_union->get_repo();

  for (auto union_field : node_union->all_fields) {
    auto field_name = union_field->name;

    if (auto type_struct = repo->get_struct(union_field->node_decl->node_type->name)) {
      auto inst_field = new CInstStruct(field_name, this, union_field, type_struct);
      parts.push_back(inst_field);
    }
        else if (auto type_union = repo->get_union(union_field->node_decl->node_type->name)) {
      auto inst_field = new CInstUnion(field_name, this, union_field, type_union);
      parts.push_back(inst_field);
    }
    else {
      auto inst_field = new CInstPrim(field_name, this, union_field);
      parts.push_back(inst_field);
    }
  }
}

//------------------------------------------------------------------------------

TraceState CInstUnion::get_trace_state() const {
  auto merged_state = TS_PENDING;
  for (auto child : parts) {
    merged_state = merge_branch(merged_state, child->get_trace_state());
  }
  return merged_state;
}

//----------------------------------------

CInstance* CInstUnion::resolve(std::string name) {
  for (auto child : parts)
    if (child->name == name) return child;
  return nullptr;
}

//----------------------------------------

void CInstUnion::push_trace_state() {
  for (auto child : parts) child->push_trace_state();
}

void CInstUnion::pop_trace_state() {
  for (auto child : parts) child->pop_trace_state();
}

void CInstUnion::swap_trace_state() {
  for (auto child : parts) child->swap_trace_state();
}

void CInstUnion::merge_state() {
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

TraceState CInstPrim::get_trace_state() const { return state_stack.back(); }

//----------------------------------------

CInstance* CInstPrim::resolve(std::string name) {
  assert(false);
  return nullptr;
}

//----------------------------------------

void CInstPrim::push_trace_state() {
  assert(state_stack.size() > 0);
  state_stack.push_back(state_stack.back());
}

//----------------------------------------

void CInstPrim::pop_trace_state() {
  assert(state_stack.size() > 0);
  state_stack.pop_back();
}

//----------------------------------------

void CInstPrim::swap_trace_state() {
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
  auto repo = node_func->get_repo();

  for (auto param : node_func->params) {
    if (auto type_struct = repo->get_struct(param->node_type->name)) {
      auto inst_param = new CInstStruct(param->name, this, nullptr, type_struct);
      params.push_back(inst_param);
    }
    else if (auto type_union = repo->get_union(param->node_type->name)) {
      auto inst_param = new CInstUnion(param->name, this, nullptr, type_union);
      params.push_back(inst_param);
    }
    else {
      auto inst_param = new CInstPrim(param->name, this, nullptr);
      params.push_back(inst_param);
    }
  }
}

//----------------------------------------

bool CInstFunc::check_port_directions(CInstFunc* b) {
  auto a = this;
  assert(a->params.size() == b->params.size());

  bool ok = true;

  if (a->inst_return) {
    auto qa = a->inst_return->get_trace_state();
    auto qb = b->inst_return->get_trace_state();

    ok &= ::check_port_directions(a->inst_return->get_trace_state(),
                                  b->inst_return->get_trace_state());
  }

  for (int i = 0; i < a->params.size(); i++) {
    ok &= ::check_port_directions(a->params[i]->get_trace_state(),
                                  b->params[i]->get_trace_state());
  }

  return ok;
}

//----------------------------------------

TraceState CInstFunc::get_trace_state() const {
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

void CInstFunc::push_trace_state() {
  for (auto p : params) p->push_trace_state();
  if (inst_return) inst_return->push_trace_state();
}

void CInstFunc::pop_trace_state() {
  for (auto p : params) p->pop_trace_state();
  if (inst_return) inst_return->pop_trace_state();
}

void CInstFunc::swap_trace_state() {
  for (auto p : params) p->swap_trace_state();
  if (inst_return) inst_return->swap_trace_state();
}

void CInstFunc::merge_state() {
  for (auto p : params) p->merge_state();
  if (inst_return) inst_return->merge_state();
}

// ##############################################################################
// ##############################################################################
