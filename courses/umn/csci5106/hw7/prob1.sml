Control.Print.printDepth := 100;
Control.Print.printLength := 100;

(* 
 * Solution to Problem 1, part 1.
 * A type for representing binary search trees of any kind of data
 *)
datatype 'a bst = Empty
		| Node of 'a * 'a bst * 'a bst;

(* 
 * Solution to Problem 1, part 2.
 * Function "member" to check if a value exists in the given BST.
 *)
fun member _ Empty lss eq              = false
  | member x (Node (v, lt, rt)) lss eq = 
    if eq x v
    then true
    else if lss x v
         then member x lt lss eq
         else member x rt lss eq;
		 				       
(* 
 * Solution to Problem 1, part 3.
 * Function "insert" to insert a new element into the given BST.
 *)
fun insert x Empty lss eq             = Node(x, Empty, Empty)
  | insert x (Node(v, lt, rt)) lss eq = 
    if eq x v
    then Node(v, lt, rt) 
    else if lss x v
         then Node(v, (insert x lt lss eq), rt)
         else Node(v, lt, (insert x rt lss eq));

(* 
 * Solution to Problem 1, part 4.
 * Function "printtree" to print the given BST on the screen
 *)
fun printtree Empty printEle             = ()
  | printtree (Node(v, lt, rt)) printEle = 
    (printtree lt printEle; printEle v; (print " "); printtree rt printEle);

(*
 * Solution to Problem1, functions for different types
 *)
val lssInt = fn x:int => fn y:int => x < y;
val eqInt = fn x:int => fn y:int => x = y;
val printInt = fn x:int => print (Int.toString(x));

val lssStr = fn x:string => fn y:string => x < y;
val eqStr = fn x:string => fn y:string => x = y;
val printStr = fn x:string => print x;

(* 
 * Solution to Problem1, test cases for integers
 *)
print "(Test-IntTree) Init a tree as intT1 and insert 10 to intT1: ";
val intT1 = insert 10 Empty lssInt eqInt;
printtree intT1 printInt;

print "(Test-IntTree) Insert 5 to intT1 and assign the ret val to intT2: ";
val intT2 = insert 5 intT1 lssInt eqInt;
printtree intT2 printInt;

print "(Test-IntTree) Check if intT1 contains 10 and 5: ";
member 10 intT1 lssInt eqInt;
member 5 intT1 lssInt eqInt;

print "(Test-IntTree) Insert more values to intT2 and update intT2: ";
(*
val intT2 = insert 2 intT2 lssInt eqInt;
val intT2 = insert 6 intT2 lssInt eqInt;
val intT2 = insert 10 intT2 lssInt eqInt;
val intT2 = insert 11 intT2 lssInt eqInt;
val intT2 = insert 15 intT2 lssInt eqInt;
val intT2 = insert 9 intT2 lssInt eqInt;
*)
val intT2 = 
  (insert 9 (
    insert 15 (
      insert 11 (
        insert 10 (
          insert 6 (
            insert 2 intT2 lssInt eqInt 
          ) lssInt eqInt 
        ) lssInt eqInt 
      ) lssInt eqInt 
    ) lssInt eqInt
  ) lssInt eqInt);
printtree intT2 printInt;

(* 
 * Solution to Problem1, test cases for strings
 *)
print "(Test-StrTree) Init a tree as strT1 and insert string to it: ";
val strT1 = insert "str_test" Empty lssStr eqStr;
printtree strT1 printStr;

print "(Test-StrTree) Insert another str to strT1 and assign the ret val to strT2: ";
val strT2 = insert "str_test_longer" strT1 lssStr eqStr;
printtree strT2 printStr;

print "(Test-StrTree) Check if strT1 contains 'str_test' and 'str_test_longer': ";
member "str_test" strT1 lssStr eqStr;
member "str_test_longer" strT1 lssStr eqStr;

print "(Test-StrTree) Insert more values to strT2 and update strT2: ";
(*
val strT2 = insert "s1" strT2 lssStr eqStr;
val strT2 = insert "str_test" strT2 lssStr eqStr;
val strT2 = insert "str_tesz" strT2 lssStr eqStr;
val strT2 = insert "2s" strT2 lssStr eqStr;
val strT2 = insert "abc" strT2 lssStr eqStr;
val strT2 = insert "sqt" strT2 lssStr eqStr;
*)
val strT2 = 
  (insert "sqt" (
    insert "abc" (
      insert "2s" (
        insert "str_tesz" (
          insert "str_test" (
            insert "s1" strT2 lssStr eqStr 
          ) lssStr eqStr 
        ) lssStr eqStr 
      ) lssStr eqStr 
    ) lssStr eqStr
  ) lssStr eqStr);
printtree strT2 printStr;
