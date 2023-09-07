#include "MetronApp.h"

#include "metron/app/MtModLibrary.h"
#include "metrolib/core/Log.h"
#include "metron/nodes/MtField.h"
#include "metron/nodes/MtModule.h"
#include "metron/nodes/MtMethod.h"
#include "metron/nodes/MtStruct.h"
#include "metron/app/MtSourceFile.h"
#include "metron/tracer/MtInstance.h"
#include "metron/tracer/MtTracer2.h"
#include "metron/tracer/MtContext.h"
#include "metron/tracer/MtTracer.h"
#include "metron/codegen/MtCursor.h"

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

int main_old(Options opts) {

  //----------
  // Load all source files.

  Err err;
  MtModLibrary lib;
  MtSourceFile* source = nullptr;

  lib.add_search_path(".");

  {
    LOG_B("Loading source file %s\n", opts.src_name.c_str());
    auto src_path = split_path(opts.src_name);
    src_path.pop_back();
    auto search_path = join_path(src_path);
    lib.add_search_path(search_path);
    err << lib.load_source(opts.src_name.c_str(), source);
  }

  if (err.has_err()) {
    LOG_R("Exiting due to error\n");
    lib.teardown();
    return -1;
  }

  if (opts.dump) {
    source->root_node.dump_tree(0, 0, 255);
  }

  //----------------------------------------
  // All modules are now in the library, we can resolve references to other
  // modules when we're collecting fields.

  LOG_B("Processing source files\n");
  for (auto m : lib.all_modules) {
    err << m->collect_fields_and_methods();
  }

  for (auto s : lib.all_structs) {
    err << s->collect_fields();
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
  LOG_B("\n");

  if (opts.verbose) {
    lib.dump_lib();
    LOG_G("\n");
  }

  //----------------------------------------
  // New Trace

  MtModule* top = nullptr;
  for (auto mod : lib.all_modules) {
    if (mod->refcount) continue;
    if (top) {
      LOG_R("Multiple tops!\n");
    }
    else {
      top = mod;
    }
  }

  MtModuleInstance* root_inst = nullptr;
  if (top) {
    LOG_B("Tracing version 2: top is '%s'\n", top->cname());
    LOG_INDENT_SCOPE();

    root_inst = new MtModuleInstance("<top>", "<top>", top, nullptr);
    //root_inst->dump();

    MtTracer2 tracer(&lib, root_inst, true);

    std::vector<MtCallInstance*> calls;

    for (auto m : root_inst->_methods) {
      auto m2 = m->_method;
      if (m2->is_constructor()) continue;
      if (m2->internal_callers.size()) continue;

      LOG_B("Tracing %s\n", m->_name.c_str());
      LOG_INDENT();
      auto call_inst = new MtCallInstance(m->_name, "<top>." + m->_name, nullptr, MnNode::null, m);

      for (auto p : call_inst->_params) {
        err << p->log_action(MnNode::null, ACT_WRITE);
      }

      calls.push_back(call_inst);
      err << tracer.trace_top_call(call_inst);
      LOG_DEDENT();
      LOG_B("Tracing %s done\n", m->_name.c_str());
    }

    LOG_B("Calls:\n");
    LOG_INDENT();
    for (auto c : calls) c->dump();
    LOG_DEDENT();

    err << root_inst->assign_types();
    //err << root_inst->sanity_check();
    root_inst->dump();

    LOG_B("Tracing version 2: done\n");
    LOG_B("\n");
  }

  //----------------------------------------
  // Trace

  for (auto mod : lib.all_modules) {
    LOG_B("Tracing %s\n", mod->cname());
    LOG_INDENT();
    mod->ctx = new MtContext(mod);
    mod->ctx->instantiate();

    MtTracer tracer(&lib, mod->ctx, opts.verbose);

    for (auto method : mod->all_methods) {
      if (method->is_constructor()) continue;
      if (method->internal_callers.size()) continue;
      if (opts.verbose) {
        LOG_G("Tracing %s.%s\n", mod->cname(), method->cname());
      }
      err << tracer.trace_method(mod->ctx, method);
      //if (verbose) {
      //  mod->ctx->dump_ctx_tree();
      //}
    }
    mod->ctx->assign_struct_states();
    if (opts.verbose) {
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
    err << m->categorize_fields(opts.verbose);
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
  err << lib.categorize_methods(opts.verbose);

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

  if (opts.verbose) {
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

  if (opts.verbose) {
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

    for (auto m : lib.all_modules) m->dump_module();

    LOG_DEDENT();
    LOG("\n");
  }

  //----------------------------------------
  // Check for and report bad fields.

  std::vector<MtField*> bad_fields;
  for (auto mod : lib.all_modules) {
    for (auto field : mod->all_fields) {
      if (field->_state == TS_INVALID) {
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

  if (root_inst) {
    err << root_inst->merge_with_source();
  }
  delete root_inst;


  //----------
  // Emit all modules.

 {
    LOG_G("Converting %s to SystemVerilog\n", opts.src_name.c_str());

    std::string out_string;
    MtCursor cursor(&lib, source, nullptr, &out_string);
    cursor.echo = opts.echo && !opts.quiet;

    if (opts.echo) LOG_G("----------------------------------------\n\n");
    err << cursor.emit_everything();
    if (opts.echo) LOG_G("----------------------------------------\n\n");

    if (err.has_err()) {
      LOG_R("Error during code generation\n");
      lib.teardown();
      return -1;
    }

    if (opts.dst_name.size()) {
      // Save translated source to output directory, if there is one.

      LOG_G("Saving %s\n", opts.dst_name.c_str());

      auto dst_path = split_path(opts.dst_name);
      dst_path.pop_back();
      mkdir_all(dst_path);

      FILE* out_file = fopen(opts.dst_name.c_str(), "wb");
      if (!out_file) {
        LOG_R("ERROR Could not open %s for output\n", opts.dst_name.c_str());
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
