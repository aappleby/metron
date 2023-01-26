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

std::string join_path(std::vector<std::string>& path) {
  std::string result;
  for (auto& s : path) {
    if (result.size()) result += "/";
    result += s;
  }
  return result;
}

//------------------------------------------------------------------------------

void mkdir_all(const std::vector<std::string>& full_path) {
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

  std::string src_name;
  std::string dst_name;
  bool verbose = false;
  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool monochrome = false;

  // clang-format off
  auto src_opt     = app.add_option("-c,--convert",    src_name,     "Full path to source file to translate from C++ to SystemVerilog");
  auto dst_opt     = app.add_option("-o,--output",     dst_name,     "Output file path. If not specified, will only check the source for convertibility.");
  auto verbose_opt = app.add_flag  ("-v,--verbose",    verbose,      "Print detailed stats about the source modules.");
  auto quiet_opt   = app.add_flag  ("-q,--quiet",      quiet,        "Quiet mode");
  auto echo_opt    = app.add_flag  ("-e,--echo",       echo,         "Echo the converted source back to the terminal, with color-coding.");
  auto dump_opt    = app.add_flag  ("-d,--dump",       dump,         "Dump the syntax tree of the source file(s) to the console.");
  auto mono_opt    = app.add_flag  ("-m,--monochrome", monochrome,   "Monochrome mode, no color-coding");
  // clang-format on

  src_opt->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  if (quiet) TinyLog::get().mute();
  if (monochrome) TinyLog::get().mono();

  //----------
  // Startup info

  LOG_B("Metron v0.0.1\n");
  LOG_B("Source file '%s'\n", src_name.empty() ? "<empty>" : src_name.c_str());
  LOG_B("Output file '%s'\n", dst_name.empty() ? "<empty>" : dst_name.c_str());
  LOG_B("Verbose    %d\n", verbose);
  LOG_B("Quiet      %d\n", quiet);
  LOG_B("Echo       %d\n", echo);
  LOG_B("Dump       %d\n", dump);
  LOG_B("Monochrome %d\n", monochrome);
  LOG_B("\n");

  //----------
  // Load all source files.

  Err err;
  MtModLibrary lib;
  MtSourceFile* source = nullptr;

  lib.add_search_path(".");

  {
    LOG_B("Loading source file %s\n", src_name.c_str());
    auto src_path = split_path(src_name);
    src_path.pop_back();
    auto search_path = join_path(src_path);
    lib.add_search_path(search_path);
    err << lib.load_source(src_name.c_str(), source);
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  if (dump) {
    source->root_node.dump_tree(0, 0, 255);
  }

  //----------------------------------------

  LOG_B("Processing source files\n");
  {
    //----------------------------------------
    // All modules are now in the library, we can resolve references to other
    // modules when we're collecting fields.

    for (auto mod : lib.all_modules) {
      err << mod->collect_parts();
    }

    //----------------------------------------
    // Give all fields pointers to their type struct or mod

    // FIXME: Why aren't these being intialized in the field constructors?

    for (auto m : lib.all_modules) {
      for (auto f : m->all_fields) {
        f->_type_mod = lib.get_module(f->type_name());

        auto new_struct = lib.get_struct(f->type_name());
        if (new_struct != f->_type_struct) {
          f->_node.dump_tree();
          //assert(false);
          f->_type_struct = new_struct;
        }
      }
    }

    for (auto s : lib.all_structs) {
      for (auto f : s->fields) {
        f->_type_struct = lib.get_struct(f->type_name());
      }
    }

    //----------------------------------------
    // Build call graphs

    for (auto m : lib.all_modules) {
      err << m->build_call_graph();
    }

    //----------------------------------------
    // Count module instances so we can find top modules.

    for (auto mod : lib.all_modules) {
      for (auto field : mod->all_fields) {
        if (field->is_component()) {
          field->_type_mod->refcount++;
        }
      }
    }
  }
  LOG_B("\n");

  if (verbose) {
    lib.dump();
    LOG_G("\n");
  }

  //----------------------------------------
  // Trace

  for (auto mod : lib.all_modules) {
    LOG_B("Tracing %s\n", mod->cname());
    LOG_INDENT();
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
      //if (verbose) {
      //  mod->ctx->dump_ctx_tree();
      //}
    }
    mod->ctx->assign_struct_states();
    if (verbose) {
      LOG_G("Final context tree for module %s:\n", mod->cname());
      mod->ctx->dump_ctx_tree();
      LOG("\n");
    }
    mod->ctx->assign_state_to_field(mod);

    err << mod->ctx->check_done();
    if (err.has_err()) {
      LOG_R("Error during trace\n");
      lib.teardown();
      return -1;
    }
    LOG_DEDENT();
  }

  //----------
  // Categorize fields

  LOG_B("Categorizing fields\n");
  for (auto m : lib.all_modules) {
    LOG_INDENT_SCOPE();
    err << m->categorize_fields(verbose);
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }
  LOG("\n");

  //----------
  // Categorize methods

  LOG_B("Categorizing methods\n");
  LOG_INDENT();
  err << lib.categorize_methods(verbose);

  int uncategorized = 0;
  int invalid = 0;
  for (auto mod : lib.all_modules) {
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
  LOG_DEDENT();
  LOG("\n");

  //----------------------------------------

  if (verbose) {
    LOG_B("Module info:\n");
    LOG_INDENT();

    //----------------------------------------
    // Print module tree

    LOG_B("Module tree:\n");
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

    for (auto m : lib.all_modules) {
      if (m->refcount == 0) step(m, 0, false);
    }
    LOG_DEDENT();
    LOG_G("\n");

    for (auto m : lib.all_modules) m->dump();

    LOG_DEDENT();
    LOG("\n");
  }

  //----------------------------------------
  // Check for and report bad fields.

  std::vector<MtField*> bad_fields;
  for (auto mod : lib.all_modules) {
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

  for (auto mod : lib.all_modules) {
    for (auto method : mod->all_methods) {
      if (method->name().starts_with("tick") && !method->is_tick_) {
        err << ERR("Method %s labeled 'tick' but is not a tick.\n", method->cname());
      }
      if (method->name().starts_with("tock") && !method->is_tock_) {
        err << ERR("Method %s labeled 'tock' but is not a tock.\n", method->cname());
      }
    }
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  //----------
  // Emit all modules.

 {
    LOG_G("Converting %s to SystemVerilog\n", src_name.c_str());

    std::string out_string;
    MtCursor cursor(&lib, source, nullptr, &out_string);
    cursor.echo = echo && !quiet;

    if (echo) LOG_G("----------------------------------------\n\n");
    err << cursor.emit_everything();
    if (echo) LOG_G("----------------------------------------\n\n");

    if (err.has_err()) {
      LOG_R("Error during code generation\n");
      lib.teardown();
      return -1;
    }

    if (dst_name.size()) {
      // Save translated source to output directory, if there is one.

      LOG_G("Saving %s\n", dst_name.c_str());

      auto dst_path = split_path(dst_name);
      dst_path.pop_back();
      mkdir_all(dst_path);

      FILE* out_file = fopen(dst_name.c_str(), "wb");
      if (!out_file) {
        LOG_R("ERROR Could not open %s for output\n", dst_name.c_str());
      } else {
        // Copy the BOM over if needed.
        if (source->use_utf8_bom) {
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
