#include "metron/CSourceRepo.hpp"

#include <functional>

#include "metron/CSourceFile.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeStruct.hpp"
#include "metron/nodes/CNodeUnion.hpp"
#include "metron/nodes/CNodeNamespace.hpp"
#include "metron/nodes/CNodeEnum.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

CSourceFile* CSourceRepo::get_file(std::string_view absolute_path) {
  for (auto s : source_files) {
    if (s->filepath == absolute_path) return s;
  }
  return nullptr;
}

CNodeClass* CSourceRepo::get_class(std::string_view name) {
  for (auto s : source_files) {
    for (auto c : s->all_classes) {
      if (c->name == name) return c;
    }
  }
  return nullptr;
}

CNodeStruct* CSourceRepo::get_struct(std::string_view name) {
  for (auto s : source_files) {
    for (auto c : s->all_structs) {
      if (c->name == name) return c;
    }
  }
  return nullptr;
}

CNodeUnion* CSourceRepo::get_union(std::string_view name) {
  for (auto s : source_files) {
    for (auto c : s->all_unions) {
      if (c->name == name) return c;
    }
  }
  return nullptr;
}

CNodeNamespace* CSourceRepo::get_namespace(std::string_view name) {
  for (auto s : source_files) {
    for (auto c : s->all_namespaces) {
      if (c->name == name) return c;
    }
  }
  return nullptr;
}

CNodeEnum* CSourceRepo::get_enum(std::string_view name) {
  for (auto s : source_files) {
    for (auto c : s->all_enums) {
      if (c->name == name) return c;
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_filename(const std::string& filename) {
  for (auto& path : search_paths) {
    auto joined_path = fs::path(path) / fs::path(filename);
    if (fs::is_regular_file(joined_path)) {
      return fs::canonical(joined_path);
    }
  }
  return "";
}

//------------------------------------------------------------------------------

Err CSourceRepo::load_source(std::string filename, CSourceFile** out_source) {
  Err err;

  std::string absolute_path = resolve_filename(filename);

  assert(!absolute_path.empty());

  if (auto f = get_file(absolute_path)) {
    if (out_source) *out_source = f;
    return Err();
  }

  fs::path absolute_dir(absolute_path);
  absolute_dir.remove_filename();
  std::string ads = absolute_dir;
  search_paths.insert(ads);

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
