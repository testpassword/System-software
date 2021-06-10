#include <Commander.h>
#include <HFSPlus.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <HFSPlusBTree.h>
#include <IOFunc.h>
#include <List.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils/Endians.h"
#include "copy/Copy.h"

PathListNode *SplitPathWithDelimeter(char *path, const char* delimeter) {
    PathListNode *listHead = NULL;
    char *pathToken;
    while ((pathToken = strsep(&path, delimeter))) {
        if (strcmp(pathToken, "") == 0) continue;
        PathListNode newNode;
        memset(&newNode, 0, sizeof(PathListNode));
        newNode.token = calloc(sizeof(char), strlen(pathToken) + 1);
        newNode.token = strcpy(newNode.token, pathToken);
        PathListAdd(&listHead, newNode);
    }
    return listHead;
}


int Verify(FileSystem* fs);

int ReadBtreeHeader(uint64_t pos, FileSystem* fs);

void ExtractCatalogBtreeHeader(uint64_t block, BTHeaderRec *header, FileSystem* fs);

PathListNode *SplitPath(char *path) {
    PathListNode *listHead = NULL;
    char *pathToken;
    PathListNode node;
    node.token = "/";
    PathListAdd(&listHead, node);
    while ((pathToken = strsep(&path, "/"))) {
        if (strcmp(pathToken, "") == 0) continue;
        PathListNode newNode;
        memset(&newNode, 0, sizeof(PathListNode));
        newNode.token = calloc(sizeof(char), strlen(pathToken) + 1);
        newNode.token = strcpy(newNode.token, pathToken);
        PathListAdd(&listHead, newNode);
    }
    return listHead;
}

int openByPath(const char *path, FileSystem* fs) {
    FILE *dev = fopen(path, "rb");
    if (!dev) {
        perror("unexpected error, can't open\n");
        return -1;
    }
    fs->file = dev;
    if (Verify(fs)) return -1;
    return 0;
}

int Verify(FileSystem* fs) {
    HFSPlusVolumeHeader header;
    if (fseek(fs->file, HFS_START_OFFSET, SEEK_SET)) {
        fprintf(stderr, "Can't set 1024 bytes offset!\n");
        return -1;
    }
    if (fread(&header, sizeof(HFSPlusVolumeHeader), 1, fs->file) != 1) {
        fprintf(stderr, (feof(fs->file)) ? "Unexpected EOF!\n" : "Can't read HFS volume header!\n");
        return -1;
    }
    if (header.signature == HFS_SIGNATURE) {
        fs->blockSize = htonl(header.blockSize);
        fs->catalogFileBlock = bswap_32(header.catalogFile.extents[0].startBlock);
        int extents = 0;
        int totalBlocks = 0;
        for (int i = 0; i < 8; i++) {
            header.catalogFile.extents[i].startBlock = bswap_32(header.catalogFile.extents[i].startBlock);
            header.catalogFile.extents[i].blockCount = bswap_32(header.catalogFile.extents[i].blockCount);
            totalBlocks += header.catalogFile.extents[i].blockCount;
            if (header.catalogFile.extents[i].startBlock != 0 && header.catalogFile.extents[i].blockCount != 0) extents += 1;
        }
        fs->volumeHeader = header;
        return 0;
    } else {
        fprintf(stderr, "path doesn't contain HFS+ fs\n");
        return -1;
    }
}

int dirContent(const char *path, FileSystem* fs) {
    if (strcmp(path, "") == 0 | strcmp(path, "\n") == 0) {
        printf("required path doesn't exist\n");
        return 0;
    }
    size_t strLength = strlen(path) + 1;
    char *pathCopy = malloc(strLength);
    memcpy(pathCopy, path, strLength);
    size_t pathCopyLength = strlen(pathCopy);
    if (pathCopy[pathCopyLength - 1] == '\n') pathCopy[strlen(pathCopy) - 1] = 0;
    PathListNode *list = SplitPath(pathCopy);
    PathListNode *listStart = list;
    free(pathCopy);
    BTHeaderRec catalogFileHeader;
    ExtractCatalogBtreeHeader(fs->catalogFileBlock, &catalogFileHeader, fs);
    fs->output = NULL;
    uint32_t parentID = 2;
    while (list) {
        if (list->next == NULL) {
            if (parentID == 0) printf("required path doesn't exist\n");
            ListDirectoryContent(parentID, catalogFileHeader, fs);
            break;
        }
        else parentID = FindIdOfFolder(list->next->token, parentID, catalogFileHeader, *fs);
        list = list->next;
    }
    PathListClear(listStart);
    return 0;
}

