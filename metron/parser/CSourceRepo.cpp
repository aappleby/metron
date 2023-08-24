#include "CSourceRepo.hpp"

#include "CNodeClass.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "CSourceFile.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"

#include <functional>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

CNodeClass* CSourceRepo::get_class(std::string_view name) {
  for (auto c : all_classes) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CNodeStruct* CSourceRepo::get_struct(std::string_view name) {
  for (auto c : all_structs) {
    auto struct_name = c->get_name();
    if (struct_name == name) return c;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_filename(const std::string& filename) {
  for (auto& path : search_paths) {
    std::string full_path = fs::absolute(path + filename);
    //LOG("%s exists? ", full_path.c_str());
    if (fs::is_regular_file(full_path)) {
      //LOG_G("YES\n");
      return full_path;
    } else {
      //LOG_R("NO\n");
    }
  }
  return "";
}

//------------------------------------------------------------------------------

Err CSourceRepo::load_source(std::string filename, CSourceFile** out_source) {
  std::string absolute_path = resolve_filename(filename);

  if (source_map.contains(absolute_path)) return Err();

  fs::path absolute_dir(absolute_path);
  absolute_dir.remove_filename();
  search_paths.push_back(absolute_dir);

  std::string src_blob = matcheroni::utils::read(absolute_path);

  bool use_utf8_bom = false;
  if (uint8_t(src_blob[0]) == 239 &&
      uint8_t(src_blob[1]) == 187 &&
      uint8_t(src_blob[2]) == 191) {
    use_utf8_bom = true;
    src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
  }

  //printf("%s\n", src_blob.c_str());

  auto source_file = new CSourceFile();

  (void)source_file->init(this, filename, absolute_path, src_blob, use_utf8_bom);

  if (out_source) *out_source = source_file;

  return Err();
}

//------------------------------------------------------------------------------

Err CSourceRepo::collect_fields_and_methods() {
  Err err;

  for (auto pair : source_map) {
    for (auto n : pair.second->context.root_node) {
      if (auto node_class = dynamic_cast<CNodeClass*>(n)) {
        all_classes.push_back(node_class);
        node_class->collect_fields_and_methods(this);
      }

      if (auto node_struct = dynamic_cast<CNodeStruct*>(n)) {
        all_structs.push_back(node_struct);
        node_struct->collect_fields_and_methods(this);
      }

    }
  }


  /*

  for (auto s : all_structs) {
    err << s->collect_fields();
  }
  */

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto c : all_classes) {
    for (auto f : c->all_fields) {
      if (f->is_component()) {
        f->_type_class->refcount++;
      }
    }
  }

  top = nullptr;
  for (auto c : all_classes) {
    if (c->refcount == 0) {
      if (top == nullptr) {
        top = c;
      }
      else {
        LOG_R("Multiple top modules!\n");
        exit(-1);
      }
    }
  }
  if (top == nullptr) {
    LOG_R("No top module?\n");
    exit(-1);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CSourceRepo::build_call_graphs() {
  Err err;

  for (auto mod : all_classes) {
    err << mod->build_call_graph(this);
  }

  return err;
}

//------------------------------------------------------------------------------

void CSourceRepo::categorize_methods() {
}


//------------------------------------------------------------------------------

void CSourceRepo::dump() {
  //std::vector<std::string> search_paths = {""};
  //std::map<std::string, CSourceFile*> source_map;

  LOG_B("//----------------------------------------\n");
  LOG_B("// Repo dump\n");

  for (auto s : search_paths) {
    LOG_G("Search path `%s`\n", s.c_str());
  }

  for (auto pair : source_map) {
    LOG_L("Source file `%s`\n", pair.first.c_str());
  }

  for (auto n : all_classes) {
    n->dump();
    //n->dump_tree(3);
    // auto name = n->get_name();
    //LOG_B("%.*s @ %p\n", name.size(), name.data(), n);

    /*
    if (n->node_parent && n->node_parent->is_a<CNodeTemplate>()) {
      LOG_B("CNodeClass has template parent\n");
    }
    */

  }

  for (auto n : all_structs) {
    n->dump();
  }

  LOG_B("//----------------------------------------\n");

  /*
  for (auto pair : source_map) {
    auto s = pair.second;
    printf("\n\n\n");
    s->dump();
  }
  */
}

//------------------------------------------------------------------------------

CNode* CSourceRepo::resolve(CNodeClass* parent, CNode* path) {
  if (!path) return nullptr;

  //----------

  if (auto field_path = path->as_a<CNodeFieldExpression>()) {
    return resolve(parent, field_path->child_head);
  }

  //----------

  if (auto scope_path = path->as_a<CNodeQualifiedIdentifier>()) {
    return resolve(parent, scope_path->child_head);
  }

  //----------

  if (auto id = path->as_a<CNodeIdentifier>()) {
    if (id->tag_is("field_path")) {
      auto field = parent->get_field(id->get_text())->as_a<CNodeField>();
      auto field_class = get_class(field->get_type_name());
      return resolve(field_class, id->node_next);
    }
    else if (id->tag_is("scope_path")) {
      assert(false);
      return nullptr;
    }
    else {
      if (auto field = parent->get_field(id->get_text())) {
        return field;
      }
      if (auto method = parent->get_method(id->get_text())) {
        return method;
      }
      assert(false);
      return nullptr;
    }
  }

  //----------

  assert(false && "Could not resolve function name");
  return nullptr;
}
