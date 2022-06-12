config const N = 8;
const D = {1..N, 1..N};
const PD = D.expand(1,1);
var a:[PD] real;
proc print(a:[]) {
  for i in a.domain.dim(0) {
    for j in a.domain.dim(1) do
      writef("%5.2dr", a[i,j]);
    writeln();
  }
}

a[PD.dim(0), 0] = 1.0;
a[0, PD.dim(1)] = 2.0;
writeln("Init:");
print(a);

config const epsilon = 0.01;
config const verbose = false;

var delta: real;
var t = 0;
do {
  forall ij in D do{  // no need to update padded elements
    a[ij] = (a[ij+(0,1)]+a[ij+(0,-1)]+a[ij+(1,0)]+a[ij+(-1,0)])/4.0;
  }
  t += 1;
  if (verbose) {
    print(a);
  }
} while (delta > epsilon);

writeln("Result: t=", t);
print(a);