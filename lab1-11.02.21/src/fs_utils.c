#include "./headers/fs_utils.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stddef.h>

FileSystem* open_fs(char* path) {
    HFSPlusVolumeHeader* header = malloc(sizeof(struct HFSPlusVolumeHeader));
    int fd = open(path, O_RDONLY, 00666);
    pread(fd, header, sizeof(struct HFSPlusVolumeHeader), HEADER_OFFSET);
    reverseHFSPlusVolumeHeader(header);
    if (header->signature == HFS_PLUS_SIGNATURE && header->version == HFS_PLUS_VERSION) {
        FileSystem *fileSystem = malloc(sizeof(FileSystem));
        fileSystem->header = header;
        fileSystem->descriptor = fd;
        fileSystem->catalog = open_btree(fileSystem, typeCatalog);
        fileSystem->pwd = kHFSRootFolderID;
        return fileSystem;
    }
    free(header);
    close(fd);
    return NULL;
}

void close_fs(FileSystem* fs) {
    close_btree(fs->catalog);
    close(fs->descriptor);
    free(fs->header);
    free(fs);
}

File* open_file_fork(FileSystem* fs, HFSPlusForkData* fork) {
    File* file = malloc(sizeof(File));
    file->data = malloc(fork->totalBlocks * fs->header->blockSize);
    file->size = fork->logicalSize;
    int read_blocks = 0;
    int ext_counter = 0;
    int block_size = fs->header->blockSize;
    while (ext_counter < 8 && read_blocks < fork->totalBlocks) {
        pread(fs->descriptor,
              file->data + (read_blocks * block_size),
              fork->extents[ext_counter].blockCount * block_size,
              fork->extents[ext_counter].startBlock * block_size);
        read_blocks += fork->extents[ext_counter].blockCount;
        ext_counter++;
    }
    return file;
}

void close_file(File* file) {
    free(file->data);
    free(file);
}

BTree* open_btree(FileSystem* fs, enum BTreeType type) {
    BTree* catalog = malloc(sizeof(BTree));
    if (type == typeAllocation) catalog->file = open_file_fork(fs, &fs->header->allocationFile);
    else if (type == typeExtent) catalog->file = open_file_fork(fs, &fs->header->extentsFile);
    else if (type == typeCatalog) catalog->file = open_file_fork(fs, &fs->header->catalogFile);
    else if (type == typeStartup) catalog->file = open_file_fork(fs, &fs->header->startupFile);
    else catalog->file = open_file_fork(fs, &fs->header->attributesFile);
    catalog->file = open_file_fork(fs, &fs->header->catalogFile);
    catalog->header = catalog->file->data + sizeof(BTNodeDescriptor);
    reverseBTHeaderRec(catalog->header);
    return catalog;
}

void close_btree(BTree* catalog) {
    close_file(catalog->file);
    free(catalog);
}

Node* open_node(int node_num, BTree* tree) {
    if (node_num >= tree->header->totalNodes) return NULL;
    Node* node = malloc(sizeof(Node));
    node->descriptor = malloc(tree->header->nodeSize);
    memcpy(node->descriptor, tree->file->data + tree->header->nodeSize * node_num, tree->header->nodeSize);
    reverseBTNodeDescriptor(node->descriptor);
    node->record_offsets = ((UInt16 *) ((void *) node->descriptor + tree->header->nodeSize)) - node->descriptor->numRecords;
    for (int i = 0; i < node->descriptor->numRecords; i++) node->record_offsets[i] = bswap_16(node->record_offsets[i]);
    return node;
}

void close_node(Node* node) {
    free(node->descriptor);
    free(node);
}

Record* open_record(int record_num, Node* node) {
    if (record_num >= node->descriptor->numRecords) return NULL;
    Record* record = malloc(sizeof(Record));
    int offset = node->record_offsets[node->descriptor->numRecords - 1 - record_num];
    record->size = node->record_offsets[node->descriptor->numRecords - 2 - record_num] - offset;
    record->data = malloc(record->size);
    void* data = ((void *) node->descriptor) + offset;
    memcpy(record->data, data, record->size);
    record->key_length = *((UInt16 *) record->data);
    record->key_length = bswap_16(record->key_length);
    record->data_offset = record->key_length + sizeof(UInt16);
    return record;
}

