Suppose we have an C++ program and we want to determine if a trivial(ish) translation of that program into Verilog will work correctly. We can assume that our source file and our translated file both compile correctly in their respective languages - syntax-checking is out of scope for now.

If some operation has similar syntax but different semantics in the two languages, our program will
likely break.

----------

## Suppose we have this class:

```
class ThingC {
public:

  int state_a;
  int state_b;

  void update(bool reset) {
    if (reset) {
      state_a = 0;
      state_b = 0;
    } else {
      state_a = state_b + 1;
      state_b = state_a + 1;
    }
  }
}
```

We can translate this literally into Verilog like so:

```
module ThingV (input logic clock, input logic reset);

  int state_a;
  int state_b;

  always_ff @(posedge clock) begin
    if (reset) begin
      state_a <= 0;
      state_b <= 0;
    end else begin
      state_a <= state_b + 1;
      state_b <= state_a + 1;
    end
  end

endmodule
```

However, the two versions will _not_ produce the same results.


|  | ThingC.state_a | ThingC.state_b | ThingV.state_a | ThingV.state_b |
|--|----------|----------|----------|----------|
|cycle 0|0|0|0|0|
|cycle 1|1|2|1|1|
|cycle 2|3|4|2|2|
|cycle 3|5|6|3|3|

The difference is in the **'<='** operator, which Verilog calls the 'non-blocking assignment' operator. In C, assignments take effect immediately. In Verilog, assignments can take effect immediately ('blocking' assignment), or they can be deferred until the end of the current simulation step ('non-blocking' assignment).

Reading from a variable in Verilog that has been non-blockingly will return the **old** value, whereas a read in C++ will always return the **new** value.

If we want an assignment in C to have the same side-effects as an assignment in Verilog, we have to ensure that the C++ code never reads from a variable after it has been non-blockingly assigned.

If we look at variable B in our example, we can see that "```b = a + 1```" reads from ```a``` after it has been assigned - in C++ this returns the **new** value of ```a```, in Verilog this returns the **old** value of ```a```.

If we can guarantee that our C++ code never reads the **new** value of a variable, then we can be more confident that our converted code will behave the same in both languages.

-----
## But wait, can't you just...

If you already know some Verilog, you're probably thinking "But wait, you don't _have_ to use '<=', you can use regular '=' instead" - and you're right, sort of.

The problem with using '=' shows up if we split our always_ff block like so:

```
module ThingV (input logic clock, input logic reset);

  int state_a;
  int state_b;

  always_ff @(posedge clock) begin : update_a
    if (reset) begin
      state_a = 0;
    end else begin
      state_a = state_b + 1;
    end
  end

  always_ff @(posedge clock) begin : update_b
    if (reset) begin
      state_b = 0;
    end else begin
      state_b = state_a + 1;
    end
  end

endmodule
```

This will compile and simulate, but the output is **undefined**. Verilog guarantees that both always_ff blocks will be evaluated when the clock goes high, but it does **not** define what order they are evaluated in.

By changing '<=' to '=', we have created an ordering dependency between "update_a" and "update_b". If update_a is evaluated first, we get "a = 1, b = 2" after the first clock cycle. If update_b is evaluated first, we get "a = 2, b = 1".

For this reason, virtually all Verilog style guides forbid using blocking assignments inside "@(posedge clock)" blocks.

-----

Let's look at a similar example, this time starting with Verilog

```
module ThingV (output logic signal_out);

  always_comb begin : set_signal_2
    signal_out = 2;
  end

  always_comb begin : set_signal_3
    signal_out = 3;
  end

endmodule
```

which could be represented in C++ as something like

```
class ThingC {
public:
  int signal_out;

  void set_signal_2() {
    signal_out = 2;
  }

  void set_signal_3() {
    signal_out = 3;
  }
}
```

However, again, the two implementations do not match up. The C++ version is well-defined, if a bit pointless. The Verilog version won't even compile. In Verilog-land, "set_signal_2" and "set_signal_3" happen _simultaneously_. What should the value of signal_out be if it is simultaneously set to 2 and 3? There's no good answer, so the Verilog compiler reports an error.

-----

We seem to be at an impasse. We've identified two examples in which syntactically-matching C++ and Verilog do not produce identical results, so in order to translate from C++ to Verilog it would seem we need to do something more sophisticated than a "trivial" translation. However, we can work around this by instead limiting the subset of what we'll accept as valid C++.

If you look back at the examples, you'll note that I've named variables written in "always_ff" as "state_*" and variables written in "always_comb" as "signal_*". We'll continue this pattern by classifying our C++ member variables as "signal" or "state" variables based on where they're assigned in the corresponding Verilog version.

