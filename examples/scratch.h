#include "metron/metron_tools.h"

// We need to support very basic preprocessor macros or else dealing with mixed
// languages will be a huge pain in the butt.

#ifdef METRON_SV

//module Submod();
//endmodule

#else

#endif

#if 0

struct Chunk {

  ChunkType type;

  Lexeme preproc;
  std::vector<Lexeme>* lexemes;
  Chunk* parent = nullptr;
  std::vecetor<Chunk*> children;
};

#endif

/*


#ifndef METRON_SV

  the quick brown fox

  #ifdef BLAH
    the quick brown fox
  #endif

  the quick brown fox

  #ifdef BLAH
    the quick brown fox
  #else
    the quick brown fox
  #endif

  the quick brown fox

  #ifndef BLAH
    the quick brown fox
  #endif

  the quick brown fox

#else

  UNLEXABLE STUFF HERE

#endif












#ifdef METRON_CPP

#else

#endif



*/
