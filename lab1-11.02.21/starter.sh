# И СКРИПТ, И ПРОГРАММУ ЗАПУСКАТЬ С ПРАВАМИ СУПЕРПОЛЬЗОВАТЕЛЯ!
# ПУТЬ МОНТИРОВАНИЯ ДОЛЖЕН СОСТОЯТЬ ИЗ ОДНОГО КУСОЧКА (пр. `/mnt` , но не `/mnt/something`)
# ФАЙЛЫ И ДИРЕКТОРИИ ДОЛЖНЫ СОДЕРЖАТЬ ТОЛЬКО АНГЛИЙСКИЙ БУКВЫ И ЦИФРЫ
# В КАЧЕСТВЕ АРГУМЕНТА РЕЖИМУ `--explorer` ПЕРЕДАВАТЬ НЕ ПУТЬ К ТОЧКЕ МОНТИРОВАНИЯ, А К РАЗДЕЛУ НА ДИСКЕ ИЛИ ФАЙЛУ (пр. `./app -E /dev/sdb2` , а не `./app -E /mnt`)

MOUNT_POINT = "/mnt/pocket"

function execute_info { ./build/app -I }

function execute_explorer { ./build/app -E build/pocket.fs }

function build {
    app_exe = "build/app"
    rm $app_exe
    gcc -o $app_exe src/shell.c src/fs_utils.c src/hfsplus_utils.c
    chmod +x $app_exe
}

function create_hfs_to_file {
    apt install hfsprogs
    dd if=/dev/zero of=build/pocket.fs bs=1024 count=1024
    mkfs.hfsplus build/pocket.fs
}

function mount_fs_from_file {
    arg = "build/pocket.fs ${MOUNT_POINT}"
    umount $arg
    mount_fs $arg
}

function mount_fs_from_disk {
    arg = "/dev/sdb2 ${MOUNT_POINT}"
    umount $arg
    mount_fs $arg
}

function mount_fs { mount ${1} /mnt }

modes=("I" "E" "B" "C" "MF" "MD")
case ${1} in
    ${modes[0]}
        execute_info
        ;;
    ${modes[1]}
        execute_explorer
        ;;
    ${modes[2]}
        build
        ;;
    ${modes[3]}
        create_hfs_to_file
        ;;
    ${modes[4]}
        mount_fs_from_file
        ;;
    ${modes[5]}
        mount_fs_from_disk
        ;;
    *)
        echo "Режим не задан"
        echo "Режимы:"
        for m in ${modes[@]}; do
            echo $m
        done
esac