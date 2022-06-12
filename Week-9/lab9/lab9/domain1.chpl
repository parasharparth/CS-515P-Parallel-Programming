//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// Domain and array examples.
//

var D = {1..6};        // declare a domain
var a: [D] int;        // declare an int array over domain
a = [i in D] i;        // a = [1,2,3,4,5,6]

writeln("D = ", D, "\t// a = [i in D] i;");
writeln("a = ", a, "\t// a has ", a.size, " elements\n");

// redefine domain D, what happens to a?
D = {2..4};
writeln("D = ", D, "\t// redefine domain D");
writeln("a = ", a, "\t// a has ", a.size, " elements now\n");

// redefine domain D, what happens to a?
D = {4..8};
writeln("D = ", D, "\t// redefine domain D again");
writeln("a = ", a, "\t// a has ", a.size, " elements now");

