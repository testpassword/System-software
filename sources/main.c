#include "../headers/main.h"
#include "../headers/hfsplus_utils.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//argc содержит количество аргументов в командной строке.
//argv является указателем на массив указателей.
int main(int argc, char *argv[]) {

    if (argc == 2 && strcmp(argv[1], "partitions") == 0) {
        partitionsMode();
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "shell") == 0) {
        shellMode(argv[2]);
        return 0;
    }
    printf("Некоректный ввод. Для запуска прокрамма выберите один из представленных аргументов: \n"
           "1) Для вывода списка дисков и разделов, подключенных к операционной системе, задайте аргумент: partitions \n"
           "2) Для выполненя операций над файловой системой, задайте аргумент: shell и укажите раздел диска");
    return 0;
}


int shellMode(char *filename) {
    FileSystem *fileSystem = openFileSystem(filename);
    if (fileSystem == NULL) {
        printf("Диск не существует или не поддерживает файловую систему HFS+");
        return -1;
    }
    printf("Диск поддерживает HFS+\n");
    int exitFlag = 0;
    char *inputString = malloc(1024);
    while (!exitFlag) {
        printf("> ");
        fgets(inputString, 1024, stdin);
        char *command = strtok(inputString, " \n");
        if (command == NULL) {
            continue;
        }
        char *path = strtok(NULL, " \n");
        char *outPath = strtok(NULL, " \n");
        if (strcmp(command, "exit") == 0) {
            exitFlag = 1;
        } else if (strcmp(command, "help") == 0) {
            printf("cd [directory] - изменение рабочей дириктории\n");
            printf("pwd -  вывод полного пути от корневого каталога к текущему рабочему каталогу\n");
            printf("cp - [directory] [target directory] - копировать файлов или директории\n");
            printf("ls - вывод содержимого каталога");
            printf("exit -  завершение процесса командной оболочки \n");
        } else if (strcmp(command, "ls") == 0) {
            char *output = ls(fileSystem, path);
            printf("%s", output);
            free(output);
        } else if (strcmp(command, "pwd") == 0) {
            char *output = pwd(fileSystem);
            printf("%s", output);
            free(output);
        } else if (strcmp(command, "cd") == 0) {
            char *output = cd(fileSystem, path);
            printf("%s", output);
            free(output);
        } else if (strcmp(command, "cp") == 0) {
            char *output = cp(fileSystem, path, outPath);
            printf("%s", output);
            free(output);
        } else {
            printf("Команды не сущесвует. Введите help чтобы просмотреть список допустимыз команд.\n");
        }
    }
    closeFileSystem(fileSystem);
    return 0;
}
