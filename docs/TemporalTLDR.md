Hi, my name is Austin Appleby and I'm the author of MurmurHash (a commonly used hash function), GateBoy (a gate-level Game Boy simulator), Metron (a very experimental C++ to SystemVerilog transpiler), and other neat things that you can find at https://github.com/aappleby.

For the last few years I've been doing some projects and experiments to try and understand more about how time is represented in programming languages. I've also been reading a ton of research papers, and I've come up with something interesting enough to be worth writing about. I'll eventually have a full-length paper that goes into more detail, but for now you can consider this post to be a TL;DR summary of that paper.

----------

In 1978, John Backus wrote an article titled "Can Programming Be Liberated from the von Neumann Style? A Functional Style and Its Algebra of Programs", in which he sketched out three ideas for programming language styles that departed from the "von Neumann" style of "word-at-a-time" modification of program state. The paper overall was hugely influential (4000+ citations on Google Scholar) yet the third example Backus presented -  "Applicative State Transition" (AST) - was referenced in only a few dozen papers and got relatively little traction. This is a shame, as it's a conceptual model that programmers (particularly hardware programmers) are familiar with in a less formal fashion - your program is a blob of state, you have a function that computes a "new" state from an "old" state, and you "run" the program by atomically replacing the old state with the newly computed state.

In 1990, Leslie Lamport published "A Temporal Logic of Actions" (TLA), a research paper that defined a logical foundation for expressing amd proving properties of concurrent programs. The model of computation used in TLA is similar to Backus's AST, but presented from a more proof-oriented point of view. TLA programs are blobs of state and sets of logical expressions about pairs of states called "actions". If we plug state A and state B into an action and the expression evaluates to true, then the program can "get from" state A to state B. To distinguish between the two states that are inputs to an action, Lamport uses "primed" and "unprimed" variables - ```x``` refers to the "old" state, ```x'``` refers to the "new" state. The expressions are not assignments in the imperative sense (```x' = x + 1``` means the same thing as ```1 = x' - x```) and thus don't need to be evaluated in any particular order, but they're still capable of modeling very complex behaviors in real programs. TLA proofs can be written and proved in Lamport's languages TLA+ and PlusCal, but they are not executable languages in the usual sense.

I believe that these two papers and their fundamental ideas - programs as state transitions, programs as expresions about adjacent states - form the basis of a programming paradigm that we're mostly all familiar with but that so far has gone unnamed. I'd like to call it "Temporal Programming", in honor of Lamport's paper.

Temporal programs, as I am attempting to define them, are of the form ```x' = f(x)``` - a recurrence relation. Like the von Neumann model of imperative languages and the lambda calculus of functional languages, recurrence relations are a mathematically simple but general way of expressing computation. ```x``` represents the entire state of our program, ```f()``` is the pure function that computes the next state of the program, and ```x'``` represents that next state. Replace ```x``` with "strip of paper" and ```f()``` with "Turing machine" and it should be immediately clear that recurrences are Turing-complete.

The two tenets that I think a temporal programming language should follow are:

1. Temporal programs must change state atomically as in Backus's paper. Intermediate states should not be expressible or visible from within the language.

2. Temporal programs must explicitly distinguish between "old" and "new" state as in Lamport's paper. A statement like ```x = x + 1``` would not be valid, while ```x' = x + 1``` would be.

I believe that languages based on these two tenents have the potential to be far more expressive and powerful than  the original papers might suggest. Representing programs as atomic state transitions removes the possibility of large classes of bugs related to synchronization and concurrency. Enforcing a distinction between "old" and "new" state decouples declaration order from evaluation order - the program ```a' = b + 1; b' = a + 1;``` is the same regardless of which statement comes first. Programs that adhere to these tenets should in theory be far more optimizable than what would be possible in a procedural language, as the compiler is essentially given the entire evaluation graph for the program up front and can completely reschedule evaluations in whatever order is most efficient. They should also be more amenable to formal TLA-type proofs since their computation model is much closer to what TLA+ expects.

Right now there are no languages available that follow both of these two tenets fully, though there are a huge number of languages and paradigms described in the literature that fit to some degree. To explore these ideas further, I will be building a minimal language based on a subset of C++, with the addition of one character that's currently unused:

```
struct Program {
  int a = 0;
  int b = 0;

  void tick() {
    a@ = b + 1;
    b@ = a + 1;
  }
};
```

Blocks containing the "@" character are temporal blocks - from an imperative viewpoint, assignments to primed ("atted" sounds strange) variables don't take effect until after all temporal blocks have been evaluated. The language transpiler will insert temporary variables and reorder expressions to make the code compatible with C++ or Verilog as needed while ensuring the transpiled code is functionally identical to the original. To follow along with the metronome theme of my other projects, I intend to call the language "MetroC".

To be absolutely clear, I am not claiming to have invented any of these concepts - I am merely assigning a name to a set of patterns that most programmers have already come across and am trying to express those patterns in a new and useful way. If you have suggestions for what the definition of "Temporal Programming" should be, I am very interested in hearing them.

-Austin
