#include "MetronApp.h"
#include "metrolib/core/Log.h"
#include "metrolib/core/Platform.h"

#include "metron/parser/CSourceFile.hpp"
#include "metron/parser/CSourceRepo.hpp"
#include "metron/parser/CParser.hpp"
#include "metron/parser/Cursor.hpp"

#include "matcheroni/Utilities.hpp"

#include <functional>

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

typedef std::function<void(CNode*)> node_visitor;

void visit(CNode* n, node_visitor v) {
  v(n);
  for (auto c = n->child_head; c; c = c->node_next) {
    visit(c, v);
  }
}

//------------------------------------------------------------------------------

int main_new(Options opts) {
  LOG_G("New parser!\n");

  Err err;

  CSourceRepo repo;
  CSourceFile* root_file = nullptr;
  err << repo.load_source(opts.src_name, &root_file);

  if (root_file) {
    //utils::print_trees(root_file->context, utils::to_span(root_file->source_code), 50, 3);
    root_file->context.root_node->dump_tree();
  }

#if 1
  std::vector<CNodeClass*> class_nodes;

  for (auto pair : repo.source_map) {
    visit(pair.second->context.root_node, [&class_nodes](CNode* n){
      if (auto node_class = n->as_a<CNodeClass>()) {
        //printf("%s %s\n", node_class->match_name, node_class->class_name().c_str());
        class_nodes.push_back(node_class);
      }
    });
  }






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
#endif

  LOG_B("Done!\n");

  return 0;
}
