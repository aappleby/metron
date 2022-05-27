#include "Log.h"
#include "MtContext.h"
#include "MtCursor.h"
#include "MtField.h"
#include "MtMethod.h"
#include "MtStruct.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "submodules/CLI11/include/CLI/App.hpp"
#include "submodules/CLI11/include/CLI/Config.hpp"
#include "submodules/CLI11/include/CLI/Formatter.hpp"

#include <dirent.h>
#include <stdio.h>

//#include "../scratch.h"

#pragma warning(disable : 4996)

//------------------------------------------------------------------------------

std::vector<std::string> split_path(const std::string& input) {
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

void mkdir_all(const std::vector<std::string>& full_path) {
  std::string temp;
  for (size_t i = 0; i < full_path.size() - 1; i++) {
    if (temp.size()) temp += "/";
    temp += full_path[i];
    //printf("making dir %s\n", temp.c_str());
    plat_mkdir(temp.c_str());
  }
}

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  TinyLog::get().reset();

  const char* banner =
      "                                                        \n"
      " ###    ### ####### ######## ######   ######  ###    ## \n"
      " ####  #### ##         ##    ##   ## ##    ## ####   ## \n"
      " ## #### ## #####      ##    ######  ##    ## ## ##  ## \n"
      " ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## \n"
      " ##      ## #######    ##    ##   ##  ######  ##   #### \n"
      "                                                        \n"
      "            a C++ to SystemVerilog Translator           \n";

  CLI::App app{banner};

  bool quiet = false;
  bool monochrome = false;
  bool echo = false;
  bool dump = false;
  bool save = false;
  bool verbose = false;
  std::string src_root;
  std::string out_root;
  std::vector<std::string> source_names;

  // clang-format off
  app.add_flag  ("-q,--quiet",      quiet,        "Quiet mode");
  app.add_flag  ("-m,--monochrome", monochrome,   "Monochrome mode, no color-coding");
  app.add_flag  ("-v,--verbose",    verbose,      "Print detailed stats about the source modules.");
  app.add_flag  ("-s,--save",       save,         "Save converted source. If not specified, will only check inputs for convertibility.");
  app.add_flag  ("-e,--echo",       echo,         "Echo the converted source back to the terminal, with color-coding.");
  app.add_flag  ("--dump",          dump,         "Dump the syntax tree of the source file(s) to the console.");
  app.add_option("-r,--src_root",   src_root,     "Root directory of the source to convert");
  app.add_option("-o,--out_root",   out_root,     "Root directory used for output files. If not specified, will use source root.");
  app.add_option("headers",         source_names, "List of .h files to convert from C++ to SystemVerilog");
  // clang-format on

  CLI11_PARSE(app, argc, argv);

  if (quiet) TinyLog::get().mute();
  if (monochrome) TinyLog::get().mono();

  //----------
  // Startup info

  LOG_B("Metron v0.0.1\n");
  LOG_B("Quiet      %d\n", quiet);
  LOG_B("Monochrome %d\n", monochrome);
  LOG_B("Echo       %d\n", echo);
  LOG_B("Save       %d\n", save);
  LOG_B("Verbose    %d\n", verbose);
  LOG_B("Source root '%s'\n", src_root.empty() ? "<empty>" : src_root.c_str());
  LOG_B("Output root '%s'\n", out_root.empty() ? "<empty>" : out_root.c_str());

  for (auto& name : source_names) {
    LOG_B("Header %s\n", name.c_str());
  }
  LOG_B("\n");

  //----------
  // Load all source files.

  Err err;

  if (src_root.empty()) {
    LOG_R("No source root specified, using current directory\n");
    src_root = ".";
  }

  MtModLibrary lib;
  lib.add_search_path(src_root);

  LOG_B("Loading source files\n");
  for (auto& name : source_names) {
    MtSourceFile* source;
    err << lib.load_source(name.c_str(), source, verbose);
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  if (dump) {
    for (auto& source_file : lib.source_files) {
      source_file->root_node.dump_tree(0, 0, 255);
    }
  }

  //----------------------------------------

  LOG_B("Processing source files\n");

  for (auto s : lib.source_files) {
    for (auto m : s->modules) {
      lib.modules.push_back(m);
    }
  }

  err << lib.collect_structs();

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  for (auto mod : lib.modules) {
    err << mod->collect_parts();
  }

  //----------------------------------------
  // Give all fields pointers to their type struct or mod

  for (auto m : lib.modules) {
    for (auto f : m->all_fields) {
      f->_type_mod = lib.get_module(f->type_name());
      f->_type_struct = lib.get_struct(f->type_name());
    }
  }

  for (auto s : lib.structs) {
    for (auto f : s->fields) {
      f->_type_struct = lib.get_struct(f->type_name());
    }
  }

  //----------------------------------------
  // Build call graphs

  for (auto m : lib.modules) {
    err << m->build_call_graph();
  }

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto mod : lib.modules) {
    for (auto field : mod->all_fields) {
      if (field->is_component()) {
        field->_type_mod->refcount++;
      }
    }
  }

  //----------------------------------------
  // Trace

  for (auto mod : lib.modules) {
    if (verbose) {
      LOG_G("Tracing %s\n", mod->cname());
      LOG_INDENT();
    }
    mod->ctx = new MtContext(mod);
    mod->ctx->instantiate();

    MtTracer tracer(&lib, mod->ctx, verbose);
    for (auto method : mod->all_methods) {
      if (method->is_constructor()) continue;
      if (method->internal_callers.size()) continue;
      if (verbose) {
        LOG_G("Tracing %s.%s\n", mod->cname(), method->cname());
      }
      err << tracer.trace_method(mod->ctx, method);
    }
    mod->ctx->assign_struct_states();
    if (verbose) {
      mod->ctx->dump_ctx_tree();
    }
    mod->ctx->assign_state_to_field(mod);

    err << mod->ctx->check_done();
    if (err.has_err()) {
      LOG_R("Error during trace\n");
      lib.teardown();
      return -1;
    }
    if (verbose) {
      LOG_DEDENT();
    }
  }

  //----------
  // Categorize fields

  LOG_B("Categorizing fields\n");
  for (auto m : lib.modules) {
    LOG_INDENT_SCOPE();
    err << m->categorize_fields(verbose);
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  //----------
  // Categorize methods

  LOG_B("Categorizing methods\n");
  {
    LOG_INDENT_SCOPE();
    err << lib.categorize_methods(verbose);
  }

  int uncategorized = 0;
  int invalid = 0;
  for (auto mod : lib.modules) {
    for (auto m : mod->all_methods) {
      if (!m->categorized()) {
        uncategorized++;
      }
      if (!m->is_valid()) {
        invalid++;
      }
    }
  }

  if (verbose) {
    LOG_G("Methods uncategorized %d\n", uncategorized);
    LOG_G("Methods invalid %d\n", invalid);
  }

  if (uncategorized || invalid) {
    err << ERR("Could not categorize all methods\n");
    lib.teardown();
    return -1;
  }

  if (verbose) {
    for (auto m : lib.modules) m->dump();
  }

  //----------------------------------------
  // Check for and report bad fields.

  std::vector<MtField*> bad_fields;
  for (auto mod : lib.modules) {
    for (auto field : mod->all_fields) {
      if (field->_state == CTX_INVALID) {
        err << ERR("Field %s is in an invalid state\n", field->cname());
        bad_fields.push_back(field);
      }
    }
  }

  for (auto bad_field : bad_fields) {
    LOG_R("Bad field \"%s.%s\" log:\n", bad_field->_parent_mod->cname(),
          bad_field->cname());
    LOG_G("\n");
  }

  for (auto mod : lib.modules) {
    for (auto method : mod->all_methods) {
      if (method->name().starts_with("tick") && !method->in_tick) {
        err << ERR("Method %s labeled 'tick' but is not a tick.\n", method->cname());
      }
      if (method->name().starts_with("tock") && !method->in_tock) {
        err << ERR("Method %s labeled 'tock' but is not a tock.\n", method->cname());
      }
    }
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  //----------------------------------------
  // Print module tree

  LOG_G("\n");
  LOG_G("Module tree:\n");
  LOG_INDENT();
  std::function<void(MtModule*, int, bool)> step;
  step = [&](MtModule* m, int rank, bool last) -> void {
    for (int i = 0; i < rank - 1; i++) LOG_Y("|  ");
    if (last) {
      if (rank) LOG_Y("\\--");
    } else {
      if (rank) LOG_Y("|--");
    }
    LOG_Y("%s\n", m->name().c_str());
    auto field_count = m->all_fields.size();
    for (auto i = 0; i < field_count; i++) {
      auto field = m->all_fields[i];
      if (!field->is_component()) continue;
      step(lib.get_module(field->type_name()), rank + 1,
            i == field_count - 1);
    }
  };

  for (auto m : lib.modules) {
    if (m->refcount == 0) step(m, 0, false);
  }
  LOG_DEDENT();
  LOG_G("\n");

  //----------
  // Emit all modules.

  for (auto& source_file : lib.source_files) {
    LOG_G("Converting %s to SystemVerilog\n", source_file->full_path.c_str());

    std::string out_string;
    MtCursor cursor(&lib, source_file, nullptr, &out_string);
    cursor.echo = echo && !quiet;

    if (echo) LOG_G("----------------------------------------\n\n");
    err << cursor.emit_everything();
    if (echo) LOG_G("----------------------------------------\n\n");

    if (err.has_err()) {
      LOG_R("Error during code generation\n");
      lib.teardown();
      return -1;
    }

    if (save) {
      // Save translated source to output directory, if there is one.
      if (out_root.empty()) {
        LOG_Y("No output root directory specified, using source root.\n");
        out_root = src_root;
      }

      auto out_name = source_file->filename;
      out_name.resize(out_name.size() - 2);
      auto out_path = out_root + "/" + out_name + ".sv";

      LOG_G("Saving %s\n", out_path.c_str());
      mkdir_all(split_path(out_path));

      FILE* out_file = fopen(out_path.c_str(), "wb");
      if (!out_file) {
        LOG_R("ERROR Could not open %s for output\n", out_path.c_str());
      } else {
        // Copy the BOM over if needed.
        if (source_file->use_utf8_bom) {
          uint8_t bom[3] = {239, 187, 191};
          fwrite(bom, 1, 3, out_file);
        }

        fwrite(out_string.data(), 1, out_string.size(), out_file);
        fclose(out_file);
      }
    }
  }

  LOG_B("Done!\n");
  lib.teardown();

  return 0;
}

//------------------------------------------------------------------------------
