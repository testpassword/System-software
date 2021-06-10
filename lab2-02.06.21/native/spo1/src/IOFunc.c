#include <stddef.h>
#include <stdio.h>
#include "Commander.h"
#include <HFSCatalog.h>
#include <List.h>
#include <HFSCatalog.h>
#include <HFSPlusBTree.h>

bool isRoot = true;

void CopyDirectory(const char* _src, const char* _dest, uint32_t parentID, BTHeaderRec btreeHeader, FileSystem fs) {
    CopyInfo copyInfo;
    const size_t _srcLen = strlen(_src) + 256;
    const size_t _destLen = strlen(_dest) + 256;
    char *src = calloc(_srcLen, 1);
    char *dest = calloc(_destLen, 1);
    strcpy(src, _src);
    strcpy(dest, _dest);
    char *srcCopy = calloc(_srcLen, 1);
    strcpy(srcCopy, _src);
    PathListNode* list = SplitPathWithDelimeter(srcCopy, "/");
    free(srcCopy);
    if (list) {
        PathListNode* lastNode = GetPathListLastNode(&list);
        strcat(dest, "/");
        strcat(dest, lastNode->token);
    }
    MakePath(dest);
    printf("coping dir %s done\n", dest);
    copyInfo.dest = dest;
    PathListNode * childrenDirs = GetChildrenDirectoriesList(parentID, btreeHeader, fs, copyInfo);
    PathListNode * childrenDirsListHead = childrenDirs;
    srcCopy = calloc(_srcLen, 1);
    while(childrenDirs) {
        strcpy(srcCopy, _src);
        strcat(srcCopy, "/");
        strcat(srcCopy, childrenDirs->token);
        CopyDirectory(srcCopy, dest, childrenDirs->_cnid, btreeHeader, fs);
        memset(srcCopy, 0, _srcLen);
        childrenDirs = childrenDirs->next;
    }
    free(src);
    free(dest);
    free(srcCopy);
}

void CopyFile(const char* dest, const char* filename, HFSPlusCatalogFile file, FileSystem* fs) {
    MakePath(dest);
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", dest, filename);
    FILE* destFile = NULL;
    destFile = fopen(filePath, "wb");
    if (destFile == NULL) {
        fprintf(stderr, "empty file\n");
        return;
    }
    for (int i = 0; i < 8; i++) {
        HFSPlusExtentRecord extent = file.dataFork.extents[i];
        if (extent.startBlock != 0 && extent.blockCount != 0)
            for (int offset = 0; offset < extent.blockCount; offset++)
                CopyFileBlock(extent.startBlock + offset, destFile, fs);
    }
    fclose(destFile);
}

void CopyFileBlock(uint64_t blockNum, FILE* fileDestination, FileSystem* fs) {
    char* fileBlock = calloc(1, fs->blockSize);
    fseek(fs->file, blockNum * fs->blockSize, SEEK_SET);
    uint32_t read = fread(fileBlock, fs->blockSize, 1, fs->file);
    fwrite(fileBlock, fs->blockSize, 1, fileDestination);
    free(fileBlock);
}

void MakePath(char* dir) {
    char cwd[512];
    if (dir[0] == '.') getcwd(cwd, sizeof(cwd));
    char tmp[1024];
    char* p = NULL;
    size_t len;
    if (dir[0] == '.') snprintf(tmp, sizeof(tmp), "%s%s", cwd, dir + 1);
    else snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}

PathListNode* IterateDevices(ProbeInfo* info) {
    blkid_dev device;
    blkid_dev_iterate iterator = blkid_dev_iterate_begin(info->blkCache);
    const double gibibyteDivider = pow(2, 30);
    const double mibibyteDivider = pow(2, 20);
    while (blkid_dev_next(iterator, &device) == 0) {
        const char * devName = blkid_dev_devname(device);
        printf("%s", devName);
        if (isRoot) {
            blkid_probe probe = blkid_new_probe_from_filename(devName);
            if (probe == NULL) {
                fprintf(stderr, "Launch util as root to get more information!\n");
                isRoot = false;
            }
            else {
                blkid_loff_t probeSize = blkid_probe_get_size(probe);
                printf("\t");
                if (probeSize >= gibibyteDivider) printf("%lld GiB\t", (long long) (probeSize / gibibyteDivider));
                else if (probeSize < gibibyteDivider) printf("%lld MiB\t", (long long) (probeSize / mibibyteDivider));
                blkid_do_probe(probe);
                const char *fsType;
                blkid_probe_lookup_value(probe, "TYPE", &fsType, NULL);
                printf("%s", fsType);
            }
        }
        printf("\n");
    }
    blkid_dev_iterate_end(iterator);
    return 0;
}

long long readItem(void * buffer, size_t structSize, size_t amount, FILE * file) {
    if (fread(buffer, structSize, amount, file) != amount) {
        fprintf(stderr, (feof(file)) ? "Unexpected EOF!\n" : "Can't read!\n");
        return -1;
    }
    return amount;
}

int seekItem(FILE *file, long int offset, int mode) {
    if (fseek(file, offset, mode)) {
        fprintf(stderr, "Can't set 1024 bytes offset!\n");
        return -1;
    }
    return 0;
}

int Init(ProbeInfo* info) {
    int status = blkid_get_cache(&info->blkCache, NULL);
    if (status < 0) {
        fprintf(stderr, "Can't initialize blkid lib!\n");
        return -1;
    }
    return 0;
}