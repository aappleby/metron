#include "test_utils.h"
#include "metrolib/core/Err.h"

//------------------------------------------------------------------------------

void parse_simple(std::string src, MtModLibrary& library) {
  /*
  std::string out;

  auto source_file = new MtSourceFile("test.h", src);

  for (auto& mod : source_file->modules) mod->load_pass1();
  for (auto& mod : source_file->modules) mod->load_pass2();
  for (auto& mod : source_file->modules) mod->load_pass3();
  for (auto& mod : source_file->modules) {
    mod->check_dirty_ticks();
    mod->check_dirty_tocks();
  }

  Err err;

  err << library.load_blob("test.h", "test.h", src, / *use_utf8_bom* / false, /
*verbose* / false); err << library.process_sources();
  */
}

//------------------------------------------------------------------------------

Err translate_simple(std::string src, std::string& out) {
  Err err;

#if 0

  MtModLibrary library;
  parse_simple(src, library);

  auto source_file = library.source_files[0];

  /*
  for (auto mod : source_file->modules) {
    if (mod->dirty_check_fail) err << ERR("DCF");
  }
  */

  MtCursor cursor(&library, source_file, nullptr, &out);
  err << cursor.emit_everything();
  err << cursor.emit_print("\n");

  for (auto c : out) {
    if (c < 0) {
      err << ERR("Non-ascii character in output?");
      break;
    }
  }
#endif

  return err;
}

//------------------------------------------------------------------------------

TestResults test_translate_simple() {
  TEST_INIT();

  std::string source = R"(

#include "metron/metron_tools.h"

class Module {
public:

  logic<8> tock(logic<8> in) {
    logic<8> out = my_reg + 7;
    tick(in + 7);
    return out;
  }

private:

  logic<8> my_reg;

  void tick(logic<8> in) {
    my_reg = in - 12;
  }
};

)";

  std::string out;

  Err err = translate_simple(source, out);

  EXPECT_EQ(false, err.has_err(), "Translation failed");

  TEST_DONE();
}

//------------------------------------------------------------------------------

TestResults test_utils() {
  TEST_INIT();
  results << test_translate_simple();
  TEST_DONE();
}

//------------------------------------------------------------------------------
