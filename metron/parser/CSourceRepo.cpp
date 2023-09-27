#include "CSourceRepo.hpp"

#include "NodeTypes.hpp"
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

CNodeNamespace* CSourceRepo::get_namespace(std::string_view name) {
  for (auto c : all_namespaces) {
    auto namespace_name = c->get_name();
    if (namespace_name == name) return c;
  }
  return nullptr;
}

CNodeEnum* CSourceRepo::get_enum(std::string_view name) {
  for (auto c : all_enums) {
    auto enum_name = c->get_name();
    if (enum_name == name) return c;
  }
  return nullptr;
}

CInstance* CSourceRepo::get_instance(std::string name) {
  for (auto i : all_instances) {
    if (i->name == name) return i;
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

void CSourceRepo::dump() {
  {
    LOG_G("Search paths:\n");
    LOG_INDENT_SCOPE();
    for (auto s : search_paths) LOG_G("`%s`\n", s.c_str());
    LOG("\n");
  }

  {
    LOG_G("Source files:\n");
    LOG_INDENT_SCOPE();
    for (auto pair : source_map) LOG_L("`%s`\n", pair.first.c_str());
    LOG("\n");
  }

  {
    LOG_G("Classes:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_classes) {
      n->dump();
      LOG("\n");
    }
    if (all_classes.empty()) LOG("\n");
  }

  {
    LOG_G("Structs:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_structs) {
      n->dump();
      LOG("\n");
    }
    if (all_structs.empty()) LOG("\n");
  }

  {
    LOG_G("Namespaces:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_namespaces) {
      n->dump();
      LOG("\n");
    }
    if (all_namespaces.empty()) LOG("\n");
  }

  {
    LOG_G("Enums:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_enums) {
      n->dump();
      LOG("\n");
    }
    if (all_enums.empty()) LOG("\n");
  }
}

//------------------------------------------------------------------------------

#if 0
CNode* CSourceRepo::resolve(CNodeClass* parent, CNode* path) {
  if (!path) return nullptr;

  //----------

  if (path->get_text() == ".") {
    return resolve(parent, path->node_next);
  }

  if (auto field_path = path->as<CNodeFieldExpression>()) {
    return resolve(parent, field_path->child_head);
  }

  //----------

  if (auto scope_path = path->as<CNodeQualifiedIdentifier>()) {
    return resolve(parent, scope_path->child_head);
  }

  //----------

  if (auto id = path->as<CNodeIdentifier>()) {
    if (id->tag_is("field_path")) {
      auto field = parent->get_field(id->get_text())->as<CNodeField>();
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
      if (auto method = parent->get_function(id->get_text())) {
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
#endif
