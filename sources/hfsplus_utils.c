#include "../headers/hfsplus_utils.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define HEADER_OFFSET 1024

FileSystem *openFileSystem(char *name) {
    HFSPlusVolumeHeader *header = malloc(sizeof(struct HFSPlusVolumeHeader));
    int fd = open(name, O_RDONLY, 00666);
    pread(fd, header, sizeof(struct HFSPlusVolumeHeader), HEADER_OFFSET);
    reverseHFSPlusVolumeHeader(header);
    if (header->signature == HFS_PLUS_SIGNATURE && header->version == HFS_PLUS_VERSION) {
        FileSystem *fileSystem = malloc(sizeof(FileSystem));
        fileSystem->volumeHeader = header;
        fileSystem->deviceDescriptor = fd;
        fileSystem->catalog = openBTree(fileSystem, typeCatalog);
        fileSystem->pwd = kHFSRootFolderID;
        return fileSystem;
    }
    free(header);
    close(fd);
    return NULL;
}

void closeFileSystem(FileSystem *fileSystem) {
    closeBTree(fileSystem->catalog);
    close(fileSystem->deviceDescriptor);
    free(fileSystem->volumeHeader);
    free(fileSystem);
}

File *openFileFromFork(FileSystem *fileSystem, HFSPlusForkData *forkData) {
    File *file = malloc(sizeof(File));
    file->data = malloc(forkData->totalBlocks * fileSystem->volumeHeader->blockSize);
    file->size = forkData->logicalSize;
    int read_blocks = 0;
    int ext_counter = 0;
    int block_size = fileSystem->volumeHeader->blockSize;
    while (ext_counter < 8 && read_blocks < forkData->totalBlocks) {
        pread(fileSystem->deviceDescriptor,
              file->data + (read_blocks * block_size),
              forkData->extents[ext_counter].blockCount * block_size,
              forkData->extents[ext_counter].startBlock * block_size);
        read_blocks += forkData->extents[ext_counter].blockCount;
        ext_counter++;
    }
    return file;
}

void closeFile(File *file) {
    free(file->data);
    free(file);
}

BTree *openBTree(FileSystem *fileSystem, enum BTreeType type) {
    BTree *catalog = malloc(sizeof(BTree));
    if (type == typeAllocation) {
        catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->allocationFile);
    } else if (type == typeExtent) {
        catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->extentsFile);
    } else if (type == typeCatalog) {
        catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->catalogFile);
    } else if (type == typeStartup) {
        catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->startupFile);
    } else {
        catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->attributesFile);
    }
    catalog->file = openFileFromFork(fileSystem, &fileSystem->volumeHeader->catalogFile);
    catalog->header = catalog->file->data + sizeof(BTNodeDescriptor);
    reverseBTHeaderRec(catalog->header);
    for (int catalog_num = 0; catalog_num < catalog->header->totalNodes; catalog_num++) {

    }
    return catalog;
}

void closeBTree(BTree *catalog) {
    closeFile(catalog->file);
    free(catalog);
}

Node *openNode(int node_num, BTree *tree) {
    if (node_num >= tree->header->totalNodes) {
        return NULL;
    }
    Node *node = malloc(sizeof(Node));
    node->descriptor = malloc(tree->header->nodeSize);
    memcpy(node->descriptor, tree->file->data + tree->header->nodeSize * node_num, tree->header->nodeSize);
    reverseBTNodeDescriptor(node->descriptor);
    node->record_offsets =
            ((UInt16 *) ((void *) node->descriptor + tree->header->nodeSize)) - node->descriptor->numRecords;
    for (int i = 0; i < node->descriptor->numRecords; i++) {
        node->record_offsets[i] = bswap_16(node->record_offsets[i]);
    }
    return node;
}

void closeNode(Node *node) {
    free(node->descriptor);
    free(node);
}