void close_record(Record* record) {
    free(record->data);
    free(record);
}

void to_string(char* output, UInt16* raw, int length) {
    strcpy(output, "");
    for (int i = 0; i < length; i++) {
        UInt16 key_character = bswap_16(raw[i]);
        if (key_character < 128) {
            char c = key_character;
            strncat(output, &c, 1);
        } else if (key_character < 3778) {
            strncat(output, (const char *) ((key_character >> 6) + 0xC0), 1);
            strncat(output, (const char *) ((key_character & 0x3F) + 0x80), 1);
        }
    }
}

void catalog_iteration(FileSystem* fs, UInt32 node_num, IterationData* data, void* output,
                      void (*callback) (IterationData* input, void* output)) {
    Node* node = open_node(node_num, fs->catalog);
    UInt32 nodeChild = node->descriptor->fLink;
    data->break_flag = 0;
    for (int i = 0; i < node->descriptor->numRecords; i++) {
        Record *record = open_record(i, node);
        data->record = record;
        data->parent_id = bswap_32(*(UInt32 *) (record->data + sizeof(UInt16)));
        data->type = bswap_16(*(SInt16 *) (record->data + sizeof(UInt16) + record->key_length));
        strcpy(data->node_name, "");
        UInt16 *key = record->data + sizeof(UInt16) + sizeof(UInt32);
        UInt16 nameLength = bswap_16(key[0]);
        to_string(data->node_name, key + 1, nameLength);
        callback(data, output);
        close_record(record);
        if (data->break_flag) return;
    }
    close_node(node);
    if (nodeChild > 0) catalog_iteration(fs, nodeChild, data, output, callback);
}

void name_by_id_callback(IterationData* input, void* output) {
    if (input->type == kHFSPlusFolderRecord) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        if (folder->folderID == input->target_id) {
            sprintf(output, "%s", input->node_name);
            input->break_flag = 1;
        }
    } else if (input->type == kHFSPlusFileRecord) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        if (file->fileID == input->target_id) {
            sprintf(output, "%s", input->node_name);
            input->break_flag = 1;
        }
    }
}

void parent_by_id_callback(IterationData* input, void* output) {
    NodeInfo* nodeInfo = (NodeInfo *) output;
    if (input->type == kHFSPlusFolderRecord) {
        HFSPlusCatalogFolder* folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        if (folder->folderID == input->target_id) {
            nodeInfo->id = input->parent_id;
            nodeInfo->type = folder->recordType;
            input->break_flag = 1;
        }
    }
}

void id_by_name_callback(IterationData* input, void* output) {
    NodeInfo* nodeInfo = (NodeInfo *) output;
    if (input->type == kHFSPlusFolderRecord &&
        input->parent_id == input->target_id &&
        strcmp(input->target_name, input->node_name) == 0) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        nodeInfo->id = folder->folderID;
        nodeInfo->type = folder->recordType;
        strcpy(nodeInfo->name, input->node_name);
        input->break_flag = 1;
    } else if (input->type == kHFSPlusFileRecord &&
               input->parent_id == input->target_id &&
               strcmp(input->target_name, input->node_name) == 0) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        nodeInfo->id = file->fileID;
        nodeInfo->type = file->recordType;
        strcpy(nodeInfo->name, input->node_name);
        nodeInfo->data = file->dataFork;
        input->break_flag = 1;
    }
}

