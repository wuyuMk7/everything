% Solution to problem 1.
append([], L, L).
append([X|L1], L2, [X|L3]) :- append(L1, L2, L3).

% Solution to problem 1, part 1 - 11.2 part(a).
% The third element of a list.
third([_|L], X) :- append([_, X], _, L).

% Solution to problem 1, part 2 - 11.2 part(d).
% Whether a list is a concatenation of three copies 
% of the same sublist.
subThree(L) :- append(L1, L2, L), append(L1, L1, L2), !.

% Test cases for problem 1.
% :- begin_tests(lists).
% :- use_module(library(lists)).
% test(t1_1_1) :- 
%   third([1,2,3,4,5,6,7,8], X), 
%   write('The third element is '),
%   write(X).
% test(t1_2_1) :- 
%   subThree([1,2,3,4,5,6]).
% test(t1_2_2) :-
%   subThree([1,2,3,1,2,3,1,2]).
% test(t1_2_3) :-
%   subThree([1,2,3,1,2,3,1,2,3]).
% :- end_tests(lists).

% Solution to problem 2.
select([X|L1], X, L1).
select([X|L1], Y, [X|L2]) :-
  select(L1, Y, L2).

% Solution to problem 2, part 1
% Check permutation between two lists.
isPerm([], []).
isPerm([X|L1], L2) :-
  select(L2, X, L3),
  isPerm(L1, L3),
  !.

% Solution to problem 2, part 2
% Check if a list has an even number of elements.
even([]).
even([_|L]) :- not(even(L)).

% Test cases fro problem 2.
% :- begin_tests(lists).
% :- use_module(library(lists)).
% test(t2_1_1) :- 
%   isPerm([], [1,2,3]).
% test(t2_1_2) :- 
%   isPerm([1,2,3,4,5], [3,5,2,4,1]).
% test(t2_1_3) :- 
%   isPerm([1,2,3,4,5], [6,2,3,4,1]).
% test(t2_1_4) :- 
%   isPerm([1,2,3,4,5,6], [1,2,3,4,5]).
% test(t2_1_5) :- 
%   isPerm([1,2,3,4], [1,2,3,4,5]).
% test(t2_2_1) :- 
%   even([]).
% test(t2_2_2) :-
%   even([1,2,3,4,5]).
% test(t2_2_3) :-
%   even([1,2,3,4,5,6]).
% :- end_tests(lists).

% Solution to problem 4, part 3
% Check if a logic expression is true 
% given an assignment L of truth values. 
isTrue(var(X), [X|_]) :- !.
isTrue(var(X), [_|L]) :- 
  isTrue(var(X), L).
isTrue(and(E1, E2), L) :- 
  isTrue(E1, L), isTrue(E2, L).
isTrue(or(E1, E2), L) :-
  (isTrue(E1, L), !); isTrue(E2, L).
isTrue(not(E), L) :-
  not(isTrue(E, L)).

% Solution to problem 4, part 4
% Generate variable list given a logic expression.
merge([], L, L).
merge([X|L1], L2, [X|L3]) :-
 merge(L1, L2, L3), not(member(X, L3)), !.
merge([X|L1], L2, L3) :-
 merge(L1, L2, L3), member(X, L3), !.

varsOf(var(X), [X]).
varsOf(and(E1, E2), L) :- 
  varsOf(E1, L1), varsOf(E2, L2), merge(L1, L2, L).
varsOf(or(E1, E2), L) :-
  varsOf(E1, L1), varsOf(E2, L2), merge(L1, L2, L).
varsOf(not(E), L) :-
  varsOf(E, L).
  
% Solution to problem 4, part 5
% Check if a logic expression is tautology.
isAssignment([], []).
isAssignment([X|L], [X|L2]) :- isAssignment(L, L2).
isAssignment([_|L], L2) :- isAssignment(L, L2).

isNotTaut(E) :- 
  varsOf(E, V),
  isAssignment(V, L),
  not(isTrue(E, L)).

isTaut(E) :- not(isNotTaut(E)).

% Test cases fro problem 4.
% :- begin_tests(lists).
% :- use_module(library(lists)).
% test(t4_3_1) :- 
%   isTrue(var(x), [x]).
% test(t4_3_2) :- 
%   isTrue(and(var(x), not(var(x))), [x]).
% test(t4_4_1, [true(X==[x,y])]) :- 
%   varsOf(and(var(x), not(var(y))), X),
%   write('The variable list is '),
%   writeln(X).
% test(t4_4_2, [true(X==[z,s,f,x,y])]) :-
%   varsOf(and(or(and(not(var(z)),var(s)), not(or(var(f), and(var(y), var(x))))), or(var(x),var(y))),X),
%   write('The variable list is '),
%   writeln(X).
% test(t4_5_1) :-
%   isTaut(and(var(x), not(var(x)))).
% test(t4_5_2) :-
%   isTaut(or(var(x), not(var(x)))).
% test(t4_5_3) :-
%   isTaut(or(var(x), not(var(y)))).
% :- end_tests(lists).
