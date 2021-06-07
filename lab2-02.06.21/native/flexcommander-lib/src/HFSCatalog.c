#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include "HFSCatalog.h"
#include <Flexcommander.h>
#include <List.h>

void PrintCatalogKey(HFSPlusCatalogKey key) {
    setlocale(LC_CTYPE, "");
    printf("Key info:\n");
    printf("Key length: %d\n", key.keyLength);
    printf("Parent ID: %d\n", key.parentID);
    printf("Node name length: %d\n", key.nodeName.length);
    printf("Node name: ");
    for (int i = 0; i < key.nodeName.length; i++) printf("%lc", key.nodeName.unicode[i]);
    printf("\n");
}

void PrintCatalogFolder(HFSPlusCatalogFolder folder) {
    printf("Folder info:\n");
    printf("Valence: %d\n", folder.valence);
    printf("Folder ID: %d\n", folder.folderID);
}

void PrintCatalogFile(HFSPlusCatalogFile file) {
    printf("File info:\n");
    printf("File ID: %d\n", file.fileID);
}

void PrintCatalogThread(HFSPlusCatalogThread thread) {
    printf("Catalog thread info:\n");
    printf("Thread type: ");
    switch (thread.recordType) {
        case FolderThreadRecord:
            printf("folder\n");
            break;
        case FileThreadRecord:
            printf("file\n");
            break;
        default:
            printf("unknown\n");
            break;
    }
    printf("Parent ID: %d\n", thread.parentID);
    printf("Thread name length: %d\n", thread.nodeName.length);
    printf("Node name: ");
    for (int i = 0; i < thread.nodeName.length; i++) printf("%lc", thread.nodeName.unicode[i]);
    printf("\n");
}

void PrintCatalogIndexNode(BTCatalogIndexNode node) {
    PrintCatalogKey(node.key);
    printf("Index: %d\n", node.nextNode);
}

void PrintPermissions(HFSPlusBSDInfo permissions, HFSDataRecordType recordType, FlexCommanderFS* fs) {
    const uint32_t ownerMask = 0000700;
    const uint32_t groupMask = 0000070;
    const uint32_t otherMask = 0000007;
    const uint32_t ownerRights = permissions.fileMode & ownerMask;
    const uint32_t groupRights = permissions.fileMode & groupMask;
    const uint32_t otherRights = permissions.fileMode & otherMask;
    PathListNode pathListNode;
    switch (recordType) {
        case FolderRecord:
            pathListNode.token = "d";
//            printf("d");
            break;
        default:
            pathListNode.token = "-";
//            printf("-");
            break;
    }

    PathListAdd(&fs->output, pathListNode);

    switch (ownerRights) {
        case 0:
            pathListNode.token = "---";
//            printf("---");
            break;
        case S_IXUSR:
            pathListNode.token = "--x";
//            printf("--x");
            break;
        case S_IWUSR:
            pathListNode.token = "-w-";
//            printf("-w-");
            break;
        case S_IXUSR + S_IWUSR:
            pathListNode.token = "-wx";
//            printf("-wx");
            break;
        case S_IRUSR:
            pathListNode.token = "r--";
//            printf("r--");
            break;
        case S_IRUSR + S_IXUSR:
            pathListNode.token = "r-x";
//            printf("r-x");
            break;
        case S_IRUSR + S_IWUSR:
            pathListNode.token = "rw-";
//            printf("rw-");
            break;
        case S_IRWXU:
            pathListNode.token = "rwx";
//            printf("rwx");
            break;
        default:
            printf("???");
            break;
    }

    PathListAdd(&fs->output, pathListNode);
}