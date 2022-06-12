//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// Domain Map Example
//
// linux> ./domainmap -nl 4
//

use BlockDist;

const D: domain(1) = {1..16};
const D1 = D dmapped Block(D);
const D2 = D dmapped Block({2..16});
const D3 = D dmapped Block({1..12});

var a:  [D] int;
var b1: [D1] int;
var b2: [D2] int;
var b3: [D3] int;

forall e in a do e = here.id;
forall e in b1 do e = here.id;
forall e in b2 do e = here.id;
forall e in b3 do e = here.id;

writeln("a:  ", a);
writeln("b1: ", b1);
writeln("b2: ", b2);
writeln("b3: ", b3);