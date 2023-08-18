#include "CSourceRepo.hpp"

#include "CSourceFile.hpp"
#include "CNodeStruct.hpp"
#include "CNodeClass.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"

#include <functional>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

CNodeClass* CSourceRepo::get_module(std::string_view name) {
  for (auto c : all_modules) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CNodeStruct* CSourceRepo::get_struct(std::string_view name) {
  for (auto c : all_structs) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_path(const std::string& filename) {
  for (auto& path : search_paths) {
    std::string full_path = fs::absolute(path + filename);
    //LOG("%s exists? ", full_path.c_str());
    if (fs::is_regular_file(full_path)) {
      //LOG_G("YES\n");
      return full_path;
    } else {
      //LOG_R("NO\n");
    }
  }
  return "";
}

//------------------------------------------------------------------------------

Err CSourceRepo::load_source(std::string filename, CSourceFile** out_source) {
  std::string absolute_path = resolve_path(filename);

  if (source_map.contains(absolute_path)) return Err();

  fs::path absolute_dir(absolute_path);
  absolute_dir.remove_filename();
  search_paths.push_back(absolute_dir);

  std::string src_blob = matcheroni::utils::read(absolute_path);

  bool use_utf8_bom = false;
  if (uint8_t(src_blob[0]) == 239 &&
      uint8_t(src_blob[1]) == 187 &&
      uint8_t(src_blob[2]) == 191) {
    use_utf8_bom = true;
    src_blob.erase(src_blob.begin(), src_blob.begin() + 3);
  }

  //printf("%s\n", src_blob.c_str());

  auto source_file = new CSourceFile();

  (void)source_file->init(this, filename, absolute_path, src_blob, use_utf8_bom);

  if (out_source) *out_source = source_file;

  return Err();
}

//------------------------------------------------------------------------------

Err CSourceRepo::collect_fields_and_methods() {
  Err err;

  for (auto pair : source_map) {
    visit(pair.second->context.root_node, [this](CNode* n){
      if (auto node_class = n->as_a<CNodeClass>()) {
        //printf("%s %s\n", node_class->match_name, node_class->class_name().c_str());
        all_modules.push_back(node_class);
        node_class->collect_fields_and_methods();
      }
      if (auto node_struct = n->as_a<CNodeStruct>()) {
        all_structs.push_back(node_struct);
        node_struct->collect_fields_and_methods();
      }

    });
  }


  /*

  for (auto s : all_structs) {
    err << s->collect_fields();
  }

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto mod : all_modules) {
    for (auto field : mod->all_fields) {
      if (field->is_component()) {
        field->_type_mod->refcount++;
      }
    }
  }
  */

  return err;
}

//------------------------------------------------------------------------------

Err CSourceRepo::build_call_graphs() {
  Err err;

  for (auto mod : all_modules) {
    err << mod->build_call_graph();
  }

  return err;
}

//------------------------------------------------------------------------------

void CSourceRepo::categorize_methods() {
}


//------------------------------------------------------------------------------

void CSourceRepo::dump() {
  //std::vector<std::string> search_paths = {""};
  //std::map<std::string, CSourceFile*> source_map;

  LOG_B("//----------------------------------------\n");
  LOG_B("// Repo dump\n");

  LOG_B("Search paths:\n");
  LOG_INDENT();
  for (auto s : search_paths) {
    LOG_B("`%s`\n", s.c_str());
  }
  LOG_DEDENT();

  LOG_B("Source files:\n");
  LOG_INDENT();
  for (auto pair : source_map) {
    LOG_B("`%s`\n", pair.first.c_str());
  }
  LOG_DEDENT();

  LOG_B("Classes:\n");
  LOG_INDENT();
  for (auto n : all_modules) {
    //n->dump_tree(3);
    LOG_B("%s @ %p\n", std::string(n->get_name()).c_str(), n);
    n->dump_tree();

    /*
    if (n->node_parent && n->node_parent->is_a<CNodeTemplate>()) {
      LOG_B("CNodeClass has template parent\n");
    }
    */

  }
  LOG_DEDENT();

  LOG_B("Structs:\n");
  LOG_INDENT();
  for (auto n : all_structs) {
    LOG_B("%p\n", n);
  }
  LOG_DEDENT();

  LOG_B("//----------------------------------------\n");

  /*
  for (auto pair : source_map) {
    auto s = pair.second;
    printf("\n\n\n");
    s->dump();
  }
  */
}

//------------------------------------------------------------------------------
