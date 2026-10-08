#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stddef.h>
#include "options.h"

// Chuyển quyền st_mode thành chuỗi 10 ký tự (vd: -rwxr-xr-x)
void format_mode_string(mode_t mode, char *buf);

// Đổi kích thước byte sang dạng dễ đọc (B, K, M, G...)
void format_size_human(off_t bytes, char *buf, size_t buflen);

// Định dạng thời gian theo chuẩn ls
void format_time(time_t t, char *buf, size_t buflen);

// In tên file, xử lý ký tự không in được với -q và -w
void print_escaped_name(const char *name, const Options *opts);

// Lấy ký tự hậu tố cho cờ -F (/ cho thư mục, * cho thực thi...)
char get_classified_suffix(mode_t mode);

// Tính số khối block hiển thị theo -k, -h và BLOCKSIZE
long long calculate_blocks(const struct stat *st, const Options *opts);

#endif
