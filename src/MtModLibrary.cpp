#include "MtModLibrary.h"

#include <sys/stat.h>

#include "metron_tools.h"
#include "MtModule.h"
#include "MtSourceFile.h"

#pragma warning(disable : 4996)

//------------------------------------------------------------------------------

void MtModLibrary::reset() {
  for (auto module : modules) delete module;
  modules.clear();
}

//------------------------------------------------------------------------------

void MtModLibrary::add_search_path(const std::string& path) {
  assert(!sources_loaded);
  search_paths.push_back(path);
}

//------------------------------------------------------------------------------

void MtModLibrary::add_source(MtSourceFile* source_file) {
  assert(!sources_loaded);
  source_file->lib = this;
  source_files.push_back(source_file);
}

//------------------------------------------------------------------------------

MtSourceFile* MtModLibrary::find_source(const std::string& filename) {
  for (auto& s : source_files) {
    if (s->filename == filename) {
      return s;
    }
  }
  return nullptr;
}

//------------------------------------------------------------------------------

bool MtModLibrary::load_source(const char* filename) {
  bool error = false;

  assert(!sources_loaded);
  bool found = false;
  for (auto& path : search_paths) {
    auto full_path = path.size() ? path + "/" + filename : filename;
    struct stat s;
    auto stat_result = stat(full_path.c_str(), &s);
    if (stat_result == 0) {
      found = true;
      LOG_B("Loading %s from %s\n", filename, full_path.c_str());
      LOG_INDENT_SCOPE();

      std::string src_blob;
      src_blob.resize(s.st_size);

      auto f = fopen(full_path.c_str(), "rb");
      fread(src_blob.data(), 1, src_blob.size(), f);
      fclose(f);

      bool use_utf8_bom = false;
      if (uint8_t(src_blob[0]) == 239 && uint8_t(src_blob[1]) == 187 &&
          uint8_t(src_blob[2]) == 191) {
        use_utf8_bom = true;
        src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
      }
      load_blob(filename, full_path, src_blob, use_utf8_bom);
      break;
    }
  }

  if (!found) {
    LOG_R("Couldn't find %s in path!\n", filename);
    error = true;
  }

  return error;
}

//------------------------------------------------------------------------------

void MtModLibrary::load_blob(const std::string& filename,
                             const std::string& full_path,
                             const std::string& src_blob, bool use_utf8_bom) {
  assert(!sources_loaded);
  auto source_file = new MtSourceFile(filename, full_path, src_blob);
  source_file->use_utf8_bom = use_utf8_bom;
  add_source(source_file);
}

//------------------------------------------------------------------------------

bool MtModLibrary::process_sources() {
  bool error = false;

  assert(!sources_loaded);
  assert(!sources_processed);

  sources_loaded = true;
  
  for (auto s : source_files) {
    for (auto m : s->modules) {
      modules.push_back(m);
    }
  }

  for (auto mod : modules) {
    error |= mod->load_pass1();
  }

  if (error) {
    LOG_R("process_sources pass 1 failed\n");
    return error;
  }

  for (auto mod : modules) {
    error |= mod->load_pass2();
  }

  if (error) {
    LOG_R("process_sources pass 2 failed\n");
    return error;
  }

  // Hook up child->parent module pointers
  for (auto m : modules) {
    for (auto s : m->submods) {
      get_module(s->type_name())->parents.push_back(m);
    }
  }

  // Generate call tree / temporal check for toplevel modules
  for (auto m : modules) {
    error |= m->trace();
  }

  if (error) {
    LOG_R("Some modules fail temporal trace!\n");
  }
  else {
    LOG_G("All modules pass temporal trace!\n");
  }
  sources_processed = true;

  return error;
}

//------------------------------------------------------------------------------

MtModule* MtModLibrary::get_module(const std::string& name) {
  assert(sources_loaded);
  for (auto mod : modules) {
    if (mod->mod_name == name) return mod;
  }
  return nullptr;
}

bool MtModLibrary::has_module(const std::string& name) {
  assert(sources_loaded);
  return get_module(name) != nullptr;
}

//------------------------------------------------------------------------------
