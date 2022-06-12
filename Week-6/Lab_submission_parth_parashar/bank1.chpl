//-----------------------------------------------------------------------------
// Program code for CS 415/515 Parallel Programming, Portland State University.
//-----------------------------------------------------------------------------

// A deposit-withdraw program.
//
// Usage: ./bank
//
//
use Random;
var rs = new RandomStream(uint); // create a random stream of unsigned int
var val = rs.getNext(); // get a random unsigned int

int total;  // account balance

void deposit(int i) {
  var amount = rand() % DEPOSIT_AMT;
  var oldtotal = total;
  total += amount;
  writef("Deposit-%d %5d (%3d -> %3d)\n", i, amount, oldtotal, total);
}

void withdraw(int i) {
  var amount = rand() % WITHDRAW_AMT;
  var oldtotal = total;
  if (amount < total) {
    total -= amount;
    writef("Withdraw-%d %4d (%3d -> %3d)\n", i, amount, oldtotal, total);
  } else {
    writef("Withdraw-%d %4d (%3d) **aborted**\n", i, amount, oldtotal);
  }
}

int main() {
  total = INIT_BALANCE;
  srand(time(NULL));
  for (int i = 1; i <= NUM_DEPOSITS; i++)
    deposit(i);
  for (int i = 1; i <= NUM_WITHDRAWS; i++)
    withdraw(i);
}
