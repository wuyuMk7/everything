datatype MyStr = Empty | :: of char * MyStr;

val emptyStr = 
  Empty;
val oneElementStr = 
  #"a" :: Empty : MyStr;
val oddElementsStr = 
  #"a" :: #"b" :: #"k" :: #"f" :: #"#" :: Empty : MyStr;
val evenElementsStr = 
  #"a" :: #"b" :: #"k" :: #"f" :: #"#" :: #"&" :: Empty : MyStr;

fun len Empty = 0
  | len (_::L : MyStr) = 1 + len L;
  
len oddElementsStr;

fun empty Empty = true
  | empty _ = false;

empty emptyStr;

fun compare (Empty, Empty) = 0
  | compare (Empty, _) = ~1
  | compare (_, Empty) = 1
  | compare (X1::L1 : MyStr, X2::L2 : MyStr) = 
    if X1 = X2 then compare(L1, L2)
    else if X1 > X2 then 1
    else ~1;

compare(oddElementsStr, evenElementsStr);

fun toString Empty = ""
  | toString (X::L) = (Char.toString X) ^ toString L;

toString oddElementsStr;
  
fun find (Empty, _) = ~1
  | find (l, x : char) = 
    let fun findIndex (Empty, _, _) = ~1
          | findIndex (X::L, x, n : int) = 
            if X = x then n else findIndex(L, x, n+1)
    in findIndex(l, x, 0) end;
  
find(oddElementsStr, #"#");

fun take (Empty, _) = Empty
  | take (X::L : MyStr, 0) = Empty
  | take (X::L : MyStr, n) = 
    if n < 0 then Empty else X::take(L, n-1);

toString(take(oddElementsStr, 3));
    
fun drop (Empty, _) = Empty
  | drop (L, 0) = L
  | drop (X::L : MyStr, n) = 
    if n < 0 then X::L else drop(L, n-1);

toString(drop(oddElementsStr, 3));
    
(* s and e should be indices. *)
fun slice (Empty, _, _) = Empty
  | slice (L, s, e) = 
    if s < 0 then take(L, e) else take(drop(L,s), e-s);

toString(slice(oddElementsStr, 1, 2)); 
  
fun append (Empty, L) = L
  | append (L, Empty) = L
  | append (X1::L1 : MyStr, L2 : MyStr) = X1 :: append(L1, L2); 
  
toString(append(oddElementsStr, oddElementsStr));

fun odd Empty = Empty
  | odd (X::Empty) = X::Empty
  | odd (X1::X2::L : MyStr) = X1::odd (L);
  
print "\nFunction odd test.";
print "- Test 1, input sequence is empty, output sequence is: ";
print(toString(Empty) ^ "\n");
print "- Test 2, input sequence is a, output sequence is: ";
print(toString(odd(oneElementStr)) ^ "\n");
print "- Test 3, input sequence is abkf#, output sequence is: ";
print(toString(odd(oddElementsStr)) ^ "\n");
print "- Test 4, input sequence is abkf#&, output sequence is: ";
print(toString(odd(evenElementsStr)) ^ "\n");
