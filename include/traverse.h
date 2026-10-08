#ifndef TRAVERSE_H
#define TRAVERSE_H

#include "options.h"

// Xử lý các đối số truyền vào từ dòng lệnh
int process_operands(int operand_count, char *operands[], const Options *opts);

// Đọc và duyệt nội dung một thư mục (hỗ trợ đệ quy -R)
int traverse_directory(const char *dir_path, const Options *opts, int print_header);

#endif
