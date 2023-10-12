% представление данных (N mod 4) + 1 = 3, 2 вариант

:-['three.pl'].
:- encoding(utf8).

% Напечатать средний балл для каждого предмета

% поиск оценки ученика по предмету Subject
get_one_mark([], _, _).
get_one_mark([grade(Subject, Grade)|_], Subject, Grade) :- !.
get_one_mark([_|T], Subject, Grade) :- get_one_mark(T, Subject, Grade).

% получение оценки предмета Subject для студента
get_mark(Subject, Grade) :- student(_, _, List), get_one_mark(List, Subject, Grade).

% получение списка всех оценок по предмету Subject
get_all_mark(Subject, Mark) :- findall(Grade, get_mark(Subject, Grade), Mark).

% средняя оценка по предмету Subject
get_midl_mark(Subject, Mark) :- get_all_mark(Subject, Mark_list), sum(Mark_list, N), length(Mark_list, L), Mark is N / L.

% список всех предметов
get_subject(X) :- findall(A, subject(A, _), X).

% вывести средний балл по всем предметам
write_all([]) :- !.
write_all([X|T]) :- subject(X, Name), write(Name), write(': '), get_midl_mark(X, Mark), write(Mark), write('\n'), write_all(T).
write_all_subject :- get_subject(X), write_all(X).

% сумма списка
sum([], 0).
sum([X|Y], N) :- sum(Y, N1), N is N1 + X.



% Для каждой группы, найти количество не сдавших студентов

% количество долгов у студента
bad_student(_, [], 0).
bad_student(Elem, [grade(_, Elem)|Tail], Count) :- bad_student(Elem, Tail, N1), Count is N1 + 1.
bad_student(Elem, [grade(_, X)|Tail], Count) :- X \= Elem, bad_student(Elem, Tail, Count).

% перебор по студентам
chance_loader([], 0).
chance_loader([X|Tail], Count) :- bad_student(2, X, Y), chance_loader(Tail, N1), Y \= 0, Count is N1 + 1.
chance_loader([X|Tail], Count) :- bad_student(2, X, Y), chance_loader(Tail, Count), Y = 0.

% список оценок всех студентов группы
very_bad(Group, A) :- findall(Y, student(Group, _, Y), A).

% вывести долги по всем группам
write_all_ba(Group) :- write(Group), write(': '), very_bad(Group, A), chance_loader(A, N), write(N), write('\n').
write_all_subject_ba :- write_all_ba(101), write_all_ba(102), write_all_ba(103), write_all_ba(104).



% Найти количество не сдавших студентов для каждого из предметов

% количество вхождений в список
n_count(_, [], 0).
n_count(Elem, [Elem|Tail], Count) :- n_count(Elem, Tail, N1), Count is N1 + 1.
n_count(Elem, [X|Tail], Count) :- X \= Elem, n_count(Elem, Tail, Count).

% количество плохих оценок по предмету
get_bad_subject(Subject, N) :- get_all_mark(Subject, Mark), n_count(2, Mark, N).

% вывести долги по всем предметам
write_all_bad([]) :- !.
write_all_bad([X|T]) :- subject(X, Name), write(Name), write(': '), get_bad_subject(X, N), write(N), write('\n'), write_all_bad(T).
write_all_subject_bad :- get_subject(X), write_all_bad(X).
