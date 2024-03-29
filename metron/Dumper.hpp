#pragma once

#include <string>

struct CNode;
struct CInstance;
struct CNodeFunction;
struct CNodeClass;
struct CSourceRepo;
struct Lexeme;
struct Chunk;

void dump_repo(CSourceRepo* repo);
void dump_parse_tree(CNode* node);
void dump_inst_tree(CInstance * inst);
void dump_call_graph(CNodeFunction* node_func);
void dump_call_graph(CNodeClass* node_class);
void dump_lexeme(Lexeme& l);
void dump_chunk_tree(Chunk* chunk);
std::string escape(const char* a, const char* b);
