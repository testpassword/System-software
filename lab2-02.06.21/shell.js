const native = require('bindings')('spo1')
const { pwd, ls, cp, cd } = native
const reader = require("readline-sync")
const args = process.argv
if (args.length == 3 && ['--help', '-H'].includes(args[2])) console.log(`
    --info : for information about disks and partitions
    --explorer <partitions_path> : for editing
    `)
else if (args.length == 3 && ['--info', '-I'].includes(args[2])) {
    native.init()
    native.probeDevs()
    console.log(native.partitions())
}
else if (args.length == 4 && ['--explorer', '-E'].includes(args[2])) {
    const fsPath = args[3]
    if (native.open(fsPath)) console.log(`unsupported filesystem in: ${fsPath}. Supported fs is HFS+ only`)
    else {
        native.loadFS()
        let cmd = ""
        while (cmd !== 'exit') {
            cmd = reader.question("_> ")
            if (cmd === "pwd") console.log(pwd())
            else if (cmd.substring(0, 2) === "ls") console.log(ls((cmd.length === 2) ? "/" : cmd.substring(3)))
            else if (cmd.substring(0, 2) === "cp") console.log(cp(cmd.substring(3)))
            else if (cmd.substring(0, 2) === "cd") console.log(cd(cmd.substring(3)))
            else console.log(`supported commands:
                pwd : show path inside mounted fs
                ls <path> : show items in path
                cd <path> : moves into mounted fs
                cp <src> <dest> : copy file or folder inside mounted os
                exit : terminate process
            `)
        }
    }
} else {
    console.log("error, mode don't specified, start with -H")
    process.exit(-1)
}