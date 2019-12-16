Control.Print.printDepth := 100;
Control.Print.printLength := 100;

(* 
 * Solution to Problem 5.
 * Tail recusive version fibonacci number
 *)

fun fib 0 = 0
  | fib 1 = 1
  | fib n = fib (n-1) + fib (n-2);

fun fibcc 0 k = k 0
  | fibcc 1 k = k 1
  | fibcc n k = 
      fibcc (n-1) (fn x => fibcc (n-2) (fn y => k (x+y)));

(* Test cases *)
print "(Test) fibcc 1 (fn x => x): ";
fibcc 1 (fn x => x);
print "(Test) fibcc 8 (fn x => x): ";
fibcc 8 (fn x => x);
print "(Test) fibcc 15 (fn x => x): ";
fibcc 15 (fn x => x);
print "(Test) fib 8: ";
fib 8;
print "(Test) fib 15: ";
fib 15;


