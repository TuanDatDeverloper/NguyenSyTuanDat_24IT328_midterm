#ifndef SORT_H
#define SORT_H

#include <stddef.h>
#include "file_info.h"
#include "options.h"

// Sắp xếp danh sách file theo các cờ (-S, -t, -r, -f...)
void sort_file_info_array(FileInfo **array, size_t count, const Options *opts);

#endif
