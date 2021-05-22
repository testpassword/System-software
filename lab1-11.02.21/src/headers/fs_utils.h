#pragma once
#include "../models/fs_structs.h"

FileSystem* open_fs(char* path);
void close_fs(FileSystem* fs);
File* open_file_fork(FileSystem* fs, HFSPlusForkData* fork);
void close_file(File* file);
BTree* open_btree(FileSystem* fs, enum BTreeType type);
void close_btree(BTree* catalog);
Node* open_node(int node_num, BTree* tree);
void close_node(Node* node);
Record* open_record(int record_num, Node* node);
void close_record(Record* record);
void to_string(char* output, UInt16* raw, int length);
void catalog_iteration(FileSystem* fs, UInt32 node_num, IterationData* data, void* output,
                      void (*callback) (IterationData* input, void* output));
void ls_callback(IterationData* input, void* output);
char* get_partitions_str();
char* ls(FileSystem* fs, char* path);
char* pwd(FileSystem* fs);
char* cd(FileSystem* fs, char* path);
char* cp(FileSystem* fs, char* src, char* dest);