#pragma once

struct MnNode;

//------------------------------------------------------------------------------

class MtChecker {
 public:
  static bool ends_with_break(MnNode n);
  static bool has_return(MnNode n);
  static bool has_non_terminal_return(MnNode n);
};

//------------------------------------------------------------------------------
