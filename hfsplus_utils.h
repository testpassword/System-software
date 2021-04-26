#pragma once

#include <stddef.h>
#include "hfs_plus_data_structures.h"

typedef struct File {
    long size;
    void *data;
} File;

typedef struct BTree {
    File *file;
    BTHeaderRec *header;
} BTree;

typedef struct FileSystem {
    int deviceDescriptor;
    HFSPlusVolumeHeader *volumeHeader;
    BTree *catalog;
    HFSCatalogNodeID pwd;
} FileSystem;

typedef struct Node {
    BTNodeDescriptor *descriptor;
    UInt16 *record_offsets;
} Node;

typedef struct Record {
    void *data;
    UInt16 size;
    UInt16 data_offset;
    UInt16 key_length;
} Record;

typedef struct IterationData {
    Record *record;
    SInt16 type;
    HFSCatalogNodeID parentID;
    HFSCatalogNodeID targetID;
    char nodeName[256];
    char targetName[256];
    char breakFlag;
} IterationData;

typedef struct NodeInfo {
    HFSCatalogNodeID id;
    SInt16 type;
    char nodeName[256];
    HFSPlusForkData data;
} NodeInfo;

typedef struct NodeInfoArray {
    int size;
    NodeInfo *data;
} NodeInfoArray;

FileSystem *openFileSystem(char *name);

void closeFileSystem(FileSystem *fileSystem);

File *openFileFromFork(FileSystem *fileSystem, HFSPlusForkData *forkData);

void closeFile(File *file);

BTree *openBTree(FileSystem *fileSystem, enum BTreeType type);

void closeBTree(BTree *catalog);

Node *openNode(int node_num, BTree *tree);

void closeNode(Node *node);

Record *openRecord(int record_num, Node *node);

void closeRecord(Record *record);

void lsCallback(IterationData *input, void *output);

//void findByNameCallback(IterationData * input, void* output);

void convertNameToString(char *output, UInt16 *rawName, int nameLength);

void catalogIteration(FileSystem *fileSystem, UInt32 nodeNumber, IterationData *data, void *output,
                      void (*callback)(IterationData *input, void *output));


char *ls(FileSystem *fileSystem, char *path);

char *pwd(FileSystem *fileSystem);

char *cd(FileSystem *fileSystem, char *path);

char *cp(FileSystem *fileSystem, char *path, char *outPath);