Record *openRecord(int record_num, Node *node) {
    if (record_num >= node->descriptor->numRecords) {
        return NULL;
    }
    Record *record = malloc(sizeof(Record));
    int offset = node->record_offsets[node->descriptor->numRecords - 1 - record_num];
    record->size = node->record_offsets[node->descriptor->numRecords - 2 - record_num] - offset;
    record->data = malloc(record->size);
    void *data = ((void *) node->descriptor) + offset;
    memcpy(record->data, data, record->size);
    record->key_length = *((UInt16 *) record->data);
    record->key_length = bswap_16(record->key_length);
    record->data_offset = record->key_length + sizeof(UInt16);
    return record;
}

void closeRecord(Record *record) {
    free(record->data);
    free(record);
}

void convertNameToString(char *output, UInt16 *rawName, int nameLength) {
    strcpy(output, "");
    for (int i = 0; i < nameLength; i++) {
        UInt16 key_character = bswap_16(rawName[i]);
        if (key_character < 128) {
            char c = key_character;
            strncat(output, &c, 1);
        } else if (key_character < 3778) {
            int c1 = (key_character >> 6) + 0xC0;
            int c2 = (key_character & 0x3F) + 0x80;
            strncat(output, (const char *) &c1, 1);
            strncat(output, (const char *) &c2, 1);
        }
    }
}

void catalogIteration(FileSystem *fileSystem, UInt32 nodeNumber, IterationData *data, void *output,
                      void (*callback)(IterationData *input, void *output)) {
    Node *node = openNode(nodeNumber, fileSystem->catalog);
    UInt32 nodeChild = node->descriptor->fLink;
    data->breakFlag = 0;
    for (int i = 0; i < node->descriptor->numRecords; i++) {
        Record *record = openRecord(i, node);
        data->record = record;
        data->parentID = bswap_32(*(UInt32 *) (record->data + sizeof(UInt16)));
        data->type = bswap_16(*(SInt16 *) (record->data + sizeof(UInt16) + record->key_length));
        strcpy(data->nodeName, "");
        UInt16 *key = record->data + sizeof(UInt16) + sizeof(UInt32);
        UInt16 nameLength = bswap_16(key[0]);
        convertNameToString(data->nodeName, key + 1, nameLength);

        callback(data, output);

        closeRecord(record);
        if (data->breakFlag) {
            return;
        }
    }
    closeNode(node);
    if (nodeChild > 0) {
        catalogIteration(fileSystem, nodeChild, data, output, callback);
    }
}

void nameByIdCallback(IterationData *input, void *output) {
    if (input->type == kHFSPlusFolderRecord) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        if (folder->folderID == input->targetID) {
            sprintf(output, "%s", input->nodeName);
            input->breakFlag = 1;
        }
    } else if (input->type == kHFSPlusFileRecord) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        if (file->fileID == input->targetID) {
            sprintf(output, "%s", input->nodeName);
            input->breakFlag = 1;
        }
    }
}

void parentByIdCallback(IterationData *input, void *output) {
    NodeInfo *nodeInfo = (NodeInfo *) output;
    if (input->type == kHFSPlusFolderRecord) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        if (folder->folderID == input->targetID) {
            nodeInfo->id = input->parentID;
            nodeInfo->type = folder->recordType;
            input->breakFlag = 1;
        }
    }
}

void idByNameCallback(IterationData *input, void *output) {
    NodeInfo *nodeInfo = (NodeInfo *) output;
    if (input->type == kHFSPlusFolderRecord &&
        input->parentID == input->targetID &&
        strcmp(input->targetName, input->nodeName) == 0) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        nodeInfo->id = folder->folderID;
        nodeInfo->type = folder->recordType;
        strcpy(nodeInfo->nodeName, input->nodeName);
        input->breakFlag = 1;
    } else if (input->type == kHFSPlusFileRecord &&
               input->parentID == input->targetID &&
               strcmp(input->targetName, input->nodeName) == 0) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        nodeInfo->id = file->fileID;
        nodeInfo->type = file->recordType;
        strcpy(nodeInfo->nodeName, input->nodeName);
        nodeInfo->data = file->dataFork;
        input->breakFlag = 1;
    }
}

