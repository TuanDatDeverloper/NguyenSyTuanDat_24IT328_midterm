#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <sys/types.h>
#include <sys/stat.h>
#include "options.h"

typedef struct {
    char *name;         // Tên file/thư mục
    char *full_path;    // Đường dẫn đầy đủ
    struct stat st;     // Thông tin lấy từ stat/lstat
    char *link_target;  // Đích đến nếu là symlink
    int stat_ok;        // 1 nếu stat thành công, 0 nếu lỗi
    int error_num;      // Mã lỗi errno nếu stat thất bại
} FileInfo;

// Tạo và lấy thông tin cho 1 file/thư mục
FileInfo *create_file_info(const char *dir_path, const char *entry_name, const Options *opts);

// Giải phóng bộ nhớ
void free_file_info(FileInfo *fi);
void free_file_info_array(FileInfo **array, size_t count);

#endif
