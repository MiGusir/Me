% swipl
% consult('prolog.pl').

length([], 0).
length([_|Y], N) :- length(Y, N1), N is N1 + 1.

member([A|_], A).
member([_|Y], A) :- member(Y, A).

append([], X, X).
append([A|Y], X, [A|Z]) :- append(Y, X, Z).

remove([A|B], A, B).
remove([S|C], A, [S|B]) :- remove(C, A, B).

permute([], []).
permute(L, [X|T]) :- remove(L, X, E), permute(E, T).

% Предикаты обработки списков (вариант вычислять как (N mod 19)+1 = 11)
% Удаление элемента с заданным номером

delete_at(N, List, Result) :- length(List, Len), N =< Len, N > 0, N1 is N - 1, N2 is Len - N, length(Prefix, N1), length(Suffix, N2), append(Prefix, [_|Suffix], List), append(Prefix, Suffix, Result).

del_n_elem([_|L], 0, L).
del_n_elem([A|L], N, R) :- N1 is N-1, del_n_elem(L, N1, R0), R=[A|R0].

% Предикаты обработки числовых списков (вариант вычислять как (N + 5) mod 20 + 1 = 16)
% Вычисление суммы двух векторов-списков (с учетом возможного несовпадения размерностей)

sum_vectors_at([], [], []).
sum_vectors_at(Xs, Ys, Result) :- length(Xs, LengthXs), length(Ys, LengthYs), LengthXs > LengthYs, append(X1, X2, Xs), length(X1, LengthYs), sum_vectorsHelper(Ys, X1, Result_do), append(Result_do, X2, Result).
sum_vectors_at(Xs, Ys, Result) :- length(Xs, LengthXs), length(Ys, LengthYs), LengthYs > LengthXs, append(Y1, Y2, Ys), length(Y1, LengthXs), sum_vectorsHelper(Xs, Y1, Result_do), append(Result_do, Y2, Result).
sum_vectors_at(Xs, Ys, Result) :- length(Xs, LengthXs), length(Ys, LengthYs), LengthYs = LengthXs, sum_vectorsHelper(Ys, Xs, Result).
sum_vectorsHelper([], [], []).
sum_vectorsHelper([X|Xs], [Y|Ys], [Sum|Result]) :- Sum is X + Y, sum_vectorsHelper(Xs, Ys, Result).

sum_vectors([], [], []).
sum_vectors([X|Xs], [], [X|Result]) :- sum_vectors(Xs, [], Result).
sum_vectors([], [Y|Ys], [Y|Result]) :- sum_vectors([], Ys, Result).
sum_vectors([X|Xs], [Y|Ys], [Sum|Result]) :- Sum is X + Y, sum_vectors(Xs, Ys, Result).
