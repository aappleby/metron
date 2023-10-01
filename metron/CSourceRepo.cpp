#include "metron/CSourceRepo.hpp"

#include <functional>

#include "metron/CSourceFile.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"

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
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CNodeNamespace* CSourceRepo::get_namespace(std::string_view name) {
  for (auto c : all_namespaces) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CNodeEnum* CSourceRepo::get_enum(std::string_view name) {
  for (auto c : all_enums) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CInstClass* CSourceRepo::get_instance(std::string name) {
  for (auto i : all_instances) {
    if (i->name == name) return i;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_filename(const std::string& filename) {
  for (auto& path : search_paths) {
    std::string full_path = fs::absolute(path + filename);
    if (fs::is_regular_file(full_path)) {
      return full_path;
    }
  }
  return "";
}

//------------------------------------------------------------------------------

Err CSourceRepo::load_source(std::string filename, CSourceFile** out_source) {
  Err err;

  std::string absolute_path = resolve_filename(filename);

  if (source_map.contains(absolute_path)) return Err();

  fs::path absolute_dir(absolute_path);
  absolute_dir.remove_filename();
  search_paths.push_back(absolute_dir);

  std::string src_blob = matcheroni::utils::read(absolute_path);

  bool use_utf8_bom = false;
  if (uint8_t(src_blob[0]) == 239 && uint8_t(src_blob[1]) == 187 &&
      uint8_t(src_blob[2]) == 191) {
    use_utf8_bom = true;
    src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
  }

  auto source_file = new CSourceFile();

  err << source_file->init(this, filename, absolute_path, src_blob,
                           use_utf8_bom);

  if (out_source) *out_source = source_file;

  return err;
}

//------------------------------------------------------------------------------
