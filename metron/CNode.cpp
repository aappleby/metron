// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CNode.hpp"

#include <ctype.h>

#include "metrolib/core/Log.h"
#include "metron/Cursor.hpp"

using namespace matcheroni;

//------------------------------------------------------------------------------

std::string_view CNode::get_name() const {
  NODE_ERR("Can't get name for CNode base class");
  return "<CNode>";
}

//------------------------------------------------------------------------------