void lsCallback(IterationData *input, void *outputPointer) {
    NodeInfoArray *array = (NodeInfoArray *) outputPointer;
    if (input->type == kHFSPlusFolderRecord && input->parentID == input->targetID) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        array->data = realloc(array->data, (array->size + 1) * sizeof(NodeInfo));
        array->data[array->size].id = folder->folderID;
        strcpy(array->data[array->size].nodeName, input->nodeName);
        array->data[array->size].type = folder->recordType;
        array->size++;
    } else if (input->type == kHFSPlusFileRecord && input->parentID == input->targetID) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        array->data = realloc(array->data, (array->size + 1) * sizeof(NodeInfo));
        array->data[array->size].id = file->fileID;
        strcpy(array->data[array->size].nodeName, input->nodeName);
        array->data[array->size].type = file->recordType;
        array->data[array->size].data = file->dataFork;
        array->size++;
    }
}

NodeInfo *findFileByName(FileSystem *fileSystem, HFSCatalogNodeID parentID, char *path) {
    NodeInfo *nodeInfo = malloc(sizeof(NodeInfo));
    nodeInfo->id = 0;
    IterationData *input = malloc(sizeof(IterationData));
    input->targetID = parentID;
    strcpy(input->targetName, path);
    if (strcmp(path, ".") == 0) {
        char name[256];
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, name, &nameByIdCallback);
        strcpy(nodeInfo->nodeName, name);
        nodeInfo->type = kHFSPlusFolderRecord;
        nodeInfo->id = parentID;
    } else if (strcmp(path, "..") == 0) {
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, nodeInfo, &parentByIdCallback);
        input->targetID = nodeInfo->id;
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, nodeInfo->nodeName,
                         &nameByIdCallback);
    } else {
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, nodeInfo, &idByNameCallback);
    }
    free(input);
    return nodeInfo;
}

NodeInfo *findFileByPath(FileSystem *fileSystem, HFSCatalogNodeID parentID, char *path) {
    char *str = malloc(strlen(path) + 1);
    char *strPointer = str;
    NodeInfo *result = NULL;
    strcpy(str, path);
    if (str[0] == '/') {
        strsep(&str, "/");
        if (strlen(str) == 0) {
            result = malloc(sizeof(NodeInfo));
            result->id = kHFSRootFolderID;
            result->type = kHFSPlusFolderRecord;
            IterationData *input = malloc(sizeof(IterationData));
            input->targetID = kHFSRootFolderID;
            catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, result->nodeName,
                             &nameByIdCallback);
            free(input);
        } else {
            result = findFileByPath(fileSystem, kHFSRootFolderID, str);
        }
    } else {
        char *current_dir = strsep(&str, "/");
        NodeInfo *currentNode = findFileByName(fileSystem, parentID, current_dir);
        if (str == NULL || strlen(str) == 0) {
            result = currentNode;
        } else {
            result = findFileByPath(fileSystem, currentNode->id, str);
            free(currentNode);
        }
    }
    free(strPointer);
    return result;
}

int copy(FileSystem *fileSystem, NodeInfo *info, char *path) {
    char *nodePath = malloc(strlen(path) + strlen(info->nodeName) + 2);
    strcpy(nodePath, path);
    strcat(nodePath, "/");
    strcat(nodePath, info->nodeName);
    if (info->type == kHFSPlusFileRecord) {
        int fd = open(nodePath, O_CREAT | O_WRONLY | O_TRUNC, 00666);
        if (fd == -1) {
            free(nodePath);
            return -1;
        }
        File *file = openFileFromFork(fileSystem, &info->data);
        pwrite(fd, file->data, file->size, 0);
        closeFile(file);
        close(fd);
    } else if (info->type == kHFSPlusFolderRecord) {
        if (mkdir(nodePath, 00777) != 0) {
            free(nodePath);
            return -1;
        }
        NodeInfoArray nodeInfoArray;
        nodeInfoArray.size = 0;
        nodeInfoArray.data = malloc(sizeof(NodeInfo));
        IterationData *input = malloc(sizeof(IterationData));
        input->targetID = info->id;
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, &nodeInfoArray, &lsCallback);

        for (int i = 0; i < nodeInfoArray.size; i++) {
            if (copy(fileSystem, nodeInfoArray.data + i, nodePath) != 0) {
                free(input);
                free(nodeInfoArray.data);
                free(nodePath);
                return -1;
            }
        }

        free(input);
        free(nodeInfoArray.data);
    }
    free(nodePath);
    return 0;
}

