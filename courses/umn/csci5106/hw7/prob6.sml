Control.Print.printDepth := 100;
Control.Print.printLength := 100;

(* 
 * Solution to Problem 6, part 1.
 * Signature BTREE
 *)
signature BTREE = 
sig 
  type item
  (*datatype tree = Empty | Node of item * tree * tree*)
  type tree
  val initTree : unit -> tree
  val insert : item -> tree -> tree
  val member : item -> tree -> bool
  val print : tree -> unit
end

(* 
 * Solution to Problem 6, part 2.
 * Signature ITEM
 *)
signature ITEM =
sig
  type item
  val eq:  item * item -> bool
  val lss: item * item -> bool
  val printEle: item -> unit
end

(* 
 * Solution to Problem 6, part 3.
 * Functor BTree
 *)
functor BTree (Item : ITEM) : BTREE = 
struct
  type item = Item.item
  fun eq(lhs:item, rhs:item) = Item.eq(lhs, rhs)
  fun lss(lhs:item, rhs:item) = Item.lss(lhs, rhs)
  fun printEle(v:item) = Item.printEle(v)

  datatype tree = Empty | Node of item * tree * tree

  fun initTree _ = Empty;
  fun insert v Empty = Node(v, Empty, Empty)
    | insert v (Node(v1, lt, rt)) = 
      if eq(v, v1)
      then Node(v1, lt, rt)
      else if lss(v, v1)
           then Node(v1, (insert v lt), rt)
           else Node(v1, lt, (insert v rt));

  fun member v Empty = false
    | member v (Node(v1, lt, rt)) = 
      if eq(v, v1)
      then true
      else if lss(v, v1)
           then member v lt
           else member v rt;
  fun print Empty = ()
    | print (Node(v, lt, rt)) = 
      (print lt; printEle v; print rt);
end

(* 
 * Solution to Problem 6, part 4.
 * Structure IntItem
 *)
structure IntItem : ITEM =
struct
  type item = int
  val eq = (op =)
  val lss = (op <)
  val printEle = (fn x => (print (Int.toString(x)); print " "))
end

(* 
 * Solution to Problem 6, part 4.
 * Structure StrItem
 *)
structure StrItem : ITEM =
struct
  type item = string
  fun eq (lhs, rhs) = (lhs = rhs)
  fun lss (lhs:item, rhs:item) = (lhs < rhs)
  val printEle = (fn x => (print x; print " "))
end

(* 
 * Solution to Problem 6, part 5. 
 * Test cases for integer binary tree
 *)
structure IntTree = BTree(IntItem);

print "(Test-IntTree) Init a tree as intT1 and insert 10 to intT1: ";
val intT1 = IntTree.initTree();
val intT1 = IntTree.insert 10 intT1;
IntTree.print intT1;

print "(Test-IntTree) Insert 5 to intT1 and assign the ret val to intT2: ";
val intT2 = IntTree.insert 5 intT1;
IntTree.print intT2;

print "(Test-IntTree) Check if intT1 contains 10 and 5: ";
IntTree.member 10 intT1;
IntTree.member 5 intT1;

(*
val intT2 = IntTree.insert 2  intT2;
val intT2 = IntTree.insert 6  intT2;
val intT2 = IntTree.insert 10 intT2;
val intT2 = IntTree.insert 11 intT2;
val intT2 = IntTree.insert 15 intT2;
val intT2 = IntTree.insert 9  intT2;
*)

print "(Test-IntTree) Insert more values to intT2 and update intT2: ";
val intT2 =
  (IntTree.insert 9
    (IntTree.insert 15
      (IntTree.insert 11
        (IntTree.insert 10
          (IntTree.insert 6
            (IntTree.insert 2 intT2))))));
IntTree.print intT2;

(* 
 * Solution to Problem 6, part 5. 
 * Test cases for string binary tree
 *)
structure StrTree = BTree(StrItem);
print "(Test-StrTree) Init a tree as strT1 and insert string to it: ";
val strT1 = StrTree.initTree();
val strT1 = StrTree.insert "str_test" strT1;
StrTree.print strT1;

print "(Test-StrTree) Insert another str to strT1 and assign the ret val to strT2: ";
val strT2 = StrTree.insert "str_test_longer" strT1;
StrTree.print strT2;

print "(Test-StrTree) Check if strT1 contains 'str_test' and 'str_test_longer': ";
StrTree.member "str_test" strT1;
StrTree.member "str_test_longer" strT1;

(*
val strT2 = StrTree.insert "s1"  strT2;
val strT2 = StrTree.insert "str_test"  strT2;
val strT2 = StrTree.insert "str_tesz" strT2;
val strT2 = StrTree.insert "2s" strT2;
val strT2 = StrTree.insert "abc" strT2;
val strT2 = StrTree.insert "sqt"  strT2;
*)

print "(Test-StrTree) Insert more values to strT2 and update strT2: ";
val strT2 = 
  StrTree.insert "sqt" 
    (StrTree.insert "abc" 
      (StrTree.insert "2s" 
        (StrTree.insert "str_testz"
          (StrTree.insert "str_test"
            (StrTree.insert "s1" strT2)))));
StrTree.print strT2;

