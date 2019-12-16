Control.Print.printDepth := 100;
Control.Print.printLength := 100;

fun assignx exp state = let
                          val (x, y) = state
                        in
                          ((exp state), y)
                        end;

fun assigny exp state = let
                          val (x, y) = state
                        in
                          (x, (exp state))
                        end;

fun seq stmt1 stmt2 = (fn x => (stmt2 (stmt1 x)));

fun ifstat test stmt1 stmt2 = 
  (fn x => if (test x) then (stmt1 x) else (stmt2 x));

(* 
 * Solution to Problem 4, part 1.
 * whilestat and repeatstat
 *)
fun whilestat test stmt = 
  (* (fn x => if (test x) then (whilestat test stmt (stmt x)) else x); *)
  ifstat test (fn x => (whilestat test stmt (stmt x))) (fn x => x);

fun repeatstat test stmt = 
  (fn x => (whilestat test stmt (stmt x)));

(* 
 * Solution to Problem 4, part 2.
 * Encodings of GCD and factorial.
 *)

(* GCD in C: 
 *
 * gcd(x, y) {
 *   if (x == 0) return y;
 *   else if (y == 0) return x;
 *   else
 *     while (x != y) {
 *       if (x > y)
 *         x -= y;
 *       else
 *         y -= x;
 *     }
 *
 *   return x;
 * }
 *)

(*
val gcd = 
  (fn (a, b) =>
    (whilestat
      (fn (x, y) => (x <> y)) 
      (ifstat 
        (fn (x, y) => (x > y)) 
        (assignx (fn (x, y) => (x - y)))
        (assigny (fn (x, y) => (y - x)))
      )
    ) (a, b)
  );
*)

val gcd = 
  (ifstat
    (fn (x, y) => (x = 0))
    (assignx (fn (x, y) => y))
    (ifstat
      (fn (x, y) => (y = 0)) 
      (assigny (fn (x, y) => x))
      (whilestat
        (fn (x, y) => (x <> y)) 
        (ifstat 
          (fn (x, y) => (x > y)) 
          (assignx (fn (x, y) => (x - y)))
          (assigny (fn (x, y) => (y - x)))
        )
      )
    )
  );

(* factorial in C: 
 *
 * factor(n) {
 *   int res = 1, i = 1;
 *   while(i <= n) {
 *     res *= i;
 *     i++;
 *   }
 *   return res;
 * }
 *
 *)

val fact = 
  (fn n => 
    (seq 
      (assignx (fn (x,y) => 1)) 
      (seq 
        (assigny (fn (x,y) => 1))
        (whilestat 
          (fn(x,y) => (y <= n))
          (seq
            (assignx (fn(x, y) => (x * y)))
            (assigny (fn(x, y) => (y + 1))) 
          )
        )    
      )
    ) (0, 0)
  );

(* 
 * Solution to Problem 4, part 3.
 * Run the encodings.
 *)
print "(Test-GCD) GCD between 0 and 19: ";
gcd (0, 19);
print "(Test-GCD) GCD between 25 and 0: ";
gcd (25, 0);
print "(Test-GCD) GCD between 12 and 64: ";
gcd (12, 64);
print "(Test-GCD) GCD between 5 and 19: ";
gcd (5, 19);

print "(Test-Fact) Factorial 0: ";
fact 0;
print "(Test-Fact) Factorial 1: ";
fact 1;
print "(Test-Fact) Factorial 5: ";
fact 5;
print "(Test-Fact) Factorial 10: ";
fact 10;


repeatstat () () 