To ensure that our converted C++ works correctly, we need to enforce some rules for our signals and states - I like to call these the "Read-Write Rules", or RWR for short.

<b>

1. State variables cannot be read after they're written. Doing so can cause the "C++ reads new state, Verilog reads old state" mismatch.

2. Signal variables cannot be written after they're read*. Doing so can cause the "multiple simultaneous assignment" error in Verilog.

</b>

\* 'read' in this sense also includes "made visible to external components via an 'output logic' port".

There are a couple strategies we could use to enforce these rules -

* Make some custom C++ types that trigger an assertion if the rules are broken. GateBoy does this in debug builds, but it's not a perfect solution. Even with 100% test coverage, there could still be _some_ path through the codebase that would break the rules.
* Analyze the codebase at compile time and try to prove that no matter what paths are taken, the rules are never broken. This is what Metron does.

Luckily for Metron we don't care about the _values_ stored in variables, only whether they're read or written and in what order. Let's start by looking at a trivial function with no branches:

```
class Thing {
public:
  int x;

  int update1() {
    int y = x + 3;
    x = 7;
    return y;
  }
};
```

We'll start by assigning the variable ```x``` a symbolic value of ```N```, for "Nothing has happened to this variable yet". When we encounter ```int y = x + 3```, we see a read of ```X``` so we'll concatenate a ```R``` onto our symbol, giving ```NR```. Next we see a write in ```x = 7```, so we concatenate a ```W``` giving ```NRW```. If we go through all possible symbol combinations, we get the following state transition table after some simplifications:

```
// The N symbol is irrelevant once we see a read or write
N + R == R
N + W == W

// The number of sequential reads or writes doesn't matter
R + R == R
W + W == W

// Read+write and write+read concatenate
R + W == RW
W + R == WR

// Concatenation order doesn't matter
(A+B)+C == A+(B+C)

// RWR and WRW are invalid symbols for reasons related to the rules above, explained shortly
RW + R == X
WR + W == X
```

So for branchless code, our possible symbols are ```N```, ```R```, ```W```, ```RW```, ```WR```, and ```X```. Next let's consider what happens when we see branching code:

```
class Thing {
public:
  int x;

  int update2(bool y) {
    if (y) {
      return x;
    } else {
      x = 2;
      return 1;
    }
  }
};
```

The variable ```x``` has symbol ```R``` in the ```if``` branch and ```W``` in the ```else``` branch, so its symbol at the end of the function is... well, we'll just create a new symbol ```R|W```, where ```|``` means something like "in parallel with". Like the branchless case, we can simplify a lot of the symbol combinations -

```
// We don't need to keep track of multiple branches that do the same thing
A | A == A

// Branch order is irrelevant, branches are commutative
A | B == B | A == A|B
(A|B) | (C|D) == A|B|C|D

// Invalid symbols override everything
A | X == X
```

Ignoring the ```X``` symbol for a moment, we have 5 'serial' symbols ```N, R, W, RW, WR``` and we can concatenate any subset of them together to produce a 'parallel' signal, so there are at least 2^5 = 32 possible parallel symbols. A lot of those can be converted into ```X``` because of the implications of the read-write rules above - for example, ```RW|WR``` can be disallowed as it would _always_ break one of the rules.

(Yes, I'm aware the logic is getting a bit circular here since we haven't talked about how to classify variables into 'signals' and 'states' yet, bear with me)

After eliminating invalid symbols, we are left with the following set of 12 valid symbols:

```N, R, W, RW, WR, N|R, N|W, N|RW, R|W, R|RW, W|RW, W|WR```

We need one additional simplification rule before we can handle all possible code paths:

```
A|B + C == AC|BC
```

and now we can write out our full state transition tables for serial and parallel code:

| + | R | W |
|---|---|---|
|N|R|W|
|R|R|RW|
|W|WR|W|
|RW|X|RW|
|WR|WR|X|
|N\|R|R|W\|RW|
|N\|W|X|W|
|N\|RW|X|W\|RW|
|R\|W|X|W\|RW|
|R\|RW|X|RW|
|W\|RW|X|W\|RW|
|W\|WR|WR|X|

| \| | N | R | W | RW | WR |
|----|---|---|---|----|----|
|  N | N | N\|R | N\|W | N\|RW | X |
|  R | N\|R | R | X | R\|RW | X |
|  W | N\|W | X | W | W\|RW | W\|WR
| RW | N\|RW | R\|RW | W\|RW | RW | X |
| WR | X | X | W\|WR | X | WR |
