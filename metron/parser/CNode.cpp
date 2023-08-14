// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CNode.hpp"

#include "Cursor.hpp"

#include "metrolib/core/Log.h"

using namespace matcheroni;

CHECK_RETURN Err CNode::emit(Cursor& c) {
  LOG_R("CNode::emit() %s\n", match_name);
  return ERR("Don't know how to emit a %s", typeid(*this).name());
}
