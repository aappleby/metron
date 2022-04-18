#include "MtModLibrary.h"

#include <sys/stat.h>

#include "Log.h"
#include "metron_tools.h"
#include "MtModule.h"
#include "MtSourceFile.h"

#pragma warning(disable : 4996)

//------------------------------------------------------------------------------

MtModule* MtModLibrary::get_module(const std::string& name) {
  assert(sources_loaded);
  for (auto mod : modules) {
    if (mod->mod_name == name) return mod;
  }
  return nullptr;
}

MtSourceFile* MtModLibrary::get_source(const std::string& name) {
  for (auto s : source_files) {
    if (s->filename == name) return s;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

void MtModLibrary::teardown() {
  modules.clear();
  for (auto s : source_files) {
    delete s;
  }
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

CHECK_RETURN Err MtModLibrary::load_source(const char* filename, MtSourceFile*& out_source) {
  Err err;

  if (get_source(filename)) {
    return WARN("Duplicate filename %s\n", filename);
  }

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
      err << load_blob(filename, full_path, src_blob, use_utf8_bom);
      break;
    }
  }

  if (!found) {
    err << ERR("Couldn't find %s in path!", filename);
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::load_blob(const std::string& filename, const std::string& full_path, const std::string& src_blob, bool use_utf8_bom) {
  Err err;

  assert(!sources_loaded);
  auto source_file = new MtSourceFile();
  err << source_file->init(filename, full_path, src_blob);
  source_file->use_utf8_bom = use_utf8_bom;
  add_source(source_file);

  // Recurse through #includes

  std::vector<std::string> includes;

  source_file->root_node.visit_tree([&](MnNode child) {
    if (child.sym != sym_preproc_include) return;

    std::string filename = child.get_field(field_path).text();
    filename.erase(filename.begin());
    filename.pop_back();
    includes.push_back(filename);
  });

  for (const auto& file : includes) {
    if (file == "metron_tools.h") continue;

    if (!get_source(file)) {
      MtSourceFile* source = nullptr;
      err << load_source(file.c_str(), source);
    }

    source_file->includes.push_back(get_source(file));
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModLibrary::process_sources() {
  Err err;

  assert(!sources_loaded);
  assert(!sources_processed);

  sources_loaded = true;
  
  for (auto s : source_files) {
    for (auto m : s->modules) {
      modules.push_back(m);
    }
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  for (auto mod : modules) {
    err << mod->collect_modparams();
    err << mod->collect_fields_and_components();
    err << mod->collect_methods();
  }

  for (auto mod : modules) {
    err << mod->categorize_methods();
  }

  //----------------------------------------
  // Generate call tree / temporal check for toplevel modules

  for (auto m : modules) {
    err << m->trace();
  }

  //----------------------------------------
  // All modules have populated their fields, match up tick/tock calls with their
  // corresponding methods.

  for (auto mod : modules) {
    err << mod->categorize_fields();
    err << mod->build_port_map();
  }

  //----------------------------------------
  // Hook up child->parent module pointers

  for (auto m : modules) {
    for (auto s : m->all_components) {
      get_module(s->type_name())->parents.push_back(m);
    }
  }

  //----------------------------------------

  sources_processed = true;

  return err;
}

