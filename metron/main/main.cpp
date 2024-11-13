#include "main.h"

#include "metrolib/core/Log.h"

#include "third_party/CLI11/include/CLI/App.hpp"
#include "third_party/CLI11/include/CLI/Config.hpp"
#include "third_party/CLI11/include/CLI/Formatter.hpp"

#include <stdio.h>
#include <stdarg.h>

Options global_options;

int main(int argc, char** argv) {
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

  global_options = Options();

  // clang-format off
  auto verbose_opt = app.add_flag  ("-v,--verbose",    global_options.verbose,      "Print detailed stats about the source modules.");
  auto quiet_opt   = app.add_flag  ("-q,--quiet",      global_options.quiet,        "Quiet mode");
  auto echo_opt    = app.add_flag  ("-e,--echo",       global_options.echo,         "Echo the converted source back to the terminal, with color-coding.");
  auto dump_opt    = app.add_flag  ("-d,--dump",       global_options.dump,         "Dump the syntax tree of the source file(s) to the console.");
  auto mono_opt    = app.add_flag  ("-m,--monochrome", global_options.monochrome,   "Monochrome mode, no color-coding");
  auto inc_opt     = app.add_option("-i,-I,--include", global_options.inc_paths,    "Include path")->allow_extra_args();
  auto src_opt     = app.add_option("-c,--convert",    global_options.src_name,     "Full path to source file to translate from C++ to SystemVerilog");
  auto dst_opt     = app.add_option("-o,--output",     global_options.dst_name,     "Output file path. If not specified, will only check the source for convertibility.");
  // clang-format on

  inc_opt->check(CLI::ExistingDirectory);
  //src_opt->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  TinyLog::get().reset();
  if (global_options.quiet) TinyLog::get().mute();
  if (global_options.monochrome) TinyLog::get().mono();

  //----------
  // Startup info

  LOG_B("========================================\n");
  LOG_B("Metron v0.0.1\n");
  LOG_B("Source file '%s'\n", global_options.src_name.empty() ? "<empty>" : global_options.src_name.c_str());
  LOG_B("Output file '%s'\n", global_options.dst_name.empty() ? "<empty>" : global_options.dst_name.c_str());
  LOG_B("Verbose    %d\n",    global_options.verbose);
  LOG_B("Quiet      %d\n",    global_options.quiet);
  LOG_B("Echo       %d\n",    global_options.echo);
  LOG_B("Dump       %d\n",    global_options.dump);
  LOG_B("Monochrome %d\n",    global_options.monochrome);
  LOG_B("\n");

  return main_new();
}
