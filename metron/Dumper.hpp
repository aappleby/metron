#pragma once

struct CNode;
struct CInstance;
struct CNodeFunction;
struct CNodeClass;

void dump_parse_tree(CNode* node);
void dump_inst_tree(CInstance * inst);
void dump_call_graph(CNodeFunction* node_func);
void dump_call_graph(CNodeClass* node_class);