void ls_callback(IterationData* input, void* outputPointer) {
    NodeInfoArray* array = (NodeInfoArray*) outputPointer;
    if (input->type == kHFSPlusFolderRecord && input->parent_id == input->target_id) {
        HFSPlusCatalogFolder *folder = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFolder(folder);
        array->data = realloc(array->data, (array->size + 1) * sizeof(NodeInfo));
        array->data[array->size].id = folder->folderID;
        strcpy(array->data[array->size].name, input->node_name);
        array->data[array->size].type = folder->recordType;
        array->size++;
    } else if (input->type == kHFSPlusFileRecord && input->parent_id == input->target_id) {
        HFSPlusCatalogFile *file = input->record->data + input->record->data_offset;
        reverseHFSPlusCatalogFile(file);
        array->data = realloc(array->data, (array->size + 1) * sizeof(NodeInfo));
        array->data[array->size].id = file->fileID;
        strcpy(array->data[array->size].name, input->node_name);
        array->data[array->size].type = file->recordType;
        array->data[array->size].data = file->dataFork;
        array->size++;
    }
}

NodeInfo* find_file_by_name(FileSystem* fs, HFSCatalogNodeID parentID, char* path) {
    NodeInfo* nodeInfo = malloc(sizeof(NodeInfo));
    nodeInfo->id = 0;
    IterationData *input = malloc(sizeof(IterationData));
    input->target_id = parentID;
    strcpy(input->target_name, path);
    if (strcmp(path, ".") == 0) {
        char name[256];
        catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, name, &name_by_id_callback);
        strcpy(nodeInfo->name, name);
        nodeInfo->type = kHFSPlusFolderRecord;
        nodeInfo->id = parentID;
    } else if (strcmp(path, "..") == 0) {
        catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, nodeInfo, &parent_by_id_callback);
        input->target_id = nodeInfo->id;
        catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, nodeInfo->name, &name_by_id_callback);
    } else catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, nodeInfo, &id_by_name_callback);
    free(input);
    return nodeInfo;
}

NodeInfo* find_file_by_path(FileSystem* fs, HFSCatalogNodeID parentID, char* path) {
    char* str = malloc(strlen(path) + 1);
    char* strPointer = str;
    NodeInfo* result = NULL;
    strcpy(str, path);
    if (str[0] == '/') {
        strsep(&str, "/");
        if (strlen(str) == 0) {
            result = malloc(sizeof(NodeInfo));
            result->id = kHFSRootFolderID;
            result->type = kHFSPlusFolderRecord;
            IterationData *input = malloc(sizeof(IterationData));
            input->target_id = kHFSRootFolderID;
            catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, result->name, &name_by_id_callback);
            free(input);
        } else result = find_file_by_path(fs, kHFSRootFolderID, str);
    } else {
        char* current_dir = strsep(&str, "/");
        NodeInfo* currentNode = find_file_by_name(fs, parentID, current_dir);
        if (str == NULL || strlen(str) == 0) result = currentNode;
        else {
            result = find_file_by_path(fs, currentNode->id, str);
            free(currentNode);
        }
    }
    free(strPointer);
    return result;
}

