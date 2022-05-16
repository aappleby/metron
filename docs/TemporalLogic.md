Suppose we have a plain, unannotated C++ program and we want to determine if a trivial translation of that program into Verilog will work correctly. We can assume that our source file is valid C++ - syntax-checking is out of scope for now. If some operation has similar syntax but different semantics in the two languages, our program will likely break.

For example, we can take this class

```
class ThingC {
public:

  int reg_a;
  int reg_b;

  void update(bool reset) {
    if (reset) {
      reg_a = 0;
      reg_b = 0;
    } else {
      reg_a = reg_b + 1;
      reg_b = reg_a + 1;
    }
  }
}
```

and do a trivial line-by-line translation into Verilog like this:

```
module ThingV (input logic clock, input logic reset);

  int reg_a;
  int reg_b;

  always_ff @(posedge clock) begin : update
    if (reset) begin
      reg_a <= 0;
      reg_b <= 0;
    end else begin
      reg_a <= reg_b + 1;
      reg_b <= reg_a + 1;
    end
  end

endmodule
```

However, the two versions will _not_ produce the same results.


|         | C reg_a | C reg_b | Verilog reg_a | Verilog reg_b |
|---------|-----------|-----------|-----------------|-----------------|
| cycle 0 |0|0|0|0|
| cycle 1 |1|2|1|1|
| cycle 2 |3|4|2|2|
| cycle 3 |5|6|3|3|

The difference is in the ```<=``` operator, which Verilog calls the 'non-blocking assignment' operator. In C, assignments take effect immediately. In Verilog, assignments can take effect immediately ('blocking' assignment), or they can be deferred until the end of the current simulation step ('non-blocking' assignment).

Reading from a variable in Verilog that has been non-blockingly assigned will return the **old** value, whereas a read in C++ will always return the **new** value. If we look at variable ```b``` in our example, we can see that ```b = a + 1``` reads from ```a``` after it has been assigned - in C++ this returns the **new** value of ```a```, in Verilog this returns the **old** value of ```a```.

-----
## But wait, can't you just...

If you already know some Verilog, you're probably thinking "But wait, you don't _have_ to use ```<=```, you can use regular ```=``` instead" - and you're right, sort of. The problem with using ```=``` shows up if we split our always_ff block like so:

```
module ThingV (input logic clock, input logic reset);

  int reg_a;
  int reg_b;

  always_ff @(posedge clock) begin : update_a
    if (reset) begin
      reg_a = 0;
    end else begin
      reg_a = reg_b + 1;
    end
  end

  always_ff @(posedge clock) begin : update_b
    if (reset) begin
      reg_b = 0;
    end else begin
      reg_b = reg_a + 1;
    end
  end

endmodule
```

This will compile, but the output is **undefined**. Verilog guarantees that both always_ff blocks will be evaluated when the clock goes high, but it does **not** define what order they are evaluated in.

By changing ```<=``` to ```=```, we have created an ordering dependency between "update_a" and "update_b". If update_a is evaluated first, we get "a = 1, b = 2" after the first clock cycle. If update_b is evaluated first, we get "a = 2, b = 1".

For this reason, virtually all Verilog style guides forbid using blocking assignments inside clocked blocks.

-----

Let's look at another example:

```
class ThingC {
public:
  int wire_out;

  void set_wire_to_2() {
    wire_out = 2;
  }

  void set_wire_to_3() {
    wire_out = 3;
  }
}
```

which could be represented in Verilog as

```
module ThingV (output logic wire_out);

  always_comb begin : set_wire_2
    wire_out = 2;
  end

  always_comb begin : set_wire_3
    wire_out = 3;
  end

endmodule
```

However, again, the two implementations do not match up. The C++ version is well-defined, if a bit pointless. The Verilog version won't even compile. In Verilog-land, "set_wire_2" and "set_wire_3" happen _continuously_ and _simultaneously_. What should the value of wire_out be if it is simultaneously set to 2 and 3? There's no good answer, so the Verilog compiler reports an error.

-----

We've barely started and we already seem to be at an impasse. We've identified two examples in which syntactically-matching C++ and Verilog do not produce identical results, so in order to translate from C++ to Verilog it would seem we need to do something more sophisticated than a "trivial" translation. However, we can work around this by instead limiting the subset of what we'll accept as our C++ input.

