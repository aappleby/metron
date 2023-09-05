#include "CSourceRepo.hpp"

#include "CNodeClass.hpp"
#include "CNodeField.hpp"
#include "CNodeFunction.hpp"
#include "CNodeStruct.hpp"
#include "CSourceFile.hpp"
#include "matcheroni/Utilities.hpp"
#include "metrolib/core/Log.h"
#include "NodeTypes.hpp"

#include <functional>

namespace fs = std::filesystem;

//------------------------------------------------------------------------------

CNodeClass* CSourceRepo::get_class(std::string_view name) {
  for (auto c : all_classes) {
    if (c->get_name() == name) return c;
  }
  return nullptr;
}

CNodeStruct* CSourceRepo::get_struct(std::string_view name) {
  for (auto c : all_structs) {
    auto struct_name = c->get_name();
    if (struct_name == name) return c;
  }
  return nullptr;
}

CNodeNamespace* CSourceRepo::get_namespace(std::string_view name) {
  for (auto c : all_namespaces) {
    auto namespace_name = c->get_name();
    if (namespace_name == name) return c;
  }
  return nullptr;
}

//------------------------------------------------------------------------------

std::string CSourceRepo::resolve_filename(const std::string& filename) {
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
  std::string absolute_path = resolve_filename(filename);

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
    CSourceFile* file = pair.second;

    for (auto n : file->context.root_node) {

      if (auto node_template = n->as_a<CNodeTemplate>()) {
        auto node_class = node_template->child<CNodeClass>();

        node_class->repo = this;
        node_class->file = file;

        all_classes.push_back(node_class);
        node_class->collect_fields_and_methods();
      }
      else if (auto node_class = n->as_a<CNodeClass>()) {

        node_class->repo = this;
        node_class->file = file;

        all_classes.push_back(node_class);
        node_class->collect_fields_and_methods();
      }
      else if (auto node_struct = n->as_a<CNodeStruct>()) {
        node_struct->repo = this;
        node_struct->file = file;

        all_structs.push_back(node_struct);
        node_struct->collect_fields_and_methods();
      }
      else if (auto node_namespace = n->as_a<CNodeNamespace>()) {
        node_namespace->repo = this;
        node_namespace->file = file;

        all_namespaces.push_back(node_namespace);
        node_namespace->collect_fields_and_methods();
      }

    }
  }


  /*

  for (auto s : all_structs) {
    err << s->collect_fields();
  }
  */

  //----------------------------------------
  // Count module instances so we can find top modules.

  for (auto c : all_classes) {
    for (auto f : c->all_fields) {
      if (f->is_component()) {
        f->_type_class->refcount++;
      }
    }
  }

  /*
  top = nullptr;
  for (auto c : all_classes) {
    if (c->refcount == 0) {
      if (top == nullptr) {
        top = c;
      }
      else {
        LOG_R("Multiple top modules!\n");
        assert(false);
      }
    }
  }
  if (top == nullptr) {
    LOG_R("No top module?\n");
    assert(false);
  }
  */

  return err;
}

//------------------------------------------------------------------------------

Err CSourceRepo::build_call_graphs() {
  Err err;

  for (auto mod : all_classes) {
    err << mod->build_call_graph(this);
  }

  return err;
}

//------------------------------------------------------------------------------

Err CSourceRepo::categorize_fields() {
  Err err;

  for (auto c : all_classes) {
    auto cname = c->get_name();
    LOG_G("Categorizing %.*s\n", cname.size(), cname.data());

    for (auto f : c->all_fields) {
      auto fname = f->get_name();


#if 0
      if (f->is_param()) {
        continue;
      }

      if (f->is_component()) {
        components.push_back(f);
      }
      else if (f->is_public() && f->is_input()) {
        input_signals.push_back(f);
      }
      else if (f->is_public() && f->is_signal()) {
        output_signals.push_back(f);
      }
      else if (f->is_public() && f->is_register()) {
        output_registers.push_back(f);
      }
      else if (f->is_private() && f->is_register()) {
        private_registers.push_back(f);
      }
      else if (f->is_private() && f->is_signal()) {
        private_signals.push_back(f);
      }
      else if (!f->is_public() && f->is_input()) {
        private_registers.push_back(f);
      }
      else if (f->is_enum()) {
      }
      else if (f->is_dead()) {
        dead_fields.push_back(f);
      }
      else {
      }
#endif
      //return ERR("Don't know how to categorize %.*s = %s\n", fname.size(), fname.data(), to_string(f->_state));
    }
  }



  return err;
}

