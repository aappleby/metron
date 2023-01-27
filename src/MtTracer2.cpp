#include "MtTracer2.h"
#include "MtNode.h"
#include "MtMethod.h"
#include "MtChecker.h"

//------------------------------------------------------------------------------

MtTracer2::MtTracer2(MtModLibrary* lib, MtModuleInstance* root_inst, bool verbose)
: lib(lib),
  root_inst(root_inst),
  verbose(verbose) {
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer2::trace_method(MtMethod* method) {
  Err err;

  auto method_inst = root_inst->get_method(method->name());

  auto node_body = method->_node.get_field(field_body);
  if (MtChecker::has_non_terminal_return(node_body)) {
    err << ERR("Method %s has non-terminal return\n", method->cname());
  }

  //err << trace_sym_function_definition(method_ctx, method->_node);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer2::trace_declarator(MtMethodInstance* inst, MnNode node) {
  Err err;

  switch (node.sym) {
    case sym_identifier:
      //err << trace_identifier(ctx, node, CTX_READ);
      break;
    case sym_init_declarator:
      //err << trace_sym_init_declarator(ctx, node);
      break;
    default:
      //err << trace_default(ctx, node);
      break;
  }

  return err;
}





























//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer2::trace_sym_function_definition(MtMethodInstance* inst, MnNode node) {
  Err err;
  assert(node.sym == sym_function_definition);

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);
  auto node_body = node.get_field(field_body);

  err << trace_sym_compound_statement(inst, node_body);

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer2::trace_sym_compound_statement(MtMethodInstance* inst, MnNode node) {
  Err err;
  assert(node.sym == sym_compound_statement);

  bool noconvert = false;
  bool dumpit = false;

  for (const auto& child : node) {
    if (noconvert) { noconvert = false; continue; }
    if (dumpit) { child.dump_tree(); dumpit = false; }
    if (child.sym == sym_comment && child.contains("dumpit")) { dumpit = true; }
    if (child.sym == sym_comment && child.contains("metron_noconvert")) { noconvert = true; }

    if (child.sym == sym_declaration) {
      //err << trace_sym_declaration(ctx, child);
    } else if (child.is_statement()) {
      //err << trace_statement(ctx, child);
    } else {
      //err << trace_default(ctx, child);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtTracer2::trace_sym_declaration(MtMethodInstance* inst, MnNode node) {
  Err err;
  assert(node.sym == sym_declaration);

  auto node_type = node.get_field(field_type);
  auto node_decl = node.get_field(field_declarator);

  err << trace_declarator(inst, node_decl);

  return err;
}

//------------------------------------------------------------------------------
