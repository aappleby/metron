#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Log.h"
#include "MtContext.h"
#include "MtField.h"
#include "MtMethod.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "metron_tools.h"
#include "submodules/CLI11/include/CLI/App.hpp"
#include "submodules/CLI11/include/CLI/Config.hpp"
#include "submodules/CLI11/include/CLI/Formatter.hpp"

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
    plat_mkdir(temp.c_str(), 0x777);
  }
}

//------------------------------------------------------------------------------

int main(int argc, char** argv) {
  const char* banner =
      "                                                        \n"
      " ###    ### ####### ######## ######   ######  ###    ## \n"
      " ####  #### ##         ##    ##   ## ##    ## ####   ## \n"
      " ## #### ## #####      ##    ######  ##    ## ## ##  ## \n"
      " ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## \n"
      " ##      ## #######    ##    ##   ##  ######  ##   #### \n"
      "                                                        \n"
      "            a C++ to SystemVerilog transpiler           \n";

  CLI::App app{banner};

  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool convert = false;
  bool verbose = false;
  std::string src_root;
  std::string out_root;
  std::vector<std::string> source_names;

  // clang-format off
  app.add_flag  ("-q,--quiet",    quiet,        "Quiet mode");
  app.add_flag  ("-v,--verbose",  verbose,      "Print detailed stats about the source modules.");
  app.add_flag  ("-c,--convert",  convert,      "Convert sources to SystemVerilog. If not specified, will only check inputs for convertibility.");
  app.add_flag  ("-e,--echo",     echo,         "Echo the converted source back to the terminal, with color-coding.");
  app.add_flag  ("--dump",        dump,         "Dump the syntax tree of the source file(s) to the console.");
  app.add_option("-r,--src_root", src_root,     "Root directory of the source to convert");
  app.add_option("-o,--out_root", out_root,     "Root directory used for output files. If not specified, will use source root.");
  app.add_option("headers",       source_names, "List of .h files to convert from C++ to SystemVerilog");
  // clang-format on

  CLI11_PARSE(app, argc, argv);

  // quiet = false;

  // -r examples/uart/metron uart_top.h
  // -r examples/rvtiny/metron toplevel.h
  // -v -r examples/rvsimple/metron toplevel.h

  if (quiet) TinyLog::get().mute();
  // TinyLog::get().mute();

  //----------
  // Startup info

  LOG_B("Metron v0.0.1\n");
  LOG_B("Quiet   %d\n", quiet);
  LOG_B("Echo    %d\n", echo);
  LOG_B("Convert %d\n", convert);
  LOG_B("Verbose %d\n", verbose);
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
    return -1;
  }

  //----------------------------------------

  LOG_B("Processing source files\n");

  for (auto s : lib.source_files) {
    for (auto m : s->modules) {
      lib.modules.push_back(m);
    }
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  for (auto mod : lib.modules) {
    err << mod->collect_parts();
  }

  //----------------------------------------
  // Build call graph

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
  // Find top module.

  MtModule* top_mod = nullptr;
  for (auto m : lib.modules) {
    if (m->refcount == 0) {
      top_mod = m;
      break;
    }
  }

  //----------------------------------------
  // Instantiate top module.

  MtContext* top_ctx = new MtContext();
  top_ctx->type = CTX_MODULE;
  top_ctx->parent = nullptr;
  top_ctx->name = "<top>";
  top_ctx->field = nullptr;
  top_ctx->method = nullptr;
  top_ctx->mod = top_mod;
  top_ctx->state = CTX_PENDING;

  MtContext::instantiate(top_mod, top_ctx);

  //----------------------------------------
  // Trace

  for (auto method : top_mod->all_methods) {
    if (method->is_constructor()) continue;
    if (method->callers.size()) continue;

    LOG_G("Tracing %s.%s\n", top_mod->cname(), method->cname());

    MtTracer tracer(&lib);

    auto method_ctx = top_ctx->resolve(method->name());

    err << tracer.trace_dispatch(method_ctx, method->_node);
  }

  top_ctx->assign_state_to_field();

  LOG_G("\n");

  //----------------------------------------

  LOG_G("Lib dump:\n");
  lib.dump();
  LOG_G("\n");

  LOG_G("Context dump:\n");
  top_ctx->dump_tree();
  LOG_G("\n");

  // lib.source_files[0]->root_node.dump_tree();

  exit(0);

  // Check that all entries in the state map ended up in a valid state.

  /*
  top_inst->dump();

  // err << lib.process_sources();

  LOG("\n");

  LOG_G("<top> ");
  for (auto mod : lib.modules) {
    if (mod->parents.empty()) mod->dump_mod_tree();
  }
  LOG("\n");

  for (auto mod : lib.modules) {
    mod->dump();
  }
  LOG("\n");
  */

#if 0
  //----------
  // Print module stats

  if (verbose) {
    for (auto& mod : lib.modules) {
      mod->dump_banner();
    }
    LOG_G("\n");

    LOG_Y("Module tree:\n");
    std::function<void(MtModule*, int, bool)> step;
    step = [&](MtModule* m, int rank, bool last) -> void {
      for (int i = 0; i < rank - 1; i++) LOG_Y("|  ");
      if (last) {
        if (rank) LOG_Y("\\--");
      }
      else {
        if (rank) LOG_Y("|--");
      }
      LOG_Y("%s\n", m->name().c_str());
      auto component_count = m->components.size();
      for (auto i = 0; i < component_count; i++) {
        auto component = m->components[i];
        step(lib.get_module(component->type_name()), rank + 1, i == component_count - 1);
      }
    };

    for (auto m : lib.modules) {
      if (m->parents.empty()) step(m, 0, false);
    }
    LOG_G("\n");
  }

  //----------
  // Dump syntax trees if requested

  if (dump) {
    for (auto& source_file : lib.source_files) {
      source_file->root_node.dump_tree();
    }
  }

  //----------

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    return -1;
  }

  //----------
  // Emit all modules.

  if (convert && out_root.empty()) {
    LOG_R("No output root directory specified, using source root.\n");
    out_root = src_root;
  }

  for (auto& source_file : lib.source_files)
  {
    Err err;

    // Translate the source.
    auto out_name = source_file->filename;
    assert(out_name.ends_with(".h"));
    out_name.resize(out_name.size() - 2);
    auto out_path = out_root + "/" + out_name + ".sv";

    if (out_root.size()) {
      LOG_G("Converting %s -> %s\n", source_file->full_path.c_str(), out_path.c_str());
    }

    std::string out_string;
    MtCursor cursor(&lib, source_file, nullptr, &out_string);
    cursor.echo = echo && !quiet;

    if (echo) {
      LOG_G("--------------------------------------------------------------------------------\n\n");
    }

    err << cursor.emit_everything();
    if (err.has_err()) {
      LOG_R("Error during code generation\n");
      exit(-1);
    }

    if (echo) {
      LOG_G("\n--------------------------------------------------------------------------------\n");
      LOG_G("Final converted source:\n\n");
      LOG_W("%s", out_string.c_str());
      LOG_G("\n--------------------------------------------------------------------------------\n");
    }

    // Save translated source to output directory, if there is one.
    if (convert && out_root.size()) {
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

  LOG_G("Done!\n");

  lib.teardown();
#endif

  LOG_B("Done!\n");

  return 0;
}

//------------------------------------------------------------------------------
