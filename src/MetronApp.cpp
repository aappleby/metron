#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "metron_tools.h"
#include "MtCursor.h"
#include "MtModLibrary.h"
#include "MtModule.h"
#include "MtSourceFile.h"
#include "Platform.h"

#include "submodules/cli11/include/CLI/App.hpp"
#include "submodules/cli11/include/CLI/Formatter.hpp"
#include "submodules/cli11/include/CLI/Config.hpp"

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

std::string join(std::vector<std::string>& path) {
  std::string result = path[0];
  for (int i = 1; i < (int)path.size(); i++) {
    result += "/";
    result += path[i];
  }
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
    "                                                        \n" \
    " ###    ### ####### ######## ######   ######  ###    ## \n" \
    " ####  #### ##         ##    ##   ## ##    ## ####   ## \n" \
    " ## #### ## #####      ##    ######  ##    ## ## ##  ## \n" \
    " ##  ##  ## ##         ##    ##   ## ##    ## ##  ## ## \n" \
    " ##      ## #######    ##    ##   ##  ######  ##   #### \n" \
    "                                                        \n" \
    "            a C++ to SystemVerilog transpiler           \n";


  CLI::App app{banner};

  bool quiet = false;
  bool echo = false;
  bool dump = false;
  bool convert = false;
  bool stats = false;
  std::string src_root;
  std::string out_root;
  std::vector<std::string> source_names;

  // clang-format off
  app.add_flag  ("-q,--quiet",    quiet,        "Quiet mode");
  app.add_flag  ("-v,--verbose",  stats,        "Print detailed stats about the source modules.");
  app.add_flag  ("-c,--convert",  convert,      "Convert sources to SystemVerilog. If not specified, will only check inputs for convertibility.");
  app.add_flag  ("-e,--echo",     echo,         "Echo the converted source back to the terminal, with color-coding.");
  app.add_flag  ("--dump",        dump,         "Dump the syntax tree of the source file(s) to the console.");
  app.add_option("-r,--src_root", src_root,     "Root directory of the source to convert");
  app.add_option("-o,--out_root", out_root,     "Root directory used for output files. If not specified, will use source root.");
  app.add_option("headers",       source_names, "List of .h files to convert from C++ to SystemVerilog");
  // clang-format on
  
  CLI11_PARSE(app, argc, argv);

  // -r examples/uart/metron uart_top.h uart_hello.h uart_tx.h uart_rx.h
  // -r examples/rvtiny/metron toplevel.h
  // -v -r examples/rvsimple/metron toplevel.h adder.h alu.h alu_control.h config.h constants.h control_transfer.h data_memory_interface.h example_data_memory.h example_data_memory_bus.h example_text_memory.h example_text_memory_bus.h immediate_generator.h instruction_decoder.h multiplexer.h multiplexer2.h multiplexer4.h multiplexer8.h regfile.h register.h riscv_core.h singlecycle_control.h singlecycle_ctlpath.h singlecycle_datapath.h

  if (quiet) TinyLog::get().mute();

  //----------
  // Startup info

  LOG_B("Metron v0.0.1\n");
  LOG_B("Quiet   %d\n", quiet);
  LOG_B("Echo    %d\n", echo);
  LOG_B("Convert %d\n", convert);
  LOG_B("Stats   %d\n", stats);
  LOG_B("Source root '%s'\n", src_root.empty() ? "<empty>" : src_root.c_str());
  LOG_B("Output root '%s'\n", out_root.empty() ? "<empty>" : out_root.c_str());

  for (auto& name : source_names) {
    LOG_B("Header %s\n", name.c_str());
  }
  LOG_B("\n");

  //----------
  // Load all source files.

  bool error = false;

  if (src_root.empty()) {
    LOG_R("No source root specified, using current directory\n");
    src_root = ".";
  }

  MtModLibrary library;
  library.add_search_path(src_root);

  LOG_B("Loading source files\n");
  for (auto& name : source_names) {
    error |= library.load_source(name.c_str());
  }
  if (error) {
    LOG_R("Exiting due to error\n");
    return -1;
  }

  LOG_B("\n");

  LOG_B("Processing source files\n");
  error |= library.process_sources();
  if (error) {
    LOG_R("Exiting due to error\n");
    return -1;
  }

  //----------
  // Dump out module stats

  if (stats) {
    for (auto& mod : library.modules) {
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
      auto submod_count = m->submods.size();
      for (auto i = 0; i < submod_count; i++) {
        auto s = m->submods[i];
        step(library.get_module(s->type_name()), rank + 1, i == submod_count - 1);
      }
    };

    for (auto m : library.modules) {
      if (m->parents.empty()) step(m, 0, false);
    }
    LOG_G("\n");
  }

  //----------
  // Dump syntax trees if requested

  if (dump) {
    for (auto& source_file : library.source_files) {
      source_file->root_node.dump_tree();
    }
  }

  //----------
  // Emit all modules.

  if (convert || echo) {
    if (convert && out_root.empty()) {
      LOG_R("No output root directory specified, using source root.\n");
      out_root = src_root;
    }

    for (auto& source_file : library.source_files)
    {
      // Translate the source.
      auto out_name = source_file->filename;
      assert(out_name.ends_with(".h"));
      out_name.resize(out_name.size() - 2);
      auto out_path = out_root + "/" + out_name + ".sv";

      if (out_root.size()) {
        LOG_G("Converting %s -> %s\n", source_file->full_path.c_str(), out_path.c_str());
      }

      if (echo) {
        LOG_G("Converted source for %s:\n", source_file->full_path.c_str());
      }

      std::string out_string;
      MtCursor cursor(&library, source_file, &out_string);
      cursor.echo = echo && !quiet;
      cursor.cursor = source_file->source;
      cursor.source_file = source_file;

      bool emit_error = cursor.emit_translation_unit(source_file->root_node);
      cursor.emit_printf("\n");

      if (emit_error) {
        LOG_R("Error during code generation\n");
        exit(-1);
      }

      // Save translated source to output directory, if there is one.
      if (out_root.size()) {
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
  }

  LOG_G("Done!\n");
  return 0;
}

//------------------------------------------------------------------------------
