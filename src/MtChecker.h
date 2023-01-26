#pragma once
#include <assert.h>

#include <map>
#include <string>

#include "Err.h"
#include "MtNode.h"
#include "MtTracer.h"

struct MtModule;
struct MtMethod;
struct MtField;

//------------------------------------------------------------------------------

class MtChecker {
 public:
  static bool ends_with_break(MnNode n);
  static bool has_return(MnNode n);
  static bool has_non_terminal_return(MnNode n);
};

//------------------------------------------------------------------------------
