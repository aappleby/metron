#include "MetronApp.h"

#include "metrolib/core/Log.h"

#include "CLI11/include/CLI/App.hpp"
#include "CLI11/include/CLI/Config.hpp"
#include "CLI11/include/CLI/Formatter.hpp"

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

  Options opts;

  // clang-format off
  auto src_opt     = app.add_option("-c,--convert",    opts.src_name,     "Full path to source file to translate from C++ to SystemVerilog");
  auto dst_opt     = app.add_option("-o,--output",     opts.dst_name,     "Output file path. If not specified, will only check the source for convertibility.");
  auto verbose_opt = app.add_flag  ("-v,--verbose",    opts.verbose,      "Print detailed stats about the source modules.");
  auto quiet_opt   = app.add_flag  ("-q,--quiet",      opts.quiet,        "Quiet mode");
  auto echo_opt    = app.add_flag  ("-e,--echo",       opts.echo,         "Echo the converted source back to the terminal, with color-coding.");
  auto dump_opt    = app.add_flag  ("-d,--dump",       opts.dump,         "Dump the syntax tree of the source file(s) to the console.");
  auto mono_opt    = app.add_flag  ("-m,--monochrome", opts.monochrome,   "Monochrome mode, no color-coding");
  auto parse_opt   = app.add_flag  ("-p,--parser",     opts.parse,       "Test new parser");
  // clang-format on

  src_opt->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  // Force everything to use the new parser. FIXME - still broken
  opts.parse = true;

  if (opts.quiet) TinyLog::get().mute();
  if (opts.monochrome) TinyLog::get().mono();

  //----------
  // Startup info

  LOG_B("Metron v0.0.1\n");
  LOG_B("Source file '%s'\n", opts.src_name.empty() ? "<empty>" : opts.src_name.c_str());
  LOG_B("Output file '%s'\n", opts.dst_name.empty() ? "<empty>" : opts.dst_name.c_str());
  LOG_B("Verbose    %d\n", opts.verbose);
  LOG_B("Quiet      %d\n", opts.quiet);
  LOG_B("Echo       %d\n", opts.echo);
  LOG_B("Dump       %d\n", opts.dump);
  LOG_B("Monochrome %d\n", opts.monochrome);
  LOG_B("Parse      %d\n", opts.parse);
  LOG_B("\n");

  if (opts.parse) {
    auto result_new = main_new(opts);
    return result_new;
  }
  else {
    auto result_old = main_old(opts);
    return result_old;
  }
}
