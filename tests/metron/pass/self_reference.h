#include "metron/metron_tools.h"

// A module should be able to have a noconvert-tagged method that returns a
// pointer to its type

class Module {
public:
  /* metron_noconvert */ Module* get_this();
};
