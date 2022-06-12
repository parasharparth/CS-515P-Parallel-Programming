//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// Matrix multiplication (shared-memory version)
//  - approach 1: with loops
//  - approach 2: with reduce
//
use BlockDist;

config const N = 8;
var D = {1..N, 1..N};
const D1 = D dmapped Block(D);
const D2 = D dmapped Block({1..N, 1..N});
const D3 = D dmapped Block({1..N, 1..N});

var a: [D] int;
var b: [D1] int;
var c: [D2] int;
var d: [D3] int;

//var a, b, c, d: [D] int;

// initialization
a = [(i,j) in D] i + j;
b = [(i,j) in D] j;
c = 0;
d = 0;

// approach 1
forall (i,j) in D do
  for k in 1..N do
    c(i,j) += a(i,k) * b(k,j);

// approach 2
d = [(i,j) in D] + reduce [k in 1..N] a(i,k) * b(k,j);

writeln("total = ", + reduce c);
writeln("total = ", + reduce d);

//code for verification
write("Locale:");
for (i,j) in c.domain do
writef("%2i", c(i,j).locale.id);
writeln();