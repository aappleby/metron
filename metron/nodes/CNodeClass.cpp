#include "metron/nodes/CNodeClass.hpp"

#include "metron/CSourceRepo.hpp"
#include "metron/nodes/CNodeStruct.hpp"

CNode* CNodeClass::resolve(std::vector<CNode*> path) {
  auto front = path[0];
  path.erase(path.begin());

  if (path.empty()) {
    if (auto f = get_field(front->name)) return f;
    if (auto f = get_function(front->name)) return f;
  }

  if (auto f = get_field(front->name)) {
    if (auto c = f->get_type_class()) {
      return c->resolve(path);
    }

    if (auto s = f->get_type_struct()) {
      return s->resolve(path);
    }

    assert(false);
  }

  return nullptr;
}
