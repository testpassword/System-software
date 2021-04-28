#include "main.h"
#include "hfsplus_utils.h"


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>

//argc содержит количество аргументов в командной строке.
//argv является указателем на массив указателей.
int main(int argc, char **argv) {

    if (argc >= 2 && strcmp(argv[1], "help") == 0) {
        printf("Для вывода списка дисков и разделов, подключенных к операционной системе, задайте аргумент: list \n"
               "Для выполненя операций над файловой системой, задайте аргумент: shell и укажите диск");
//        printf("To run program in list mode use 'list' key. To run it in shell mode use 'shell' key and specify drive or deviceDescriptor.");
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "list") == 0) {
        listMode();
        return 0;
    }
    if (argc >= 3 && strcmp(argv[1], "shell") == 0) {
        shellMode(argv[2]);
        return 0;
    }
    printf("Некоректный ввод. Воспользуйтесь аргументом help для просмотра возможности программы");
    return 0;
}

//Вывод списка дисков и разделов подключенных к ОС.
int listMode() {
    DIR *block_devices_dir;
    DIR *partitions_dir;
    struct dirent *block_device_entry;
    struct dirent *partition_entry;
    regex_t matcher_drives;
    regex_t matcher_partitions;


    regcomp(&matcher_drives, DRIVE_PATTERN,
            REG_EXTENDED); //https://www.opennet.ru/cgi-bin/opennet/man.cgi?topic=regcomp&category=3
    block_devices_dir = opendir(SYS_BLOCK_DIR);
    if (!block_devices_dir) {
        return -1;
    }
    while ((block_device_entry = readdir(block_devices_dir)) != NULL) {
        if (!regexec(&matcher_drives, block_device_entry->d_name, 0, NULL, 0)) {
            printf("Drive %s\n", block_device_entry->d_name);

            regcomp(&matcher_partitions, block_device_entry->d_name, 0);
            char drive_dir[strlen(SYS_BLOCK_DIR) + strlen(block_device_entry->d_name) + 1];
            drive_dir[0] = 0;
            strcat(drive_dir, SYS_BLOCK_DIR);
            strcat(drive_dir, block_device_entry->d_name);
            partitions_dir = opendir(drive_dir);
            if (!partitions_dir) {
                return -1;
            }
            while ((partition_entry = readdir(partitions_dir)) != NULL) {
                if (!regexec(&matcher_partitions, partition_entry->d_name, 0, NULL, 0)) {
                    printf("\t- partition %s\n", partition_entry->d_name);
                }
            }
        }
    }
    closedir(block_devices_dir);
    return 0;
}

int shellMode(char *filename) {
    FileSystem *fileSystem = openFileSystem(filename);
    if (fileSystem == NULL) {
        printf("Диск не существует или не поддерживает файловую систему HFS+");
        return -1;
    }
    printf("Данный диск имеет файловую систему HFS+\n");
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
            printf("cd [directory] - change working directory\n");
            printf("pwd - print working directory full name\n");
            printf("cp - [directory] [target directory] - copy dir or file from mounted device\n");
            printf("ls - show working directory elements\n");
            printf("exit - terminate program\n");
            printf("help - print help\n");
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
            printf("Wrong command. Enter 'help' to get help.\n");
        }
    }
    closeFileSystem(fileSystem);
    return 0;
}
