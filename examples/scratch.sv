`include "metron/metron_tools.sv"

// We need to support very basic preprocessor macros or else dealing with mixed
// languages will be a huge pain in the butt.

module Module
(
);
endmodule

/*
#ifdef METRON_SV

  UNLEXABLE STUFF HERE

#else

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

#endif
*/
