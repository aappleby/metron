#include "MetronApp.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"

#include "metron/parser/CSourceFile.hpp"
#include "metron/parser/CSourceRepo.hpp"
#include "metron/parser/CParser.hpp"
#include "metron/parser/Cursor.hpp"
#include "metron/parser/NodeTypes.hpp"
#include "metron/parser/CNodeClass.hpp"
#include "metron/parser/CNodeStruct.hpp"
#include "metron/parser/CNodeField.hpp"
#include "metron/parser/CNodeFunction.hpp"
#include "metron/parser/CInstance.hpp"

#include "matcheroni/Utilities.hpp"

using namespace matcheroni;

//------------------------------------------------------------------------------

static std::vector<std::string> split_path(const std::string& input) {
  std::vector<std::string> result;
  std::string temp;

  const char* c = input.c_str();

  do {
    if (*c == '/' || *c == '\\' || *c == 0) {
      if (temp.size()) result.push_back(temp);
      temp.clear();
    } else {
      temp.push_back(*c);
    }
  } while (*c++ != 0);

  return result;
}

//------------------------------------------------------------------------------

static std::string join_path(std::vector<std::string>& path) {
  std::string result;
  for (auto& s : path) {
    if (result.size()) result += "/";
    result += s;
  }
  return result;
}

//------------------------------------------------------------------------------

static void mkdir_all(const std::vector<std::string>& full_path) {
  std::string temp;
  for (size_t i = 0; i < full_path.size(); i++) {
    if (temp.size()) temp += "/";
    temp += full_path[i];
    //printf("making dir %s\n", temp.c_str());
    int result = plat_mkdir(temp.c_str());
    //printf("mkdir result %d\n", result);
  }
}

//------------------------------------------------------------------------------

int main_new(Options opts) {
  LOG_G("New parser!\n");

  Err err;

  CSourceRepo repo;
  CSourceFile* root_file = nullptr;
  err << repo.load_source(opts.src_name, &root_file);

  if (!root_file) {
    LOG_R("Could not load %s\n", opts.src_name.c_str());
    return -1;
  }

  if (opts.verbose) {
    root_file->context.root_node->dump_tree();
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  LOG_B("Processing source files\n");

  {
    LOG_INDENT_SCOPE();

    LOG_B("collect_fields_and_methods\n");
    err << repo.collect_fields_and_methods();

    LOG_B("build_call_graphs\n");
    err << repo.build_call_graphs();
  }

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto c : repo.all_classes) {
    for (auto f : c->all_fields) {
      if (f->is_component()) {
        f->_type_class->refcount++; 
      }
    }
  }

  CNodeClass* top = nullptr;
  for (auto c : repo.all_classes) {
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

  LOG_V("Instance:\n");
  auto inst = new CInstClass(top);
  inst->dump();

  LOG_B("\n");

  if (opts.verbose) {
    repo.dump();
    LOG_G("\n");
  }


  //exit(0);



  //----------
  // Emit all modules.

  LOG_G("Converting %s to SystemVerilog\n", opts.src_name.c_str());

  std::string out_string;
  Cursor cursor(&repo, root_file, &out_string);
  cursor.echo = opts.echo && !opts.quiet;

  if (opts.echo) LOG_G("\n----------------------------------------\n");
  err << cursor.emit_everything();
  err << cursor.emit_trailing_whitespace();
  if (opts.echo) LOG_G("----------------------------------------\n\n");

  if (err.has_err()) {
    LOG_R("Error during code generation\n");
    //lib.teardown();
    return -1;
  }







  // Save translated source to output directory, if there is one.
  if (opts.dst_name.size()) {
    LOG_G("Saving %s\n", opts.dst_name.c_str());

    auto dst_path = split_path(opts.dst_name);
    dst_path.pop_back();
    mkdir_all(dst_path);

    FILE* out_file = fopen(opts.dst_name.c_str(), "wb");
    if (!out_file) {
      LOG_R("ERROR Could not open %s for output\n", opts.dst_name.c_str());
    } else {
      // Copy the BOM over if needed.
      if (root_file->use_utf8_bom) {
        uint8_t bom[3] = {239, 187, 191};
        fwrite(bom, 1, 3, out_file);
      }

      fwrite(out_string.data(), 1, out_string.size(), out_file);
      fclose(out_file);
    }
  }

  LOG("Done!\n");

  return 0;
}
