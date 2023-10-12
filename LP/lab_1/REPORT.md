# Отчет по лабораторной работе №1
## Работа со списками и реляционным представлением данных
## по курсу "Логическое программирование"

### студент: Гусаров М. И.

## Результат проверки

| Преподаватель     | Дата         |  Оценка       |
|-------------------|--------------|---------------|
| Сошников Д.В. |              |               |
| Левинская М.А.|              |               |

> *Комментарии проверяющих (обратите внимание, что более подробные комментарии возможны непосредственно в репозитории по тексту программы)*


## Введение

Списки в языке Пролог отличаются от принятых в императивных языках подходов к хранению данных в нескольких аспектах:
1. Структура: В языке Пролог списки являются рекурсивными структурами. Каждый список представляется в виде пары [Голова|Хвост], где Голова - первый элемент списка, а Хвост - список из оставшихся элементов.
2. Мутабельность: В языке Пролог списки являются неизменяемыми. Это означает, что, создав список, мы не можем изменять его содержимое. Всякий раз, когда нам нужно добавить, удалить или изменить элемент списка, создается новый список.
3. Итерация: В императивных языках обычно используются циклы (например, for, while) для обхода элементов в списке. В Прологе для обработки элементов списка используется рекурсия.

Таким образом, списки в Прологе предоставляют свой подход к хранению и работе с данными, основанный на рекурсии и неизменяемости.

Списки в Прологе по своей природе более похожи на односвязные списки в традиционных языках программирования. Односвязные списки и списки в Прологе оба являются коллекциями элементов, где каждый элемент содержит ссылку на следующий элемент в списке. Они обычно состоят из двух частей: головы (head), представляющей текущий элемент списка, и хвоста (tail), представляющего оставшуюся часть списка.

## Задание 1.1: Предикат обработки списка

`length(List, Size).` - предикат подсчитывает длинц списка

Примеры использования:
```prolog
92 ?- length([1,2,3,4,5,as], N).
N = 6.

93 ?- length(A, 7).             
A = [_, _, _, _, _, _, _].
```

Реализация:
```prolog
length([], 0).
length([_|Y], N) :- length(Y, N1), N is N1 + 1.
```

`Member(List, X).` - предикат проверяет принадлежность элемента списку

Примеры использования:
```prolog
94 ?- member([1,2,3,4,5,6], 4).
true ;
false.

95 ?- member([1,2,3,4,5,6], 99).
false.
```

Реализация:
```prolog
member([A|_], A).
member([_|Y], A) :- member(Y, A).
```

`append(List_1, List_2, X).` - предикат объединяет два списка

Примеры использования:
```prolog
97 ?- append([1,2,3,4],[a,s,d],Z).
Z = [1, 2, 3, 4, a, s, d].

98 ?- append([1,2,3,4], A, [1,2,3,4,f,d,s]).
A = [f, d, s].
```

Реализация:
```prolog
append([], X, X).
append([A|Y], X, [A|Z]) :- append(Y, X, Z).
```

`remove(List, X, B).` - предикат удаления элемента из списка

Примеры использования:
```prolog
99 ?- remove([1,2,3,4],3,A).
A = [1, 2, 4] ;
false.

100 ?- remove([1,2,3,4],A, [1, 2]).
false.

101 ?- remove([1,2,3,4],A, [1, 2, 4]).
A = 3 ;
false.
```

Реализация:
```prolog
remove([A|B], A, B).
remove([S|C], A, [S|B]) :- remove(C, A, B).
```

`permute(A, B).` - предикат всех перестановок элементов списка

Примеры использования:
```prolog
102 ?- permute([1,2,3],A).
A = [1, 2, 3] ;
A = [1, 3, 2] ;
A = [2, 1, 3] ;
A = [2, 3, 1] ;
A = [3, 1, 2] ;
A = [3, 2, 1] ;
false.
```

Реализация:
```prolog
permute([], []).
permute(L, [X|T]) :- remove(L, X, E), permute(E, T).
```

Предикаты обработки списков (вариант вычислять как (N mod 19)+1 = 11)
`delete_at(N, List, Result).` - удаление элемента с заданным номером на основе стандартных предикатов
`del_n_elem(List, N, Result).` - удаление элемента с заданным номером


Примеры использования:
```prolog
2 ?- delete_at(3, [1,2,3,4,5,6], A).
A = [1, 2, 4, 5, 6].
4 ?- del_n_elem([1,2,3,4,5], 2, A).
A = [1, 2, 4, 5] ;
false.
```

Реализация:
```prolog
delete_at(N, List, Result) :- length(List, Len), N =< Len, N > 0, N1 is N - 1, N2 is Len - N, length(Prefix, N1), length(Suffix, N2), append(Prefix, [_|Suffix], List), append(Prefix, Suffix, Result).

del_n_elem([_|L], 0, L).
del_n_elem([A|L], N, R) :- N1 is N-1, del_n_elem(L, N1, R0), R=[A|R0].
```

## Задание 1.2: Предикат обработки числового списка

Предикаты обработки числовых списков (вариант вычислять как (N + 5) mod 20 + 1 = 16)
`sum_vectors_at(A, B, C).` - вычисление суммы двух векторов-списков (с учетом возможного несовпадения размерностей) на основе стандартных предикатов
`sum_vectors(A, B, C).` - вычисление суммы двух векторов-списков (с учетом возможного несовпадения размерностей)


Примеры использования:
```prolog
5 ?- sum_vectors([1,2,3],[5,5,5], A).
A = [6, 7, 8] ;
false.

6 ?- sum_vectors_at([1,2,3],[5,5,5], A).
A = [6, 7, 8].
```

Реализация:
```prolog
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
```

## Задание 2: Реляционное представление данных

Представление данных (N mod 4) + 1 = 3, 2 вариант.
3 представление данных удобна для поиска учеников по группам и простого доступа ко всем данным об ученике. Однако данные об ученике сложно обработать, так как это в неком смысле таблица (матрица).

Примеры использования:
```prolog
7 ?- write_all_subject.
Логическое программирование: 4
Математический анализ: 3.892857142857143
Функциональное программирование: 3.857142857142857
Информатика: 3.75
Английский язык: 3.857142857142857
Психология: 3.75
true.

8 ?- write_all_subject_ba.
101: 3
102: 3
103: 2
104: 3
true.

9 ?- write_all_subject_bad.
ERROR: Unknown procedure: ite_all_subject_badwrite_all_subject_bad/0 (DWIM could not correct goal)
10 ?- write_all_subject_bad.
Логическое программирование: 0
Математический анализ: 3
Функциональное программирование: 3
Информатика: 3
Английский язык: 2
Психология: 3
true ;
false.
```

Реализация:
```prolog
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
```

## Выводы

В лабораторной работе были реалезованы стандартные предикаты обработки списков, однако swipl не позволяет использовать собственно написаный предикат lenght, так как он уже встроен в систему. (возможно его можно переопределить или вовсе удалить) Также были реализованы более сложные предикаты, предикаты обработки списков и даже небольшой базы данных. Пролог вполне справляется с обработкой строго иерархических структур, будь то список или база данных, но разбиение всех "задач" на простые, однопонятные предикаты усложняет понимание и читаемость кода. Такое дробление задач дает возможность быстрой обработки ошибок, что ускоряется еще и оболочкой swipl, которая работает, как своя командная строка программы пользователя. 
