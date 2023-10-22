#include "metron/CSourceFile.hpp"

#include "metron/CSourceRepo.hpp"
#include "metrolib/core/Log.h"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

Err collect_fields_and_methods(CNodeClass* node, CSourceRepo* repo) {
  Err err;

  bool in_public_section = false;

  for (auto child : node->node_body->items) {
    if (auto access = child->as<CNodeAccess>()) {
      in_public_section = child->get_text() == "public:";
      continue;
    }

    bool child_is_public = in_public_section && !child->tag_internal();

    if (auto n = child->as<CNodeField>()) {
      n->is_public = child_is_public;

      //n->parent_class = node;
      //n->parent_struct = nullptr;
      //n->node_decl->_type_class  = repo->get_class(n->node_decl->node_type->name);

      //printf("%s = %p\n", n->node_decl->node_type->name.c_str(), n->node_decl->_type_class);

      if (n->node_decl->is_param()) {
        node->all_localparams.push_back(n);
      } else {
        node->all_fields.push_back(n);
      }

      continue;
    }

    if (auto n = child->as<CNodeFunction>()) {
      n->is_public = child_is_public;

      if (auto constructor = child->as<CNodeConstructor>()) {
        assert(node->constructor == nullptr);
        node->constructor = constructor;
        constructor->method_type = MT_INIT;
      } else {
        node->all_functions.push_back(n);
      }

      // Hook up _type_struct on all struct params
      //for (auto decl : n->params) {
        //decl->_type_class = repo->get_class(decl->node_type->name);
        //decl->_type_struct = repo->get_struct(decl->node_type->name);
      //}
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

  translation_unit = context.root_node->as<CNodeTranslationUnit>();
  translation_unit->file = this;

  if (tail.is_valid() && tail.is_empty() && translation_unit) {
    LOG("Parse OK\n");
  } else {
    LOG_R("could not parse %s\n", filepath.c_str());
    return ERR("Could not parse file");
  }

  //repo->source_map[filename] = this;
  repo->source_files.push_back(this);

  return Err();
}

//------------------------------------------------------------------------------

void CSourceFile::link() {
  LOG_B("Processing source file\n");

  for (auto n : translation_unit) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      all_classes.push_back(node_class);
    } else if (auto node_class = n->as<CNodeClass>()) {
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

  for (auto n : translation_unit) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_class = n->as<CNodeClass>()) {
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_struct = n->as<CNodeStruct>()) {
      //node_struct->collect_fields_and_methods(repo);
    } else if (auto node_namespace = n->as<CNodeNamespace>()) {
      node_namespace->collect_fields_and_methods();
    } else if (auto node_enum = n->as<CNodeEnum>()) {
      // LOG_G("top level enum!!!!\n");
    }
  }
}

//------------------------------------------------------------------------------
