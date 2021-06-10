#pragma once
#include <stdint.h>
#include <HFSPlusBTree.h>
#include <HFSCatalog.h>

void GetNextBlockNum(uint64_t* _nodeBlockNumber, uint64_t* _extentNum, uint64_t* _currentBlockNum,
                     BTNodeDescriptor descriptor, FileSystem fs);
void ReadNodeDescriptor(FileSystem fs, uint64_t nodeBlockNumber, BTNodeDescriptor *descriptor, char *rawNode);
void FillRecordAddress(BTHeaderRec btreeHeader, BTNodeDescriptor descriptor, char *rawNode, uint16_t *recordAddress);
char *HFSStringToBytes(HFSUniStr255 hfsStr);
bool HFSStrToStrCmp(HFSUniStr255 hfsStr, const char *str);
bool CheckForHFSPrivateDataNode(HFSPlusCatalogKey key);
void PrintHFSUnicode(HFSUniStr255 str, FileSystem* fs);

#define CAST_PTR_TO_TYPE(type, ptr) *(type*)ptr