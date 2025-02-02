#pragma once

#include <stdint.h>

typedef struct BTHeaderRec BTHeaderRec;
typedef struct HFSPlusCatalogFile HFSPlusCatalogFile;
typedef struct FileSystem FileSystem;

void MakePath(char* dir);
void CopyFile(const char* dest, const char* filename, HFSPlusCatalogFile file, FileSystem* fs);
void CopyDirectory(const char* _src, const char* _dest, uint32_t parentID, BTHeaderRec btreeHeader, FileSystem fs);

typedef struct CopyInfo {
    char* dest;
} CopyInfo;

long long readItem(void * buffer, size_t structSize, size_t amount, FILE * file);
int seekItem(FILE *file, long int offset, int mode);