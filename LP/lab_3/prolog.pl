% swipl
% consult('prolog.pl').


prolong([X|T], [Y,X|T]) :- move(X,Y),not(member(Y,[X|T])).

% Поиск в глубину
dfs_path([X|T],X,[X|T]).
dfs_path(P,Y,R):- prolong(P,P1), dfs_path(P1,Y,R).
dfs(X,Y,P):- dfs_path([X],Y,P).

% Поиск в ширину
bfs_path([[X|T]|_],X,[X|T]). 
bfs_path([P|QI],X,R) :- findall(Z,prolong(P,Z),T),append(QI,T,QO),!,bfs_path(QO,X,R).
bfs_path([_|T],Y,L) :- bfs_path(T,Y,L).
bfs(X,Y,P) :- bfs_path([[X]],Y,P).

% Поиск с итерационным заглублением
id_path([X | T], X, [X | T], _).
id_path(P, B, R, N):- N > 0, prolong(P, P1), N1 is N - 1, id_path(P1, B, R, N1).
id(s(X, Y, Z), B, R) :- length(X,L), N is 3 * L / 2, id_path([s(X, Y, Z)], B, R, N).


% может начинаться с черного или белого вагона
bw(X) :- b(X); w(X).

b([]).
b([X|T]) :- X==black,w(T).    % если X черный, то следующий должен быть белым

w([]).
w([X|T]) :- X==white,b(T).    % если X белый, то следующий должен быть черным

first([],[]).
first([X|_],X).

reverse_lists([],[]).
reverse_lists([s(X, Y, Z)|T], [s(P,Q,R)|RT]) :- 
    reverse(X,P), 
    reverse(Y,Q), 
    reverse(Z,R), 
    reverse_lists(T,RT).

% первый аргумент - вагоны слева, второй - в тупике, третий - справа
move(s([A|AT],B,C), s(AT,B,[A|C])) :- first(C,X), X\=A.
move(s(A,[B|BT],C), s(A,BT,[B|C])) :- first(C,X), X\=B.
move(s([A|AT],B,C), s(AT,[A|B],C)) :- first(C,X), X==A.

solve(X,R):- 
    reverse(X,X1), 
    permutation(X1,Y), 
    bw(Y),
    get_time(Start), 
    id(s(X1,[],[]),     % здесь bfs, dfs или id - в зависимости от вида поиска
    s([],[],Y),W), 
    get_time(End), 
    T is End - Start,
    write('run time:'), write(T), write('\n'),
    reverse_lists(W,W1), 
    reverse(W1,R), !.