int copy(FileSystem* fs, NodeInfo* info, char* path) {
    char* nodePath = malloc(strlen(path) + strlen(info->name) + 2);
    strcpy(nodePath, path);
    strcat(nodePath, "/");
    strcat(nodePath, info->name);
    if (info->type == kHFSPlusFileRecord) {
        int fd = open(nodePath, O_CREAT | O_WRONLY | O_TRUNC, 00666);
        if (fd == -1) {
            free(nodePath);
            return -1;
        }
        File* file = open_file_fork(fs, &info->data);
        pwrite(fd, file->data, file->size, 0);
        close_file(file);
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
        input->target_id = info->id;
        catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, &nodeInfoArray, &ls_callback);
        for (int i = 0; i < nodeInfoArray.size; i++) {
            if (copy(fs, nodeInfoArray.data + i, nodePath) != 0) {
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

char* get_partitions_str() {
    const char partitions_path[] = "/sys/block";
    DIR* sys_block_dir = opendir(partitions_path);
    struct dirent* disk;
    char* all_partitions = malloc(255 * 255);
    void concat_device(char str[]) {
        strcat(all_partitions, "/dev/");
        strcat(all_partitions, str);
        strcat(all_partitions, "\n");
    };
    if (sys_block_dir) {
        while ((disk = readdir(sys_block_dir)) != NULL) {
            char* d_name = disk->d_name;
            if (strcmp(d_name, ".") &&  strcmp(d_name, "..") && !strstr(d_name, "ram") && !strstr(d_name, "loop")) {
                concat_device(d_name);
                char disk_path[FILENAME_MAX] = { 0 };
                strcat(strcat(disk_path, partitions_path), d_name);
                DIR* sys_block_subdir = opendir(disk_path);
                struct dirent* partition;
                if (sys_block_subdir)
                    while ((partition = readdir(sys_block_subdir)) != NULL)
                        if (!memcmp(partition->d_name, d_name, strlen(d_name))) concat_device(partition->d_name);
                closedir(sys_block_subdir);
            }
        }
        closedir(sys_block_dir);
    }
    return all_partitions;
}

char* ls(FileSystem* fs, char* path) {
    char* list;
    NodeInfo* info = (path == NULL) ? find_file_by_name(fs, fs->pwd, ".") : find_file_by_path(fs, fs->pwd, path);
    if (info->id == 0) return "path didn't exists";
    else if (info->type != kHFSPlusFolderRecord) return "it's not a dir";
    NodeInfoArray nodeInfoArray;
    nodeInfoArray.size = 0;
    nodeInfoArray.data = malloc(sizeof(NodeInfo));
    IterationData* input = malloc(sizeof(IterationData));
    input->target_id = info->id;
    catalog_iteration(fs, fs->catalog->header->firstLeafNode, input, &nodeInfoArray, &ls_callback);
    list = malloc(263 * nodeInfoArray.size);
    list[0] = '\0';
    char result[263]; //255 - maximal length of name, 7 - "FOLDER\t", 1 - "\0"
    for (int i = 0; i < nodeInfoArray.size; i++) {
        if (nodeInfoArray.data[i].type == kHFSPlusFolderRecord) {
            sprintf(result, "FOLDER\t%s\n", nodeInfoArray.data[i].name);
            strcat(list, result);
        }
        if (nodeInfoArray.data[i].type == kHFSPlusFileRecord) {
            sprintf(result, "FILE\t%s\n", nodeInfoArray.data[i].name);
            strcat(list, result);
        }
    }
    free(input);
    free(nodeInfoArray.data);
    free(info);
    return list;
}

char* pwd(FileSystem* fs) {
    char* res_path = malloc(2);
    res_path[0] = '\n';
    res_path[1] = '\0';
    char buf[256];
    IterationData* root_dir = malloc(sizeof(IterationData));
    root_dir->target_id = fs->pwd;
    root_dir->parent_id = 0;
    while (root_dir->parent_id != kHFSRootParentID) {
        catalog_iteration(fs, fs->catalog->header->firstLeafNode, root_dir, buf, &name_by_id_callback);
        root_dir->target_id = root_dir->parent_id;
        if (root_dir->parent_id == kHFSRootParentID) strcpy(buf, "");
        strcat(buf, "/");
        res_path = realloc(res_path, strlen(res_path) + strlen(buf) + 1);
        char* tmp = malloc(strlen(res_path) + strlen(buf) + 1);
        strcpy(tmp, buf);
        strcat(tmp, res_path);
        strcpy(res_path, tmp);
        free(tmp);
    }
    free(root_dir);
    return res_path;
}

char* cd(FileSystem* fs, char* path) {
    if (path == NULL) return "";
    else {
        NodeInfo* info = find_file_by_path(fs, fs->pwd, path);
        if (info->id == 0 || path == NULL) {
            free(info);
            return "path didn't exists";
        }
        else if (info->type != kHFSPlusFolderRecord) {
            free(info);
            return "it's not a dir";
        }
        else if (info->id != kHFSRootParentID) fs->pwd = info->id;
        free(info);
        return "";
    }
}

char* cp(FileSystem* fs, char* src, char* dest) {
    if (src == NULL || dest == NULL) return "path not specified";
    NodeInfo* info = find_file_by_path(fs, fs->pwd, src);
    if (info->id == 0) {
        free(info);
        return "file or dir didn't exists";
    }
    int res_code = copy(fs, info, dest);
    free(info);
    return (res_code == 0) ? "done" : "unexpected error";
}