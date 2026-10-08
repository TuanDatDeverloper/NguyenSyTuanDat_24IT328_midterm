#ifndef DISPLAY_H
#define DISPLAY_H

#include <stddef.h>
#include "file_info.h"
#include "options.h"

// In danh sách file ra màn hình (cột, định dạng dài -l, -s, -i...)
void display_entries(FileInfo **entries, size_t count, const Options *opts, int is_directory_content);

#endif