char *ls(FileSystem *fileSystem, char *path) {
    NodeInfo *info;
    char *output;
    if (path == NULL) {
        info = findFileByName(fileSystem, fileSystem->pwd, ".");
    } else {
        info = findFileByPath(fileSystem, fileSystem->pwd, path);
    }
    if (info->id == 0) {
        char *message = "No such file or directory\n";
        output = malloc(strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
        free(info);
        return output;
    } else if (info->type != kHFSPlusFolderRecord) {
        char *message = "Not a directory\n";
        output = malloc(strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
        free(info);
        return output;
    }
    NodeInfoArray nodeInfoArray;
    nodeInfoArray.size = 0;
    nodeInfoArray.data = malloc(sizeof(NodeInfo));
    IterationData *input = malloc(sizeof(IterationData));
    input->targetID = info->id;
    catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, &nodeInfoArray, &lsCallback);
    output = malloc(263 * nodeInfoArray.size);
    output[0] = '\0';
    char result[263]; //255 - maximal length of name, 7 - "FOLDER\t", 1 - "\0"
    for (int i = 0; i < nodeInfoArray.size; i++) {
        if (nodeInfoArray.data[i].type == kHFSPlusFolderRecord) {
            sprintf(result, "FOLDER\t%s\n", nodeInfoArray.data[i].nodeName);
            strcat(output, result);
        }
        if (nodeInfoArray.data[i].type == kHFSPlusFileRecord) {
            sprintf(result, "FILE\t%s\n", nodeInfoArray.data[i].nodeName);
            strcat(output, result);
        }
    }
    free(input);
    free(nodeInfoArray.data);
    free(info);
    return output;
}

char *pwd(FileSystem *fileSystem) {
    char *output = malloc(2);
    output[0] = '\n';
    output[1] = '\0';
    char name[256];
    IterationData *input = malloc(sizeof(IterationData));
    input->targetID = fileSystem->pwd;
    input->parentID = 0;
    while (input->parentID != kHFSRootParentID) {
        catalogIteration(fileSystem, fileSystem->catalog->header->firstLeafNode, input, name, &nameByIdCallback);
        input->targetID = input->parentID;
        if (input->parentID == kHFSRootParentID) {
            strcpy(name, "");
        }
        strcat(name, "/");
        output = realloc(output, strlen(output) + strlen(name) + 1);
        char *tmp = malloc(strlen(output) + strlen(name) + 1);
        strcpy(tmp, name);
        strcat(tmp, output);
        strcpy(output, tmp);
        free(tmp);
    }
    free(input);
    return output;
}

char *cd(FileSystem *fileSystem, char *path) {
    char *output = malloc(1);
    output[0] = '\0';
    if (path == NULL) {
        return output;
    }
    NodeInfo *info = findFileByPath(fileSystem, fileSystem->pwd, path);
    if (info->id == 0) {
        char *message = "No such file or directory\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
    } else if (info->type != kHFSPlusFolderRecord) {
        char *message = "Not a directory\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
    } else if (info->id != kHFSRootParentID) {
        fileSystem->pwd = info->id;
    }
    free(info);
    return output;
}

char *cp(FileSystem *fileSystem, char *path, char *outPath) {
    char *output = malloc(1);
    output[0] = '\0';
    if (path == NULL || outPath == NULL) {
        char *message = "Empty path\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
        return output;
    }
    NodeInfo *info = findFileByPath(fileSystem, fileSystem->pwd, path);
    if (info->id == 0) {
        char *message = "No such file or directory\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
    } else if (copy(fileSystem, info, outPath) == 0) {
        char *message = "Copied successfully\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
    } else {
        char *message = "Error\n";
        output = realloc(output, strlen(output) + strlen(message) + 1);
        sprintf(output, "%s", message);
    }
    free(info);
    return output;
}

