#pragma once

long long readItem(void * buffer, size_t structSize, size_t amount, FILE * file);
int seekItem(FILE *file, long int offset, int mode);