//------------------------------------------------------------------------------

Err CSourceRepo::categorize_methods() {
  //----------------------------------------
  // Trace done, all our fields should have a state assigned. Categorize the
  // methods.

  for (auto c : all_classes) {
    if (c->all_constructors.size() > 1) {
      auto name = c->get_name();
      return ERR("Class %.*s has multiple constructors", int(name.size()), name.data());
    }
  }

  //----------------------------------------
  // Methods named "tick" are ticks, etc.

  for (auto c : all_classes) {
    for (auto f : c->all_functions) {
      if (f->get_name().starts_with("tick")) f->is_tick_ = true;
      if (f->get_name().starts_with("tock")) f->is_tock_ = true;
    }
  }

  //----------------------------------------
  // Methods that only call funcs in the same module and don't write anything
  // are funcs.

  //----------------------------------------
  // Methods that call funcs in other modules _must_ be tocks.

  //----------------------------------------
  // Methods that write registers _must_ be ticks.

  //----------------------------------------
  // Methods that are downstream from ticks _must_ be ticks.

  //----------------------------------------
  // Methods that write signals _must_ be tocks.

  //----------------------------------------
  // Methods that are upstream from tocks _must_ be tocks.

  //----------------------------------------
  // Methods that write outputs are tocks unless they're already ticks.

  //----------------------------------------
  // Methods that are downstream from tocks are tocks unless they're already
  // ticks.

  //----------------------------------------
  // Just mark everything left as tock.

  //----------------------------------------
  // Methods categorized, we can assign emit types

  //----------------------------------------
  // Methods categorized, we can split up internal_callers

  //----------------------------------------
  // Methods categorized, now we can categorize the inputs of the methods.

  //----------------------------------------
  // Check for ticks with return values.

  //----------------------------------------
  // Done!

  return Err();
}


//------------------------------------------------------------------------------

void CSourceRepo::dump() {
  {
    LOG_G("Search paths:\n");
    LOG_INDENT_SCOPE();
    for (auto s : search_paths) LOG_G("`%s`\n", s.c_str());
  }

  {
    LOG_G("Source files:\n");
    LOG_INDENT_SCOPE();
    for (auto pair : source_map) LOG_L("`%s`\n", pair.first.c_str());
  }

  {
    LOG_G("Classes:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_classes) n->dump();
  }

  {
    LOG_G("Structs:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_structs) n->dump();
  }

  {
    LOG_G("Namespaces:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_namespaces) n->dump();
  }
}

//------------------------------------------------------------------------------

#if 0
CNode* CSourceRepo::resolve(CNodeClass* parent, CNode* path) {
  if (!path) return nullptr;

  //----------

  if (path->get_text() == ".") {
    return resolve(parent, path->node_next);
  }

  if (auto field_path = path->as_a<CNodeFieldExpression>()) {
    return resolve(parent, field_path->child_head);
  }

  //----------

  if (auto scope_path = path->as_a<CNodeQualifiedIdentifier>()) {
    return resolve(parent, scope_path->child_head);
  }

  //----------

  if (auto id = path->as_a<CNodeIdentifier>()) {
    if (id->tag_is("field_path")) {
      auto field = parent->get_field(id->get_text())->as_a<CNodeField>();
      auto field_class = get_class(field->get_type_name());
      return resolve(field_class, id->node_next);
    }
    else if (id->tag_is("scope_path")) {
      assert(false);
      return nullptr;
    }
    else {
      if (auto field = parent->get_field(id->get_text())) {
        return field;
      }
      if (auto method = parent->get_function(id->get_text())) {
        return method;
      }
      assert(false);
      return nullptr;
    }
  }

  //----------

  assert(false && "Could not resolve function name");
  return nullptr;
}
#endif
