empty([]).
cmp(L, L).

find(X, [X|_]).
find(X, [_|L]) :- find(X, L).

len([], 0).
len([_|L], Res) :- 
    len(L, ResL), 
    Res is ResL+1.

at(0, [X|_], X).
at(N, [_|L], X) :-
    N1 is N-1,
    at(N1, L, X).

head(L, X) :-
    at(0, L, X).
    
last(L, X) :-
    len(L, Length),
    Pos is Length-1,
    at(Pos, L, X).

take(_, 0, []).
take([X|L], Count, Res) :-
    NextCount is Count - 1,
    Res=[X|NextRes],
    take(L, NextCount, NextRes).

slice(L, Start, End, Res) :-
    take(L, Start, Taken),
    append(Taken, Rest, L),
    Count is End-Start,
    take(Rest, Count, Res).

append([], L, L).
append([X|L1], L2, [X|L3]) :-
    append(L1, L2, L3).

odd([], []).
odd([X], [X]).
odd([X|L], Res) :-
    L=[_|L1],
    Res=[X|Res1],
    odd(L1, Res1).

:- 
    write("Function odd test.\n"),
    write("- Test 1, input sequence is empty, output sequence is: "),
    odd([], L), write(L), cmp([], L), write(". Test1 passed.\n"),
    write("- Test 2, input sequence is a, output sequence is: "),
    odd([a], L2), write(L2), cmp([a], L2), write(". Test2 passed.\n"),
    write("- Test 3, input sequence is abcdefg, output sequence is: "),
    odd([a,b,c,d,e,f,g], L3), write(L3), 
    cmp([a,c,e,g], L3), write(". Test3 passed.\n"),
    write("- Test 4, input sequence is abcdefgh, output sequence is: "),
    odd([a,b,c,d,e,f,g,h], L4), write(L4), 
    cmp([a,c,e,g], L4), write(". Test4 passed.\n").