int setCurDir(const char* path, FileSystem* fs) {
    if (strcmp(path, "") == 0 | strcmp(path, "\n") == 0) return -1;
    int result = -1;
    char *pathCopy = malloc(strlen(path) + 1);
    strcpy(pathCopy, path);
    PathListNode *list = SplitPath(pathCopy);
    PathListNode *listHead = list;
    free(pathCopy);
    BTHeaderRec catalogFileHeader;
    ExtractCatalogBtreeHeader(fs->catalogFileBlock, &catalogFileHeader, fs);
    uint32_t parentID = 2;
    while (list) {
        if (list->next == NULL) {
            if (parentID == 0) {
                result = -1;
                break;
            }
            result = 0;
            break;
        }
        else parentID = FindIdOfFolder(list->next->token, parentID, catalogFileHeader, *fs);
        list = list->next;
    }
    PathListClear(listHead);
    return result;
}

void ExtractCatalogBtreeHeader(uint64_t block, BTHeaderRec *header, FileSystem* fs) {
    BTNodeDescriptor btreeHeaderDescr;
    seekItem(fs->file, block * fs->blockSize, SEEK_SET);
    readItem(&btreeHeaderDescr, sizeof(BTNodeDescriptor), 1, fs->file);
    if (btreeHeaderDescr.kind != HeaderNode) {
        fputs("Unexpected node type. Expected header type!\n", stderr);
        return;
    }
    readItem(header, sizeof(BTHeaderRec), 1, fs->file);
    ConvertBTreeHeader(header);
    ConvertBTreeNodeDescriptor(&btreeHeaderDescr);
}

int copyItems(const char* path, const char* currentDir, FileSystem* fs) {
    PathListNode *list = SplitPathWithDelimeter(path, " ");
    PathListNode *listHead = list;
    char *src;
    char *dest;
    uint32_t i = 0;
    while (list) {
        if (i == 0) src = list->token;
        else {
            dest = list->token;
            if (dest[strlen(dest) - 1] == '\n') dest[strlen(dest) - 1] = '\0';
        }
        i++;
        list = list->next;
    }
    PathListClear(listHead);
    char srcPath[1024];
    if (src[0] == '.')
        (strlen(src) > 1) ? snprintf(srcPath, sizeof(srcPath), "%s%s", currentDir, src + 1) : snprintf(srcPath, sizeof(srcPath), "%s", currentDir);
    else snprintf(srcPath, sizeof(srcPath), "%s", src);
    BTHeaderRec header;
    ExtractCatalogBtreeHeader(fs->catalogFileBlock, &header, fs);
    char* srcPathCopy = calloc(strlen(srcPath) + 1, 1);
    strcpy(srcPathCopy, srcPath);
    PathListNode *splitedSrcPathList = SplitPath(srcPathCopy);
    PathListNode *splitedSrcPathListStart = splitedSrcPathList;
    free(srcPathCopy);
    uint32_t parentID = 2;
    while (splitedSrcPathList) {
        if (splitedSrcPathList->next == NULL) {
            if (parentID == 0) {
                parentID = 0;
                break;
            }
            break;
        }
        else parentID = FindIdOfFolder(splitedSrcPathList->next->token, parentID, header, *fs);
        splitedSrcPathList  = splitedSrcPathList->next;
    }
    if (parentID != 0) CopyDirectory(srcPath, dest, parentID, header, *fs);
    else {
        splitedSrcPathList = splitedSrcPathListStart;
        parentID = 2;
        HFSPlusCatalogFile *file = NULL;
        while (splitedSrcPathList) {
            if (splitedSrcPathList->next == NULL) {
                if (parentID == 0) {
                    parentID = 0;
                    break;
                }
                file = GetFileRecord(parentID, header, *fs);
                break;
            } else parentID = FindIdOfFile(splitedSrcPathList->next->token, parentID, header, *fs);
            splitedSrcPathList = splitedSrcPathList->next;
        }
        if (parentID == 0) printf("unexpected error, reboot your linux please\n");
        else {
            CopyFile(dest, splitedSrcPathList->token, *file, fs);
            printf("copied\n");
        }
        free(file);
    }
    PathListClear(splitedSrcPathListStart);
    return 0;
}
