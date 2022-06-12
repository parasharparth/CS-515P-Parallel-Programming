//----------------------------------------------------------------------------- 
// Program code for CS 415/515 Parallel Programming, Portland State University.
//----------------------------------------------------------------------------- 

// Print out info for all locales
//

for loc in Locales do
  writeln("Locale ", loc.id, ": ", loc.name,
              " (with ", loc.numPUs(true,true), " cores)");
