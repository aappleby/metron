#include "CSourceRepo.hpp"

#include "CSourceFile.hpp"
#include "matcheroni/Utilities.hpp"

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_path(const std::string& filename) {
  for (auto& path : search_paths) {
    std::string full_path = fs::absolute(path + filename);
    printf("%s exists? ", full_path.c_str());
    if (fs::is_regular_file(full_path)) {
      printf("YES\n");
      return full_path;
    } else {
      printf("NO\n");
    }
  }
  return "";
}

//------------------------------------------------------------------------------

Err CSourceRepo::load_source(std::string filename) {
  std::string absolute_path = resolve_path(filename);

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

  return Err();
}

//------------------------------------------------------------------------------
