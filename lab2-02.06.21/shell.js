const native = require('bindings')('spo_wrapper')
const reader = require("readline-sync")
const args = process.argv
if (args.length == 3 && ['--help', '-H'].includes(args[2])) console.log(`
    --info : for information about disks and partitions
    --explorer <partitions_path> : for editing
    `)
else if (args.length == 3 && ['--info', '-I'].includes(args[2])) {
    native.init()
    console.log(native.partitions())
}
else if (args.length == 4 && ['--explorer', '-E'].includes(args[2])) {
    const fsPath = args[3]
    if (native.open(fsPath)) console.log(`unsupported filesystem in: ${fsPath}. Supported fs is HFS+ only`)
    else {
        native.loadFS()
        let isWorking = true
        while (isWorking) {
            const parse_cmd = (it) => { return { action: it.substring(0, 2), arg: it.substring(3) } }
            const cmd = reader.question("_> ")
            if (cmd === "pwd") console.log(native.pwd())
            else if (cmd == "exit") isWorking = false
            else {
                const { action, arg } = parse_cmd(cmd)
                console.log(arg)
                if (action === "ls") console.log(native.ls((arg === "") ? "/" : arg))
                else if (action === "cp") console.log((arg.split(" ").length !== 2) ? "you should set 2 arguments for `cp` command" : native.cp(arg))
                else if (action === "cd") console.log(native.cd(arg))
                else console.log(`supported commands:
                    pwd : show path inside mounted fs
                    ls <path> : show items in path
                    cd <path> : moves into mounted fs
                    cp <src> <dest> : copy file or folder inside mounted os
                    exit : terminate process
                `)
            }
        }
    }
} else {
    console.log("error, mode don't specified, start with -H")
    process.exit(-1)
}