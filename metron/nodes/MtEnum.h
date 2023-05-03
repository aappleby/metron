#pragma once
#include "metron/nodes/MtNode.h"

//------------------------------------------------------------------------------

struct MtEnum {
  MtEnum(const MnNode& n);
  std::string name();
  MnNode node;
};
