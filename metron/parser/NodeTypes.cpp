#include "NodeTypes.hpp"

using namespace matcheroni;
using namespace parseroni;

void CNodeClass::init(const char* match_name, SpanType span, uint64_t flags) {
  CNode::init(match_name, span, flags);

  for (auto c : this) {
    if (auto n = c->as_a<CNodeDeclaration>()) {
      decls.push_back(n);
    }
    if (auto n = c->as_a<CNodeFunction>()) {
      methods.push_back(n);
    }
  }

}
