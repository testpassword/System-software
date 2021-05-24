#include "./headers/fs_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./str_extensions/println.h"
#include "./str_extensions/switch.h"

int main(int argc, char *argv[]) {
    if ((argc == 2 && strcmp(argv[1], "--help") == 0) || (argc == 2 && strcmp(argv[1], "-H") == 0)) 
        println("--info : for information about disks and partitions\n--explorer <partition_path> : for editing");
    else if ((argc == 2 && strcmp(argv[1], "--info") == 0) || (argc == 2 && strcmp(argv[1], "-I") == 0))
        println(get_partitions_str());
    else if ((argc == 3 && strcmp(argv[1], "--explorer") == 0) || (argc == 3 && strcmp(argv[1], "-E") == 0)) {
        FileSystem* fs = open_fs(argv[2]);
        if (fs == NULL) println("unsupported filesystem in: %s. Supported fs is HFS+ only");
        else {
            char* input_str = malloc(FILENAME_MAX * 4 + 2); // команда + 2 аргумента + запас на пробелы
            for(;;) {
                printf("> ");
                fgets(input_str, 1024, stdin);
                char* sep = " \n";
                char* cmd = strtok(input_str, sep);
                if (cmd == NULL) continue;
                char* path = strtok(NULL, sep);
                char* cmd_arg = strtok(NULL, sep);
                switchs(cmd) {
                    cases("exit")
                        close_fs(fs);
                        return 0;
                    cases("ls")
                        println(ls(fs, path));
                        break;
                    cases("pwd")
                        println(pwd(fs));
                        break;
                    cases("cd")
                        println(cd(fs, path));
                        break;
                    cases("cp")
                        println(cp(fs, path, cmd_arg));
                        break;
                    defaults
                        println("supported commands:\npwd : show path in mounted fs;\nls <path> : show items in path\ncd <path> : moves into mounted fs\ncp <src> <dest> : copy file or folder inside mounted fs or from mounted os to external (only absolute path) supports\nexit : terminate session");
                        break;
                } switchs_end;
            }
        }
    } else println("error, mode don't specified, start with -H");
    return 0;
}