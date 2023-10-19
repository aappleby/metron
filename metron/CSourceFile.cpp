#include "metron/CSourceFile.hpp"

#include "metron/CSourceRepo.hpp"
#include "metrolib/core/Log.h"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeNamespace.hpp"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

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

      n->parent_class = node;
      n->parent_struct = nullptr;
      n->node_decl->_type_class = repo->get_class(n->node_decl->node_type->name);
      n->node_decl->_type_struct = repo->get_struct(n->node_decl->node_type->name);

      if (n->node_decl->is_param()) {
        node->all_params.push_back(n);
      } else {
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
      } else {
        node->all_functions.push_back(n);
      }

      // Hook up _type_struct on all struct params
      for (auto decl : n->params) {
        decl->_type_class = repo->get_class(decl->node_type->name);
        decl->_type_struct = repo->get_struct(decl->node_type->name);
      }
      continue;
    }
  }

  if (auto parent = node->node_parent->as<CNodeTemplate>()) {
    for (CNode* param : parent->node_params->items) {
      if (param->as<CNodeDeclaration>()) {
        node->all_modparams.push_back(param->as<CNodeDeclaration>());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CSourceFile::init(CSourceRepo* _repo, const std::string& _filename,
                      const std::string& _filepath,
                      const std::string& _source_code, bool _use_utf8_bom) {
  this->repo = _repo;
  this->filename = _filename;
  this->filepath = _filepath;
  this->use_utf8_bom = _use_utf8_bom;

  context.reset();
  context.repo = _repo;
  context.source = _source_code;

  LOG("Lexing %s\n", filename.c_str());
  lexer.lex(context.source, context.lexemes);

  LOG("Tokenizing %s\n", filename.c_str());
  for (auto& t : context.lexemes) {
    if (!t.is_gap()) {
      context.tokens.push_back(CToken(&t));
    }
  }

  LOG("Parsing %s\n", filepath.c_str());
  LOG_INDENT();
  auto tail = context.parse();
  LOG_DEDENT();

  if (tail.is_valid() && tail.is_empty() && context.root_node) {
    LOG("Parse OK\n");
  } else {
    LOG_R("could not parse %s\n", filepath.c_str());
    return ERR("Could not parse file");
  }

  LOG_B("Processing source file\n");

  for (auto n : context.root_node) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      node_class->repo = repo;
      node_class->file = this;
      all_classes.push_back(node_class);
    } else if (auto node_class = n->as<CNodeClass>()) {
      node_class->repo = repo;
      node_class->file = this;
      all_classes.push_back(node_class);
    } else if (auto node_struct = n->as<CNodeStruct>()) {
      all_structs.push_back(node_struct);
    } else if (auto node_namespace = n->as<CNodeNamespace>()) {
      all_namespaces.push_back(node_namespace);
    } else if (auto node_enum = n->as<CNodeEnum>()) {
      all_enums.push_back(node_enum);
    }
  }

  LOG_B("Collecting fields and methods\n");

  for (auto n : context.root_node) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_class = n->as<CNodeClass>()) {
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_struct = n->as<CNodeStruct>()) {
      node_struct->collect_fields_and_methods(repo);
    } else if (auto node_namespace = n->as<CNodeNamespace>()) {
      node_namespace->collect_fields_and_methods();
    } else if (auto node_enum = n->as<CNodeEnum>()) {
      // LOG_G("top level enum!!!!\n");
    }
  }

  //repo->source_map[filename] = this;
  repo->source_files.push_back(this);

  return Err();
}

//------------------------------------------------------------------------------
