#№ Отчет по лабораторной работе №4
## по курсу "Логическое программирование"

## Обработка естественного языка

### студент: Иванопуло А.Б.

## Результат проверки

| Преподаватель     | Дата         |  Оценка       |
|-------------------|--------------|---------------|
| Сошников Д.В. |              |               |
| Левинская М.А.|              |               |

> *Комментарии проверяющих (обратите внимание, что более подробные комментарии возможны непосредственно в репозитории по тексту программы)*


## Введение

Для обработки и анализа естественных и искусственных языков обычно применяются следующие подходы:

1. Статистический подход: Этот подход основан на использовании статистических моделей и машинного обучения для извлечения знаний и шаблонов из больших объемов текстовых данных. Он позволяет автоматически обрабатывать и анализировать тексты, определять свойства языка и решать задачи, такие как распознавание речи, машинный перевод, классификация текстов и другие.
2. Грамматический подход: В грамматическом подходе используются формальные грамматики и правила для определения синтаксической структуры языка. Этот подход включает в себя создание контекстно-свободных грамматик, деревьев вывода и семантических моделей, которые позволяют анализировать и интерпретировать тексты.
3. Логический подход: Логический подход, включающий в себя логическое программирование, основан на применении логических формализмов и правил вывода для моделирования и решения задач обработки языка. Этот подход позволяет формулировать грамматические правила, описывать семантику языка и выполнять логический вывод для решения задач, таких как обработка естественного языка и автоматическое доказательство теорем.

Prolog является удобным языком для решения задач обработки и анализа естественных и искусственных языков по нескольким причинам:

1. Логическое программирование: Пролог основан на логическом программировании, что делает его естественным выбором для моделирования и решения задач обработки языка с использованием логических формализмов и правил вывода.
2. Поддержка шаблонного сопоставления: Пролог позволяет использовать мощный механизм шаблонного сопоставления, который позволяет выражать сложные образцы и шаблоны для обработки и анализа языка. Это полезно для поиска и извлечения информации из текстов и выполнения логического вывода.
3. Работа с грамматиками: Пролог предоставляет поддержку для описания грамматик в форме дифференциальной нотации с использованием оператора -->. Это позволяет лаконично определять правила грамматики и осуществлять разбор текстовых данных.
4. Возможности интерпретации и доказательства: Пролог позволяет интерпретировать и анализировать тексты с помощью логического вывода и автоматического доказательства теорем. Это особенно полезно при решении задач, связанных с семантикой и интерпретацией языка.

Совокупность этих особенностей делает Prolog удобным и мощным языком для решения задач обработки и анализа языка, позволяя моделировать грамматики, выполнять шаблонное сопоставление и использовать механизмы логического вывода.

## Задание
Реализовать разбор фраз языка, представляющих собой положительные и отрицательные высказывания. В результате предикат должен создать все атомарные глубинные структуры.

## Принцип решения

Предложение, как и в жизни, состоит из слов. В какждом предложении должны быть подлежащее и сказуемое (для упрощения). Будем делить предложения на слова пока не доберется до терминальных символов. 

```prolog
% Предложение состоит из подлежащего и подпредложения (группы сказуемых)
sentence(X) --> obj(Y), sub(Y, X).

% Подпредложение состоит из группы глагола
sub(Y, X) --> verb_g(Y, X).
% либо из группы глагола, информация из которой нас интересует, и еще одного подпредложения
sub(Y, X) --> verb_g(Y, X), sub(_, _).
% либо из группы глагола, и еще одного подпредложения, информация из которого нас интересует
sub(Y, X) --> verb_g(_, _), sub(Y, X).

% группа глагола состоит из союза(опционально) и словосочетания с каким-то субъектом и сказуемого "любит"
verb_g(X, likes(X, Y)) --> (pr_g; []), likes(Y).
% либо из союза и сказуемого "не любит"
verb_g(X, not_likes(X, Y)) --> (pr_g; []), not_likes(Y).

% группа субьектов состоит либо из субьекта, который нас интересует и субьекта, который не интересует
subj_g(X) --> subj(X), ["и"], subj(_).
% либо из субьекта, который нас не интересует и субьекта, который интересует
subj_g(X) --> subj(_), ["и"], subj(X).
% либо из одиночного субьекта
subj_g(X) --> subj(X).
```
Если в предложении есть частица "не", то возвращающий терм означает отрицание. Иначе говорится, что подлежщее любит что-то. Это что-то мы получаем при помощи правила likes или not_likes.
```prolog
% словосочетания с каким-то субъектом сказуемым "любит"
likes(X) --> ["любит"], subj_g(X).
% словосочетания с каким-то субъектом сказуемым "не любит"
not_likes(X) --> not, ["любит"], subj_g(X).

% частица не
not --> ["не"].
```
Остается разобрать предложение и перебрать неповторяющиеся решения.
```prolog
% Предикат для вычленения всех атомарных глубинных структур вида likes(X, Y) и not_likes(X, Y)
decompose(X, Y):-
    setof(Z, phrase(sentence(Z), X), Y).
```

## Результаты

```prolog
?- decompose(["Саша", "любит", "кубики", "и", "шарики"], X).
X = [likes("Саша", "кубики"), likes("Саша", "шарики")].

?- decompose(["Саша", "любит", "кубики", ",", "но", "не", "любит", "ролики"], X).
X = [likes("Саша", "кубики"), not_likes("Саша", "ролики")].

?- decompose(["Саша", "любит", "кубики", "и", "шарики", ",", "но", "не", "любит", "ролики", "и",
"квадратики",",", "а", "любит", "штуковины"], X).
X = [likes("Саша", "кубики"), likes("Саша", "шарики"), likes("Саша", "штуковины"), not_likes
("Саша", "квадратики"), not_likes("Саша", "ролики")].
```

## Выводы

В ходе выполнения данной лабораторной работы, я получил опыт в использовании пролога для решения задачи грамматического разбора. Она позволила мне разобраться в основах построения грамматических правил.
Я осознал, что использование пролога для решения задач грамматического разбора является удобным и эффективным подходом. Пролог предоставляет возможность использовать сопоставление с образцом и рекурсию для описания грамматических правил и создания грамматического разбора. Такое декларативное описание правил в прологе позволяет четко выразить структуру языка и упростить процесс разбора текста.