If you look back at the examples, you'll note that I've named variables written in "always_ff" as "reg_*" and variables written in "always_comb" as "wire_*". This is because in hardware-land, we have two fundamentally different types of "variables" to deal with. Registers are able to store state and will hold their values across clock cycles if not changed. Wires are wires, tiny strips of metal that only hold a value as long as they're being 'driven' by the output of a logic gate or register.

If we knew in advance which of our C++ member variables were going to turn into registers and which were going to turn into wires, we'd have an easier time of things. We could try and enforce these two rules -

1. Registers can't be read after they're written (


Let's pretend that we know in advance which C++ member variables will become "wires" (assigned in always_comb after conversion) and which will become "registers" (

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

We'll start by assigning the variable ```x``` a symbolic value of ```N```, for "Nothing has happened to this variable yet". When we encounter ```int y = x + 3```, we see a read of ```X``` so we'll concatenate a ```R``` onto our symbol, giving ```NR```. Next we see a write in ```x = 7```, so we concatenate a ```W``` giving ```NRW```. If we go through all possible symbol combinations, we get the following symbol transition table after some simplifications:

```
// The N symbol is irrelevant once we see a read or write
N + R == R
N + W == W

// The number of sequential reads or writes doesn't matter
R + R == R
W + W == W
RW + W = RW
WR + R + WR

// Read+write and write+read concatenate
R + W == RW
W + R == WR

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
R | R == R
W | W == W

// Branch order is irrelevant, branches are commutative
R | W == W | R == R|W
(N|R) | (W|RW) == N|R|W|RW

// Invalid symbols override everything
* | X == X
```

Ignoring the ```X``` symbol for a moment, we have 5 'serial' symbols ```N, R, W, RW, WR``` and we can concatenate any subset of them together to produce a 'parallel' symbol, so there are 2^5-1 = 31 possible parallel symbols (we can safely ignore the empty set).

```
N |   |   |    |    + R = NR               = R
N |   |   |    |    + W = NW               = W
  | R |   |    |    + R = RR               = R
  | R |   |    |    + W = RW               = RW
N | R |   |    |    + R = NR|RR            = R
N | R |   |    |    + W = NW|RW            = W|RW
  |   | W |    |    + R = WR               = WR
  |   | W |    |    + W = WW               = W
N |   | W |    |    + R = NR|WR            = R|WR
N |   | W |    |    + W = NW|WW            = W
  | R | W |    |    + R = RR|WR            = R|WR
  | R | W |    |    + W = RW|WW            = W|RW
N | R | W |    |    + R = NR|RR|WR         = R|WR
N | R | W |    |    + W = NW|RW|WW         = W|RW
  |   |   | RW |    + R = RWR              = X
  |   |   | RW |    + W = RWW              = RW
N |   |   | RW |    + R = NR|RWR           = X
N |   |   | RW |    + W = NW|RWW           = W|RW
  | R |   | RW |    + R = RR|RWR           = X
  | R |   | RW |    + W = RW|RWW           = RW
N | R |   | RW |    + R = R|RR|RWR         = X
N | R |   | RW |    + W = NW|RW|RWW        = W|RW
  |   | W | RW |    + R = WR|RWR           = X
  |   | W | RW |    + W = WW|RWW           = W|RW
N |   | W | RW |    + R = NR|WR|RWR        = X
N |   | W | RW |    + W = NW|WW|RWW        = W|RW
  | R | W | RW |    + R = RR|WR|RWR        = X
  | R | W | RW |    + W = RW|WW|RWW        = W|RW
N | R | W | RW |    + R = NR|RR|WR|RWR     = X
N | R | W | RW |    + W = NW|RW|WW|RWW     = W|RW
  |   |   |    | WR + R = WRR              = WR
  |   |   |    | WR + W = WRW              = X
N |   |   |    | WR + R = NR|WRR           = R|WR
N |   |   |    | WR + W = NW|WRW           = X
  | R |   |    | WR + R = RR|WRR           = R|WR
  | R |   |    | WR + W = RW|WRW           = X
N | R |   |    | WR + R = R|RR|WRR         = R|WR
N | R |   |    | WR + W = W|RW|WRW         = X
  |   | W |    | WR + R = WR|WRR           = WR
  |   | W |    | WR + W = WW|WRW           = X
N |   | W |    | WR + R = R|WR|WRR         = R|WR
N |   | W |    | WR + W = W|WW|WRW         = X
  | R | W |    | WR + R = RR|WR|WRR        = R|WR
  | R | W |    | WR + W = RW|WW|WRW        = X
N | R | W |    | WR + R = NR|RR|WR|WRR     = R|WR
N | R | W |    | WR + W = NW|RW|WW|WRW     = X
  |   |   | RW | WR + R = RWR|WRR          = X
  |   |   | RW | WR + W = RWW|WRW          = X
N |   |   | RW | WR + R = NR|RWR|WRR       = X
N |   |   | RW | WR + W = W|RWW|WRW        = X
  | R |   | RW | WR + R = RR|RWR|WRR       = X
  | R |   | RW | WR + W = RW|RWW|WRW       = X
N | R |   | RW | WR + R = NR|RR|RWR|WRR    = X
N | R |   | RW | WR + W = NW|RW|RWW|WRW    = X
  |   | W | RW | WR + R = WR|RWR|WRR       = X
  |   | W | RW | WR + W = WW|RWW|WRW       = X
N |   | W | RW | WR + R = NR|WR|RWR|WRR    = X
N |   | W | RW | WR + W = NW|WW|RWW|WRW    = X
  | R | W | RW | WR + R = RR|WR|RWR|WRR    = X
  | R | W | RW | WR + W = RW|WW|RWW|WRW    = X
N | R | W | RW | WR + R = NR|RR|WR|RWR|WRR = X
N | R | W | RW | WR + W = NW|RW|WW|RWW|WRW = X
```

With that table written out, we have enough information to fully trace a program. For each member variable we start with the N symbol and trace through each line of code. If we see a read or a write, we look up the current symbol in the table above and transition to the new symbol. If we see a branch, we trace through both sides of the branch independently and then parallel-merge the symbols together afterwards (just concatenate the N|R|W with the R|WR or whatever and then remove duplicates).

This works, but it's kinda clunky and it doesn't really give us insight into what the symbols _mean_. We can simplify things a bit further by taking advantage of redundancies in the table and by removing symbols that are impossible because they would always break the RWR rules - for example, ```RW|WR``` can be disallowed as it contains both a read-after-write and a write-after-read which means it can't be categorized as either a "register" or "wire" in Verilog-land. ```R|WR``` is out for similar reasons - ```WR``` implies "wire", but ```R``` implies "register" and it can't be both.

After removing all impossible symbols from the table, we get this -

```
// NONE
N |   |   |    |    + R = NR               = R      = INPUT
N |   |   |    |    + W = NW               = W      = OUTPUT

// INPUT
  | R |   |    |    + R = RR               = R      = INPUT
N | R |   |    |    + R = NR|RR            = R      = INPUT
  | R |   |    |    + W = RW               = RW     = REGISTER
N | R |   |    |    + W = NW|RW            = W|RW   = REGISTER

// OUTPUT
  |   | W |    |    + W = WW               = W      = OUTPUT
  |   | W |    |    + R = WR               = WR     = SIGNAL

// SIGNAL
  |   |   |    | WR + R = WRR              = WR     = SIGNAL
  |   | W |    | WR + R = WR|WRR           = WR     = SIGNAL
  |   |   |    | WR + W = WRW              = X      = INVALID
  |   | W |    | WR + W = WW|WRW           = X      = INVALID

// REGISTER
  | R | W |    |    + W = RW|WW            = W|RW   = REGISTER
N | R | W |    |    + W = NW|RW|WW         = W|RW   = REGISTER
  |   |   | RW |    + W = RWW              = RW     = REGISTER
N |   |   | RW |    + W = NW|RWW           = W|RW   = REGISTER
  | R |   | RW |    + W = RW|RWW           = RW     = REGISTER
N | R |   | RW |    + W = NW|RW|RWW        = W|RW   = REGISTER
  |   | W | RW |    + W = WW|RWW           = W|RW   = REGISTER
N |   | W | RW |    + W = NW|WW|RWW        = W|RW   = REGISTER
  | R | W | RW |    + W = RW|WW|RWW        = W|RW   = REGISTER
N | R | W | RW |    + W = NW|RW|WW|RWW     = W|RW   = REGISTER
  | R | W |    |    + R = RR|WR            = R|WR   = INVALID
N | R | W |    |    + R = NR|RR|WR         = R|WR   = INVALID
  |   |   | RW |    + R = RWR              = X      = INVALID
N |   |   | RW |    + R = NR|RWR           = X      = INVALID
  | R |   | RW |    + R = RR|RWR           = X      = INVALID
N | R |   | RW |    + R = R|RR|RWR         = X      = INVALID
  |   | W | RW |    + R = WR|RWR           = X      = INVALID
N |   | W | RW |    + R = NR|WR|RWR        = X      = INVALID
  | R | W | RW |    + R = RR|WR|RWR        = X      = INVALID
N | R | W | RW |    + R = NR|RR|WR|RWR     = X      = INVALID

// MAYBE
N |   | W |    |    + R = NR|WR            = R|WR   = INVALID
N |   | W |    |    + W = NW|WW            = W      = OUTPUT

// INVALID
N |   |   |    | WR + R = NR|WRR           = R|WR   = INVALID
N |   |   |    | WR + W = NW|WRW           = X      = INVALID
  | R |   |    | WR + R = RR|WRR           = R|WR   = INVALID
  | R |   |    | WR + W = RW|WRW           = X      = INVALID
N | R |   |    | WR + R = R|RR|WRR         = R|WR   = INVALID
N | R |   |    | WR + W = W|RW|WRW         = X      = INVALID
N |   | W |    | WR + R = R|WR|WRR         = R|WR   = INVALID
N |   | W |    | WR + W = W|WW|WRW         = X      = INVALID
  | R | W |    | WR + R = RR|WR|WRR        = R|WR   = INVALID
  | R | W |    | WR + W = RW|WW|WRW        = X      = INVALID
N | R | W |    | WR + R = NR|RR|WR|WRR     = R|WR   = INVALID
N | R | W |    | WR + W = NW|RW|WW|WRW     = X      = INVALID
```

Note that for each group, all symbols in that group followed by a read go to the same group, ditto for writes.

```
| series   | read    | write    |
|----------|---------|----------|
| NONE     | INPUT   | OUTPUT   |
| INPUT    | INPUT   | REGISTER |
| OUTPUT   | SIGNAL  | OUTPUT   |
| MAYBE    | INVALID | OUTPUT   |
| SIGNAL   | SIGNAL  | INVALID  |
| REGISTER | INVALID | REGISTER |

| parallel | NONE     | INPUT    | OUTPUT   | MAYBE    | SIGNAL  | REGISTER |
|----------|----------|----------|----------|----------|---------|----------|
| NONE     | NONE     | INPUT    | MAYBE    | MAYBE    | INVALID | REGISTER |
| INPUT    | INPUT    | INPUT    | REGISTER | REGISTER | INVALID | REGISTER |
| OUTPUT   | MAYBE    | REGISTER | OUTPUT   | MAYBE    | SIGNAL  | REGISTER |
| MAYBE    | MAYBE    | REGISTER | MAYBE    | MAYBE    | INVALID | REGISTER |
| SIGNAL   | INVALID  | INVALID  | SIGNAL   | INVALID  | SIGNAL  | INVALID  |
| REGISTER | REGISTER | REGISTER | REGISTER | REGISTER | INVALID | REGISTER |
```










































 A lot of those can be converted into ```X``` because of the implications of the read-write rules above

(Yes, I'm aware the logic is getting a bit circular here since we haven't talked about how to classify variables into registers and wires yet, bear with me)

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
|  R | N\|R | R | R\|W | R\|RW | X |
|  W | N\|W | R\|W | W | W\|RW | W\|WR
| RW | N\|RW | R\|RW | W\|RW | RW | X |
| WR | X | X | W\|WR | X | WR |

This isn't bad, but there's some redundancy here - we can further simplify things by putting symbols into groups:

```
NONE     := { N }
INPUT    := { R, N|R }
OUTPUT   := { W }
SIGNAL   := { WR, W|WR }
REGISTER := { RW, N|RW,
MAYBE    := { N|W }
```

RW       + w = RW
N|RW     + w = W|RW
R|W      + w = W|RW
R|RW     + w = RW
W|RW     + w = W|RW
N|R|W    + w = W|RW
N|R|RW   + w = W|RW
N|W|RW   + w = W|RW
R|W|RW   + w = W|RW
N|R|W|RW + w = W|RW


















To ensure that our converted C++ works correctly, we need to enforce some rules for our signals and states - I like to call these the "Read-Write Rules", or RWR for short.

<b>

1. All member variables can be classified into "state" variables or "signal" variables.

3. Signal variables cannot be written after they're read*. Doing so can cause the "multiple simultaneous assignment" error in Verilog.

2. State variables cannot be read after they're written. Doing so can cause the "C++ reads new state, Verilog reads old state" mismatch.

</b>

\* 'read' in this sense also includes "made visible to external components via an 'output logic' port".
