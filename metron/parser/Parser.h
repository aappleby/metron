#pragma once

#include <stack>
#include "tree-sitter/lib/include/tree_sitter/api.h"

struct PNode;
struct MtSourceFile;

class Parser {
public:

  void parse(MtSourceFile* source);

  PNode* parse(TSNode n);
  PNode* parse_error(TSNode n);
  PNode* parse_translation_unit(TSNode n);
  PNode* parse_preproc_ifdef(TSNode n);

  std::stack<const char*> cursor;
  MtSourceFile* source = nullptr;
};
