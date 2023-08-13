#pragma once

#include <map>
#include <stack>
#include <vector>
#include <functional>
#include <string>

struct CNode;

struct Cursor {

  using string_to_node = std::map<std::string, CNode*>;
  using string_to_string = std::map<std::string, std::string>;

  std::stack<std::string> block_prefix;
  std::stack<std::string> block_suffix;
  std::stack<int> override_size;
  std::stack<string_to_node> preproc_vars;
  std::stack<string_to_string> id_map;
  std::stack<string_to_string> token_map;
  std::stack<bool> elide_type;
  std::stack<bool> elide_value;

  //----------------------------------------
  // Output state

  std::string* str_out;
  std::stack<std::string> indent;
  bool at_newline = true;
  bool at_comma = false;
  bool line_dirty = false;
  bool line_elided = false;
  bool echo = false;
};
