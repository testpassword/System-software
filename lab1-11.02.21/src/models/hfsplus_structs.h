#pragma once
#include <byteswap.h>
#define HFS_PLUS_SIGNATURE 0x482b
#define HFS_PLUS_VERSION 4
#define HEADER_OFFSET 1024

typedef unsigned char UInt8;
typedef signed char SInt8;
typedef unsigned short UInt16;
typedef signed short SInt16;
typedef unsigned int UInt32;
typedef unsigned long UInt64;
typedef UInt16 UniChar;

struct HFSUniStr255 {
    UInt16 length;
    UniChar unicode[255];
}__attribute__((packed));
typedef struct HFSUniStr255 HFSUniStr255;
typedef const HFSUniStr255 *ConstHFSUniStr255Param;

struct HFSPlusBSDInfo {
    UInt32 ownerID;
    UInt32 groupID;
    UInt8 adminFlags;
    UInt8 ownerFlags;
    UInt16 fileMode;
    union {
        UInt32 iNodeNum;
        UInt32 linkCount;
        UInt32 rawDevice;
    } special;
}__attribute__((packed));
typedef struct HFSPlusBSDInfo HFSPlusBSDInfo;

struct HFSPlusExtentDescriptor {
    UInt32 startBlock;
    UInt32 blockCount;
}__attribute__((packed));
typedef struct HFSPlusExtentDescriptor HFSPlusExtentDescriptor;
typedef HFSPlusExtentDescriptor HFSPlusExtentRecord[8];

struct HFSPlusForkData {
    UInt64 logicalSize;
    UInt32 clumpSize;
    UInt32 totalBlocks;
    HFSPlusExtentRecord extents;
}__attribute__((packed));
typedef struct HFSPlusForkData HFSPlusForkData;
typedef UInt32 HFSCatalogNodeID;

struct HFSPlusVolumeHeader {
    UInt16 signature;
    UInt16 version;
    UInt32 attributes;
    UInt32 lastMountedVersion;
    UInt32 journalInfoBlock;
    UInt32 createDate;
    UInt32 modifyDate;
    UInt32 backupDate;
    UInt32 checkedDate;
    UInt32 fileCount;
    UInt32 folderCount;
    UInt32 blockSize;
    UInt32 totalBlocks;
    UInt32 freeBlocks;
    UInt32 nextAllocation;
    UInt32 rsrcClumpSize;
    UInt32 dataClumpSize;
    HFSCatalogNodeID nextCatalogID;
    UInt32 writeCount;
    UInt64 encodingsBitmap;
    UInt32 finderInfo[8];
    HFSPlusForkData allocationFile;
    HFSPlusForkData extentsFile;
    HFSPlusForkData catalogFile;
    HFSPlusForkData attributesFile;
    HFSPlusForkData startupFile;
}__attribute__((packed));
typedef struct HFSPlusVolumeHeader HFSPlusVolumeHeader;

struct BTNodeDescriptor {
    UInt32 fLink;
    UInt32 bLink;
    SInt8 kind;
    UInt8 height;
    UInt16 numRecords;
    UInt16 reserved;
}__attribute__((packed));
typedef struct BTNodeDescriptor BTNodeDescriptor;

enum BTreeType {
    typeAllocation = 0,
    typeExtent = 1,
    typeCatalog = 2,
    typeAttributes = 3,
    typeStartup = 4
};

struct BTHeaderRec {
    UInt16 treeDepth;
    UInt32 rootNode;
    UInt32 leafRecords;
    UInt32 firstLeafNode;
    UInt32 lastLeafNode;
    UInt16 nodeSize;
    UInt16 maxKeyLength;
    UInt32 totalNodes;
    UInt32 freeNodes;
    UInt16 reserved1;
    UInt32 clumpSize;
    UInt8 btreeType;
    UInt8 keyCompareType;
    UInt32 attributes;
    UInt32 reserved3[16];
}__attribute__((packed));
typedef struct BTHeaderRec BTHeaderRec;

enum {
    kHFSRootParentID = 1,
    kHFSRootFolderID = 2,
    kHFSExtentsFileID = 3,
    kHFSCatalogFileID = 4,
    kHFSBadBlockFileID = 5,
    kHFSAllocationFileID = 6,
    kHFSStartupFileID = 7,
    kHFSAttributesFileID = 8,
    kHFSRepairCatalogFileID = 14,
    kHFSBogusExtentFileID = 15,
    kHFSFirstUserCatalogNodeID = 16
};

struct HFSPlusCatalogKey {
    UInt16 keyLength;
    HFSCatalogNodeID parentID;
    HFSUniStr255 nodeName;
}__attribute__((packed));
typedef struct HFSPlusCatalogKey HFSPlusCatalogKey;

enum {
    kHFSPlusFolderRecord = 0x0001,
    kHFSPlusFileRecord = 0x0002,
    kHFSPlusFolderThreadRecord = 0x0003,
    kHFSPlusFileThreadRecord = 0x0004
};

struct HFSPlusCatalogFolder {
    SInt16 recordType;
    UInt16 flags;
    UInt32 valence;
    HFSCatalogNodeID folderID;
    UInt32 createDate;
    UInt32 contentModDate;
    UInt32 attributeModDate;
    UInt32 accessDate;
    UInt32 backupDate;
    HFSPlusBSDInfo permissions;
    UInt32 userInfo[4];
    UInt32 finderInfo[4];
    UInt32 textEncoding;
    UInt32 reserved;
}__attribute__((packed));
typedef struct HFSPlusCatalogFolder HFSPlusCatalogFolder;

struct HFSPlusCatalogFile {
    SInt16 recordType;
    UInt16 flags;
    UInt32 reserved1;
    HFSCatalogNodeID fileID;
    UInt32 createDate;
    UInt32 contentModDate;
    UInt32 attributeModDate;
    UInt32 accessDate;
    UInt32 backupDate;
    HFSPlusBSDInfo permissions;
    UInt32 userInfo[4];
    UInt32 finderInfo[4];
    UInt32 textEncoding;
    UInt32 reserved2;
    HFSPlusForkData dataFork;
    HFSPlusForkData resourceFork;
}__attribute__((packed));
typedef struct HFSPlusCatalogFile HFSPlusCatalogFile;