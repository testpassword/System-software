# Лабораторная №1 - файловые системы
**Вариант - HFS PLus**
***Forked from https://github.com/progML/Spo_1***

Реализовать программу, которая может использоваться в двух режимах. Режимы задаются в виде аргументов командной строки и позволяют:
1. Выводить список дисков и разделов, подключенных к операционной системе.
2. Выполнять операции надфайловой системой, представленной назаданномдиске,разделе или в файле.

Запущенная во втором режиме программа должна выполнять следующие действия:
1. Проверять, поддерживается ли файловая система на заданном разделе или диске. 
2. В случае, если файловая система поддерживается, программа переходит в диалоговый режим, ожидая ввода команд от пользователя. Команды задают операции над файловой системой:
    - вывод списка имен и атрибутов элементов указанной директории;
    - копирование  файлов  или  директорий из исследуемой (заданной по  варианту) файловой системы;
    - отображение названия «текущей» директории и переход в другую директорию.
    
Программа должна состоять из двух модулей. Первый модуль реализует функции для работы с файловой системой, а второй – взаимодействие с пользователем.

# Лабораторная №2
**Вариант - Node.js**
***Forked from https://github.com/rbetik12/flexcommander-js***

Цель – изучение способов организации программных интерфейсов между средами высокого и низкого уровней программной архитектуры.

Построить разделяемую библиотеку (shared library) с функциональностью для работы с файловой системой, реализованную в лабораторной работе No1. На языке высокого уровня реализовать консольное или графическое приложение, функциональность которого аналогична программе из лабораторной работы No1, для операций с файловой системой использовать полученную библиотеку, написанную на Си. При при необходимости реализовать дополнительную библиотеку, обеспечивающую вызов функций из программы на языке высокого уровня.