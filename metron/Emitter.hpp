#pragma once

#include <set>
#include <string>
#include <stack>
#include <map>
#include "metrolib/core/Err.h"

#include "metron/CNode.hpp"
#include "metron/Cursor.hpp"

struct CNodeAccess;
struct CNodeAssignment;
struct CNodeBuiltinType;
struct CNodeCall;
struct CNodeCase;
struct CNodeClass;
struct CNodeClassType;
struct CNodeCompound;
struct CNodeConstant;
struct CNodeConstructor;
struct CNodeDeclaration;
struct CNodeDefault;
struct CNodeDoWhile;
struct CNodeEnum;
struct CNodeExpStatement;
struct CNodeField;
struct CNodeFieldExpression;
struct CNodeFunction;
struct CNodeIdentifier;
struct CNodeIf;
struct CNodeKeyword;
struct CNodeNamespace;
struct CNodePrefixExp;
struct CNodePreproc;
struct CNodeQualifiedIdentifier;
struct CNodeReturn;
struct CNodeStruct;
struct CNodeSuffixExp;
struct CNodeSwitch;
struct CNodeTemplate;
struct CNodeTypedef;
struct CNodeUnion;
struct CNodeUsing;

//==============================================================================

struct Emitter {
  Emitter(CSourceRepo* repo, Cursor& c) : repo(repo), cursor(c) {
    id_map.push({
      {"signed",         "$signed"},
      {"unsigned",       "$unsigned"},
      {"clog2",          "$clog2" },
      {"pow2",           "2**" },
      {"readmemh",       "$readmemh" },
      {"value_plusargs", "$value$plusargs" },
      {"write",          "$write" },
      {"sign_extend",    "$signed" },
      {"zero_extend",    "$unsigned" },
      {"DONTCARE",       "'x" },
      {"#include",       "`include"},
      {"#define",        "`define"},
      {"#ifndef",        "`ifndef"},
      {"#endif",         "`endif"},
      {"unsigned int",   "int unsigned"},
    });

    elide_type.push(false);
    elide_value.push(false);
    override_size.push(0);
  }

  Err emit_everything();
  Err emit_default(CNode* node);
  Err emit_children(CNode* node);
  Err emit_dispatch(CNode* node);
  Err emit_dispatch2(CNode* node);

  Err emit(CNodeAssignment* node);
  Err emit(CNodeBuiltinType* node);
  Err emit(CNodeCall* node);
  Err emit(CNodeCase* node);
  Err emit(CNodeClass* node);
  Err emit(CNodeClassType* node);
  Err emit(CNodeCompound* node);
  Err emit(CNodeConstant* node);
  Err emit(CNodeDeclaration* node);
  Err emit(CNodeEnum* node);
  Err emit(CNodeExpStatement* node);
  Err emit(CNodeField* node);
  Err emit(CNodeFieldExpression* node);
  Err emit(CNodeFunction* node);
  Err emit(CNodeIdentifier* node);
  Err emit(CNodeKeyword* node);
  Err emit(CNodeNamespace* node);
  Err emit(CNodePrefixExp* node);
  Err emit(CNodePreproc* node);
  Err emit(CNodeQualifiedIdentifier* node);
  Err emit(CNodeReturn* node);
  Err emit(CNodeStruct* node);
  Err emit(CNodeSuffixExp* node);
  Err emit(CNodeSwitch* node);
  Err emit(CNodeTemplate* node);
  Err emit(CNodeUnion* node);
  Err emit(CNodeUsing* node);

  Err emit(const char* fmt, ...);
  Err emit_splice(CNode* n);
  Err emit_splice2(CNode* n);

  Err emit_parameter_list(CNodeClass* node);
  Err emit_block(CNodeCompound* node, std::string ldelim, std::string rdelim);
  Err emit_bit_extract(CNodeCall* node);
  Err emit_hoisted_decls(CNodeCompound* node);
  Err emit_call_arg_bindings(CNodeCompound* node, CNode* child);
  Err emit_component(CNodeField* node);
  Err emit_module_ports(CNodeClass* node);
  Err emit_function_ports(CNodeFunction* f);
  Err emit_field_ports(CNodeField* f, bool is_output);
  Err emit_init(CNodeFunction* node);
  Err emit_always_comb(CNodeFunction* node);
  Err emit_always_ff(CNodeFunction* node);
  Err emit_func(CNodeFunction* node);
  Err emit_task(CNodeFunction* node);
  Err emit_func_binding_vars(CNodeFunction* node);
  Err emit_params_with_return(CNodeFunction* node);

  Err emit_cat(CNodeCall* node);
  Err emit_sra(CNodeCall* node);
  Err emit_dup(CNodeCall* node);
  Err emit_slice(CNodeCall* node);
  Err emit_submod_call(CNodeCall* node);




  Err check_at(CNode* n);
  Err check_done(CNode* n);
  Err emit_gap(CNode* n);
  Err skip_gap(CNode* n);
  Err skip_over(CNode* n);
  Err skip_over2(CNode* n);
  Err skip_to(CNode* n);
  Err emit_replacement(CNode* n, const std::string& s);
  Err emit_replacement2(CNode* n, const std::string& s);

  Err emit_replacement(CNode* n, const char* fmt, ...);
  Err emit_replacement2(CNode* n, const char* fmt, ...);
  Err emit_raw(CNode* n);
  Err emit_raw2(CNode* n);


  Err emit_format(CNode* n, const char* fmt, ...);


  Err comment_out(CNode* n) {
    if (n == nullptr) return Err();

    Err err = check_at(n);
    err << cursor.emit_print("/*") << emit_raw(n) << cursor.emit_print("*/");

    return err << check_done(n);
  }

  Err comment_out2(CNode* n) {
    Err err;
    if (n == nullptr) return err;
    err << comment_out(n) << emit_gap(n);
    return err;
  }

  CSourceRepo* repo = nullptr;
  Cursor& cursor;
  std::set<std::string> preproc_vars;
  std::stack<int> override_size;
  using string_to_string = std::map<std::string, std::string>;
  std::stack<string_to_string> id_map;
  std::stack<bool> elide_type;
  std::stack<bool> elide_value;
};

//==============================================================================
