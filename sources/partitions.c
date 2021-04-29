//https://firststeps.ru/linux/r.php?20
#include <dirent.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

char *const SYS_BLOCK_PATH = "/sys/block/"; //подкаталоги всех блочных устройств


int partitionsMode()
{

    //открываем дирикторию для чтения /sys/block/
    DIR *sys_block_dir = opendir(SYS_BLOCK_PATH);
    struct dirent *disk;

    if (sys_block_dir)
    {
        // читаем dirent, пока не null
        while ((disk = readdir(sys_block_dir)) != NULL)
        {
            if (strcmp(disk->d_name, ".") && strcmp(disk->d_name, "..") && strchr(disk->d_name, 'l') == NULL)
            {
                printf("/dev/%s\n", disk->d_name);

                char disk_path[256] = {0};
                strcat(strcat(disk_path, SYS_BLOCK_PATH), disk->d_name);
                DIR *sys_block_subdir = opendir(disk_path);
                struct dirent *partition;

                //выводим подкаталоги
                if (sys_block_subdir)
                {
                    while ((partition = readdir(sys_block_subdir)) != NULL)
                    {
                        //http://all-ht.ru/inf/prog/c/func/memcmp.html
                        if (!memcmp(partition->d_name, disk->d_name, strlen(disk->d_name)))
                            printf("- /dev/%s\n", partition->d_name);
                    }
                }

                closedir(sys_block_subdir);
            }
        }

        closedir(sys_block_dir);
    }

    return 0;
}
