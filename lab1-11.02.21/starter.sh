function execute_info { ./build/app -I }

function execute_explorer { ./build/app -E build/pocket.fs }

function build {
    rm build/app
    gcc -o build/app src/shell.c src/fs_utils.c src/hfsplus_utils.c
}

function create_hfs_to_file {
    sudo apt install hfsprogs
    dd if=/dev/zero of=build/pocket.fs bs=1024 count=1024
    mkfs.hfsplus build/pocket.fs
    mkdir build/pocket.fs /mnt/pocket/
}

modes=("EXEC-I" "EXEC-E" "BUILD" "CREATE_HFS")
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
    *)
        echo "Режим не задан"
        echo "Режимы:"
        for m in ${modes[@]}; do
            echo $m
        done
esac