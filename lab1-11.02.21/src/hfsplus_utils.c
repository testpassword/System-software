#include "./headers/hfsplus_utils.h"

void reverseHFSPlusBSDInfo(HFSPlusBSDInfo *s) {
    s->ownerID = bswap_32(s->ownerID);
    s->groupID = bswap_32(s->groupID);
    s->fileMode = bswap_16(s->fileMode);
    s->special.iNodeNum = bswap_32(s->special.iNodeNum);
    s->special.linkCount = bswap_32(s->special.linkCount);
    s->special.rawDevice = bswap_32(s->special.rawDevice);
}

void reverseHFSPlusForkData(HFSPlusForkData *s) {
    s->logicalSize = bswap_64(s->logicalSize);
    s->clumpSize = bswap_32(s->clumpSize);
    s->totalBlocks = bswap_32(s->totalBlocks);
    for (int i = 0; i < 8; i++) {
        s->extents[i].blockCount = bswap_32(s->extents[i].blockCount);
        s->extents[i].startBlock = bswap_32(s->extents[i].startBlock);
    }
}

void reverseHFSPlusVolumeHeader(HFSPlusVolumeHeader *s) {
    s->signature = bswap_16(s->signature);
    s->version = bswap_16(s->version);
    s->attributes = bswap_32(s->attributes);
    s->lastMountedVersion = bswap_32(s->lastMountedVersion);
    s->journalInfoBlock = bswap_32(s->journalInfoBlock);
    s->createDate = bswap_32(s->createDate);
    s->modifyDate = bswap_32(s->modifyDate);
    s->backupDate = bswap_32(s->backupDate);
    s->checkedDate = bswap_32(s->checkedDate);
    s->fileCount = bswap_32(s->fileCount);
    s->folderCount = bswap_32(s->folderCount);
    s->blockSize = bswap_32(s->blockSize);
    s->totalBlocks = bswap_32(s->totalBlocks);
    s->freeBlocks = bswap_32(s->freeBlocks);
    s->nextAllocation = bswap_32(s->nextAllocation);
    s->rsrcClumpSize = bswap_32(s->rsrcClumpSize);
    s->dataClumpSize = bswap_32(s->dataClumpSize);
    s->nextCatalogID = bswap_32(s->nextCatalogID);
    s->writeCount = bswap_32(s->writeCount);
    s->encodingsBitmap = bswap_64(s->encodingsBitmap);
    for (int i = 0; i < 8; i++) s->finderInfo[i] = bswap_32(s->finderInfo[i]);
    reverseHFSPlusForkData(&s->allocationFile);
    reverseHFSPlusForkData(&s->attributesFile);
    reverseHFSPlusForkData(&s->extentsFile);
    reverseHFSPlusForkData(&s->catalogFile);
    reverseHFSPlusForkData(&s->startupFile);
}

void reverseBTNodeDescriptor(BTNodeDescriptor *s) {
    s->fLink = bswap_32(s->fLink);
    s->bLink = bswap_32(s->bLink);
    s->numRecords = bswap_16(s->numRecords);
    s->reserved = bswap_16(s->reserved);
}

void reverseBTHeaderRec(BTHeaderRec *s) {
    s->treeDepth = bswap_16(s->treeDepth);
    s->rootNode = bswap_32(s->rootNode);
    s->leafRecords = bswap_32(s->leafRecords);
    s->firstLeafNode = bswap_32(s->firstLeafNode);
    s->lastLeafNode = bswap_32(s->lastLeafNode);
    s->nodeSize = bswap_16(s->nodeSize);
    s->maxKeyLength = bswap_16(s->maxKeyLength);
    s->totalNodes = bswap_32(s->totalNodes);
    s->freeNodes = bswap_32(s->freeNodes);
    s->clumpSize = bswap_32(s->clumpSize);
    s->attributes = bswap_32(s->attributes);
}

void reverseHFSPlusCatalogFolder(HFSPlusCatalogFolder *s) {
    s->recordType = bswap_16(s->recordType);
    s->flags = bswap_16(s->flags);
    s->valence = bswap_32(s->valence);
    s->folderID = bswap_32(s->folderID);
    s->createDate = bswap_32(s->createDate);
    s->contentModDate = bswap_32(s->contentModDate);
    s->attributeModDate = bswap_32(s->attributeModDate);
    s->accessDate = bswap_32(s->accessDate);
    s->backupDate = bswap_32(s->backupDate);
    s->textEncoding = bswap_32(s->textEncoding);
    reverseHFSPlusBSDInfo(&s->permissions);
}

void reverseHFSPlusCatalogFile(HFSPlusCatalogFile *s) {
    s->recordType = bswap_16(s->recordType);
    s->flags = bswap_16(s->flags);
    s->fileID = bswap_32(s->fileID);
    s->createDate = bswap_32(s->createDate);
    s->contentModDate = bswap_32(s->contentModDate);
    s->attributeModDate = bswap_32(s->attributeModDate);
    s->accessDate = bswap_32(s->accessDate);
    s->backupDate = bswap_32(s->backupDate);
    s->textEncoding = bswap_32(s->textEncoding);
    reverseHFSPlusBSDInfo(&s->permissions);
    reverseHFSPlusForkData(&s->dataFork);
}