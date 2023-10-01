#include "CNodeClass.hpp"

#include "metron/Emitter.hpp"

#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeCall.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "metron/nodes/CNodeExpression.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFieldExpression.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeIdentifier.hpp"
#include "metron/nodes/CNodeLValue.hpp"
#include "metron/nodes/CNodeTemplate.hpp"

//==============================================================================

void CNodeClass::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);

  node_class = child("class")->as<CNodeKeyword>();
  node_name  = child("name")->as<CNodeIdentifier>();
  node_body  = child("body")->as<CNodeList>();

  name = node_name->get_namestr();

  for (auto child : node_body->items) {
    if (auto node_enum = child->as<CNodeEnum>()) {
      all_enums2.push_back(node_enum);
    }
  }
}

uint32_t CNodeClass::debug_color() const {
  return 0x00FF00;
}

std::string_view CNodeClass::get_name() const {
  return name;
}

CNodeField* CNodeClass::get_field(CNode* node_name) {
  if (node_name == nullptr) return nullptr;

  if (node_name->as<CNodeLValue>()) {
    return get_field(node_name->child("name"));
  }

  if (auto node_id = node_name->as<CNodeIdentifier>()) {
    return get_field(node_id->get_text());
  }

  if (auto node_field = node_name->as<CNodeFieldExpression>()) {
    return get_field(node_field->child("field_path"));
  }

  if (auto node_prefix = node_name->as<CNodeSuffixExp>()) {
    return get_field(node_prefix->child("rhs"));
  }

  if (auto node_suffix = node_name->as<CNodeSuffixExp>()) {
    return get_field(node_suffix->child("lhs"));
  }

  LOG_R("----------------------------------------\n");
  LOG_R("Don't know how to get field for %s\n", node_name->get_textstr().c_str());
  LOG_R("----------------------------------------\n");

  assert(false);
  return nullptr;
}

CNodeField* CNodeClass::get_field(std::string_view name) {
  for (auto f : all_fields) if (f->get_name() == name) return f;
  return nullptr;
}

CNodeFunction* CNodeClass::get_function(std::string_view name) {
  for (auto m : all_functions) if (m->get_name() == name) return m;
  return nullptr;
}

CNodeDeclaration* CNodeClass::get_modparam(std::string_view name) {
  for (auto p : all_modparams) if (p->get_name() == name) return p;
  return nullptr;
}

CNodeEnum* CNodeClass::get_enum(std::string_view name) {
  for (auto e : all_enums2) if (e->get_name() == name) return e;
  return nullptr;
}

//------------------------------------------------------------------------------
// FIXME move this to init()

Err collect_fields_and_methods(CNodeClass* node, CSourceRepo* repo) {
  Err err;

  bool is_public = false;

  for (auto child : node->node_body->items) {

    if (auto access = child->as<CNodeAccess>()) {
      is_public = child->get_text() == "public:";
      continue;
    }

    if (auto n = child->as<CNodeField>()) {
      n->is_public = is_public;

      n->parent_class  = node;
      n->parent_struct = nullptr;
      n->node_decl->_type_class    = repo->get_class(n->get_type_name());
      n->node_decl->_type_struct   = repo->get_struct(n->get_type_name());

      if (n->node_decl->node_static && n->node_decl->node_const) {
        node->all_localparams.push_back(n);
      }
      else {
        node->all_fields.push_back(n);
      }

      continue;
    }

    if (auto n = child->as<CNodeFunction>()) {
      n->is_public = is_public;

      if (auto constructor = child->as<CNodeConstructor>()) {
        assert(node->constructor == nullptr);
        node->constructor = constructor;
        constructor->method_type = MT_INIT;
      }
      else {
        node->all_functions.push_back(n);
      }

      // Hook up _type_struct on all struct params
      for (auto decl : n->params) {
        decl->_type_class = repo->get_class(decl->get_type_name());
        decl->_type_struct = repo->get_struct(decl->get_type_name());
      }
      continue;
    }

  }

  if (auto parent = node->node_parent->as<CNodeTemplate>()) {
    for (CNode*  param : parent->node_params->items) {
      if (param->as<CNodeDeclaration>()) {
        node->all_modparams.push_back(param->as<CNodeDeclaration>());
      }
    }
  }

  return err;
}

Err CNodeClass::collect_fields_and_methods() {
  return ::collect_fields_and_methods(this, repo);
}

//------------------------------------------------------------------------------

Err build_call_graph(CNodeClass* node, CSourceRepo* repo) {
  Err err;

  node_visitor link_callers = [&](CNode* child) {
    auto call = child->as<CNodeCall>();
    if (!call) return;

    auto src_method = call->ancestor<CNodeFunction>();

    auto func_name = call->child("func_name");

    if (auto submod_path = func_name->as<CNodeFieldExpression>()) {
      auto submod_field = node->get_field(submod_path->child("field_path")->get_text());
      auto submod_class = repo->get_class(submod_field->get_type_name());
      auto submod_func  = submod_class->get_function(submod_path->child("identifier")->get_text());

      src_method->external_callees.insert(submod_func);
      submod_func->external_callers.insert(src_method);
    }
    else if (auto func_id = func_name->as<CNodeIdentifier>()) {
      auto dst_method = node->get_function(func_id->get_text());
      if (dst_method) {
        src_method->internal_callees.insert(dst_method);
        dst_method->internal_callers.insert(src_method);
      }
    }
    else {
      assert(false);
    }
  };

  if (node->constructor) {
    visit_children(node->constructor, link_callers);
  }

  for (auto src_method : node->all_functions) {
    visit_children(src_method, link_callers);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::build_call_graph(CSourceRepo* repo) {
  return ::build_call_graph(this, repo);
}

//------------------------------------------------------------------------------

bool CNodeClass::needs_tick() {
  for (auto f : all_functions) {
    if (f->method_type == MT_TICK) return true;
  }

  for (auto f : all_fields) {
    if (f->node_decl->_type_class && f->node_decl->_type_class->needs_tick()) return true;
  }

  return false;
}

//----------------------------------------

bool CNodeClass::needs_tock() {
  for (auto f : all_functions) {
    if (f->method_type == MT_TOCK) return true;
  }

  for (auto f : all_fields) {
    if (f->node_decl->_type_class && f->node_decl->_type_class->needs_tock()) return true;
  }

  return false;
}

//==============================================================================
