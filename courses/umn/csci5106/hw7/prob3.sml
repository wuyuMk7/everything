Control.Print.printDepth := 100;
Control.Print.printLength := 100;

(* 
 * Solution to Problem 3, part 1.
 * A type for representing logical expressions
 *)
datatype prop = T 
              | F 
              | Var of string 
              | And of prop * prop
              | Or  of prop * prop
              | Not of prop;

(* 
 * Solution to Problem 3, part 2.
 * In this part, I used a list to store all of propositional vars 
 * whose value is T(true). If a variable does not appear in this 
 * list, that means this variable is F(false).
 *)

(* 
 * Solution to Problem 3, part 3.
 * Function "eval" calculates the truth value given a logical exp 
 * and an assignment of truth values for propositional variables
 *)
fun eval T _      = T
  | eval F _      = F
  | eval (Var(var)) assignments    = let fun search (x:string) []  = F
                                           | search (x:string) (v::l) = 
                                               if x = v
                                               then T
                                               else search x l
                                     in 
                                       search var assignments
                                     end
  | eval (And(e1, e2)) assignments = if (eval e1 assignments) = T
                                     then (eval e2 assignments)
                                     else F
  | eval (Or(e1, e2)) assignments  = if (eval e1 assignments) = T
                                     then T
                                     else (eval e2 assignments)
  | eval (Not(e)) assignments      = if (eval e assignments) = T
                                     then F
                                     else T;

(* 
 * Solution to Problem 3, part 4.
 * Function "varsInExp" returns a list with all of 
 * propositional variables from a logical expression.
 * Function "mergeVarList" combines two lists from 
 * two logical expressions.
 * Function "varListLookup" checks if a variable
 * is in the given list or not.
 *)
fun varListLookup _ []                = false
  | varListLookup (var:string) (v::l) = (var = v) orelse
                                        (varListLookup var l);

fun mergeVarList [] [] ret      = ret
  | mergeVarList [] (x::l2) ret = if varListLookup x ret
                                  then mergeVarList [] l2 ret
                                  else mergeVarList [] l2 (x::ret)
  | mergeVarList (x::l1) l2 ret = if varListLookup x ret
                                  then mergeVarList l1 l2 ret
                                  else mergeVarList l1 l2 (x::ret);

fun varsInExp T             = []
  | varsInExp F             = []
  | varsInExp (Var(var))    = [var]
  | varsInExp (And(e1, e2)) = mergeVarList (varsInExp e1) (varsInExp e2) []
  | varsInExp (Or(e1, e2))  = mergeVarList (varsInExp e1) (varsInExp e2) []
  | varsInExp (Not(e))      = varsInExp e;

(* 
 * Solution to Problem 3, part 5.
 * Function "isTaut" checks if a logical expression is a tautology.
 *)
(*
fun isTaut e = let
                 val vars = varsInExp e;
                 fun isTautHelper e []     = if (eval e []) = T
                                             then true
                                             else false
                   | isTautHelper e (x::l) = if (eval e (x::l)) = T
                                             then (isTautHelper e l)
                                             else false;
               in
                 isTautHelper e vars
               end;
*)
exception NotTaut

fun isTaut e = let
                 val vars = varsInExp e;
                 fun isTautHelper cur nil    = if (eval e cur) = F
                                               then raise NotTaut
                                               else true
                   | isTautHelper cur (x::l) = (isTautHelper cur l) andalso
                                               (isTautHelper (x::cur) l);
               in
                 isTautHelper [] vars
               end
               handle NotTaut => false;

(* 
 * Solution to Problem 3, test cases 
 *)
print "(Test-part 1)Logical expressions: ";
val prop1 = Var("p");
val prop2 = Var("q");
val prop3 = T;

val e1 = (And(Or(prop1, prop2), Not(prop1)));

print "(Test-part 3)Evaluate e1 with variable q = T: ";
eval e1 ["q"];

print "(Test-part 4)List all of propositional variables in e1: ";
varsInExp e1;

print "(Test-part 5)Check if exp 'T' is a tautology: ";
isTaut T;
print "(Test-part 5)Check if exp e1 is a tautology: ";
isTaut e1;
print "(Test-part 5)Check if exp '(Or(k, Not(k)))' is a tautology: ";
isTaut (Or(Var("k"), Not(Var("k"))));
