#include "metron/CSourceFile.hpp"

#include "metron/CSourceRepo.hpp"
#include "metrolib/core/Log.h"
#include "metron/nodes/CNodeAccess.hpp"
#include "metron/nodes/CNodeTemplate.hpp"
#include "metron/nodes/CNodeConstructor.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeUnion.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeTranslationUnit.hpp"

namespace fs = std::filesystem;

// Dep file format:

/*
aappleby@lappytron:~/blah$ cat a-main.d
main.o: main.cpp /usr/include/stdc-predef.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/libc-header-start.h \
 /usr/include/features.h /usr/include/features-time64.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/bits/timesize.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/long-double.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/13/include/stddef.h \
 /usr/lib/gcc/x86_64-linux-gnu/13/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h \
 /usr/include/x86_64-linux-gnu/bits/time64.h \
 /usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__FILE.h \
 /usr/include/x86_64-linux-gnu/bits/types/FILE.h \
 /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h \
 /usr/include/x86_64-linux-gnu/bits/types/cookie_io_functions_t.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/floatn.h \
 /usr/include/x86_64-linux-gnu/bits/floatn-common.h
aappleby@lappytron:~/blah$ ./run
*/

//------------------------------------------------------------------------------

Err collect_fields_and_methods(CNodeClass* node, CSourceRepo* repo) {
  Err err;

  bool in_public_section = false;

  for (auto child : node->node_body->items) {
    if (child->tag_noconvert()) continue;

    if (auto access = child->as<CNodeAccess>()) {
      in_public_section = child->get_text() == "public:";
      continue;
    }

    bool child_is_public = in_public_section && !child->tag_internal();

    if (auto n = child->as<CNodeField>()) {
      n->is_public = child_is_public;

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
      } else {
        node->all_functions.push_back(n);
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
  context.source_file = this;

  if (global_options.verbose) LOG("Lexing %s\n", filename.c_str());
  lexer.lex(context.source, context.lexemes);

  //return Err();

  if (global_options.verbose) LOG("Tokenizing %s\n", filename.c_str());
  for (auto& t : context.lexemes) {
    if (!t.is_gap()) {
      context.tokens.push_back(CToken(&t));
    }
  }

  context.span.begin = &context.tokens[0];
  context.span.end   = context.span.begin + context.tokens.size();

  if (global_options.verbose) LOG("Parsing %s\n", filepath.c_str());
  LOG_INDENT();
  auto tail = context.parse();
  LOG_DEDENT();

  translation_unit = context.root_node->as<CNodeTranslationUnit>();
  translation_unit->file = this;

  if (tail.is_valid() && tail.is_empty() && translation_unit) {
    if (global_options.verbose) LOG("Parse OK\n");
  } else {
    LOG_R("could not parse %s\n", filepath.c_str());
    return ERR("Could not parse file");
  }

  repo->source_files.push_back(this);

  return Err();
}

//------------------------------------------------------------------------------

void CSourceFile::link() {
  if (global_options.verbose) LOG_B("Processing source file %s\n", filename.c_str());

  for (auto n : translation_unit) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      all_classes.push_back(node_class);
    } else if (auto node_class = n->as<CNodeClass>()) {
      all_classes.push_back(node_class);
    } else if (auto node_struct = n->as<CNodeStruct>()) {
      all_structs.push_back(node_struct);
    } else if (auto node_union = n->as<CNodeUnion>()) {
      all_unions.push_back(node_union);
    } else if (auto node_namespace = n->as<CNodeNamespace>()) {
      all_namespaces.push_back(node_namespace);
    } else if (auto node_enum = n->as<CNodeEnum>()) {
      all_enums.push_back(node_enum);
    }
  }

  if (global_options.verbose) LOG_B("Collecting fields and methods for %s\n", filename.c_str());

  for (auto n : translation_unit) {
    if (auto node_template = n->as<CNodeTemplate>()) {
      auto node_class = node_template->child<CNodeClass>();
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_class = n->as<CNodeClass>()) {
      collect_fields_and_methods(node_class, repo);
    } else if (auto node_struct = n->as<CNodeStruct>()) {
      //node_struct->collect_fields_and_methods(repo);
    } else if (auto node_union = n->as<CNodeUnion>()) {
      //node_struct->collect_fields_and_methods(repo);
    } else if (auto node_namespace = n->as<CNodeNamespace>()) {
      node_namespace->collect_fields_and_methods();
    } else if (auto node_enum = n->as<CNodeEnum>()) {
      // LOG_G("top level enum!!!!\n");
    }
  }
}

//------------------------------------------------------------------------------
