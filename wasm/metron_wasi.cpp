#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Log.h"
#include "MtCursor.h"
#include "MtSourceFile.h"
#include "MtModule.h"
#include "MtField.h"
#include "MtContext.h"
#include "MtTracer.h"
#include "MtMethod.h"
#include "MtModLibrary.h"

__attribute__((export_name("malloc"))) void* wasi_malloc(int x) {
  return malloc(x);
}

__attribute__((export_name("free"))) void wasi_free(void* x) {
  return free(x);
}


__attribute__((export_name("metron_test2"))) void metron_test2(void* src_blob, int src_len) {

  TinyLog::get().reset();

  bool verbose = true;
  bool dump = false;
  bool echo = true;
  bool quiet = false;

  MtModLibrary lib;

  LOG("Loading blob\n");

  std::string src_str((const char*)src_blob);
  Err err;
  err << lib.load_blob("filename", "path/filename", src_blob, src_len, false, true);

  //----------------------------------------

  LOG("Collect modules\n");

  for (auto s : lib.source_files) {
    for (auto m : s->modules) {
      lib.modules.push_back(m);
    }
  }

  //----------------------------------------

  LOG("Collect parts\n");

  for (auto mod : lib.modules) {
    err << mod->collect_parts();
  }

  //----------------------------------------

  LOG("Build call graph\n");

  for (auto m : lib.modules) {
    err << m->build_call_graph();
  }

  //----------------------------------------

  LOG("Mod refcount\n");

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
    LOG_G("Tracing %s\n", mod->cname());
    LOG_INDENT();
    MtContext* ctx = new MtContext(mod);
    MtContext::instantiate(mod, ctx);
    MtTracer tracer(&lib, ctx);
    for (auto method : mod->all_methods) {
      if (method->is_constructor()) continue;
      if (method->internal_callers.size()) continue;
      LOG_G("Tracing %s.%s\n", mod->cname(), method->cname());
      err << tracer.trace_method(ctx, method);
    }
    ctx->dump_ctx_tree();
    ctx->assign_state_to_field(mod);
    err << ctx->check_done();
    if (err.has_err()) {
      return;
    }
    LOG_DEDENT();
  }

  //----------
  // Categorize fields

  LOG("Categorizing fields\n");
  for (auto m : lib.modules) {
    LOG_INDENT_SCOPE();
    err << m->categorize_fields();
  }
  LOG("\n");

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    return;
  }

  //----------
  // Categorize methods

  LOG("Categorizing methods\n");
  {
    LOG_INDENT_SCOPE();
    err << lib.categorize_methods();
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

  LOG_G("Methods uncategorized %d\n", uncategorized);
  LOG_G("Methods invalid %d\n", invalid);

  if (uncategorized || invalid) {
    err << ERR("Could not categorize all methods\n");
    return;
  }

  LOG("\n");

  for (auto m : lib.modules) m->dump();

  //----------------------------------------
  // Check for and report bad fields.

  std::vector<MtField*> bad_fields;
  for (auto mod : lib.modules) {
    for (auto field : mod->all_fields) {
      if (field->state == CTX_INVALID) {
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

  //----------------------------------------
  // Print module stats

  if (verbose) {
    LOG_Y("Module tree:\n");
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
    LOG_G("\n");
  }

  //----------
  // Dump syntax trees if requested

  if (dump) {
    for (auto& source_file : lib.source_files) {
      source_file->root_node.dump_tree(0, 0, 255);
    }
  }

  //----------
  // Emit all modules.

  for (auto& source_file : lib.source_files) {
    LOG_G("Translating %s\n", source_file->full_path.c_str());

    std::string out_string;
    MtCursor cursor(&lib, source_file, nullptr, &out_string);
    cursor.echo = echo && !quiet;

    if (echo) {
      LOG_G("----------------------------------------\n\n");
    }

    err << cursor.emit_everything();
    if (err.has_err()) {
      LOG_R("Error during code generation\n");
      return;
    }

    if (echo) {
      LOG_G("\n----------------------------------------\n");
      LOG_G("Final converted source:\n\n");
      // LOG_W("%s", out_string.c_str());
      printf("%s", out_string.c_str());
      LOG_G("\n----------------------------------------\n");
    }
  }

  LOG_B("Done!\n");
  lib.teardown();

}

__attribute__((export_name("metron_test"))) void metron_test(void* src_blob, int src_len) {
  metron_test2(src_blob, src_len);
  //metron_test2(src_blob, src_len);
}
