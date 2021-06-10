#include <stddef.h>
#include <stdio.h>
#include "Commander.h"

bool isRoot = true;

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