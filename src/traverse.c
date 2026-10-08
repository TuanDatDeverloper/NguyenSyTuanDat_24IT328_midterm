#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "traverse.h"
#include "file_info.h"
#include "sort.h"
#include "display.h"

int traverse_directory(const char *dir_path, const Options *opts, int print_header) {
    if (!dir_path || !opts) return 1;

    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "ls: %s: %s\n", dir_path, strerror(errno));
        return 1;
    }

    if (print_header) {
        printf("%s:\n", dir_path);
    }

    size_t capacity = 32;
    size_t count = 0;
    FileInfo **entries = (FileInfo **)malloc(capacity * sizeof(FileInfo *));
    if (!entries) {
        closedir(dir);
        return 1;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        const char *name = ent->d_name;

        // Lọc file ẩn theo -a và -A
        if (opts->show_all) {
            // Hiển thị tất cả
        } else if (opts->show_almost_all) {
            // Bỏ qua "." và ".."
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
                continue;
            }
        } else {
            // Mặc định: bỏ qua các file bắt đầu bằng '.'
            if (name[0] == '.') {
                continue;
            }
        }

        FileInfo *fi = create_file_info(dir_path, name, opts);
        if (!fi) continue;

        if (count >= capacity) {
            capacity *= 2;
            FileInfo **tmp = (FileInfo **)realloc(entries, capacity * sizeof(FileInfo *));
            if (!tmp) {
                free_file_info(fi);
                break;
            }
            entries = tmp;
        }
        entries[count++] = fi;
    }

    closedir(dir);

    sort_file_info_array(entries, count, opts);
    display_entries(entries, count, opts, 1);

    int ret_status = 0;

    // Đệ quy thư mục con (-R) nếu không có cờ -d
    if (opts->recursive && !opts->list_dir_as_plain) {
        for (size_t i = 0; i < count; i++) {
            FileInfo *fi = entries[i];
            if (!fi || !fi->stat_ok) continue;

            if (S_ISDIR(fi->st.st_mode)) {
                // Tránh lặp vô tận với "." và ".."
                if (strcmp(fi->name, ".") == 0 || strcmp(fi->name, "..") == 0) {
                    continue;
                }
                putchar('\n');
                int r = traverse_directory(fi->full_path, opts, 1);
                if (r != 0) ret_status = r;
            }
        }
    }

    free_file_info_array(entries, count);
    return ret_status;
}

int process_operands(int operand_count, char *operands[], const Options *opts) {
    if (operand_count == 0) {
        // Mặc định không truyền đối số thì duyệt thư mục hiện tại
        return traverse_directory(".", opts, 0);
    }

    FileInfo **non_dirs = (FileInfo **)malloc(operand_count * sizeof(FileInfo *));
    FileInfo **dirs = (FileInfo **)malloc(operand_count * sizeof(FileInfo *));
    size_t non_dir_count = 0;
    size_t dir_count = 0;
    int had_error = 0;

    // Phân loại: file thường và thư mục
    for (int i = 0; i < operand_count; i++) {
        const char *op = operands[i];
        FileInfo *fi = create_file_info("", op, opts);
        if (!fi) {
            had_error = 1;
            continue;
        }

        if (!fi->stat_ok) {
            fprintf(stderr, "ls: %s: %s\n", op, strerror(fi->error_num));
            free_file_info(fi);
            had_error = 1;
            continue;
        }

        if (S_ISDIR(fi->st.st_mode) && !opts->list_dir_as_plain) {
            dirs[dir_count++] = fi;
        } else {
            non_dirs[non_dir_count++] = fi;
        }
    }

    sort_file_info_array(non_dirs, non_dir_count, opts);
    sort_file_info_array(dirs, dir_count, opts);

    // Hiển thị các file thường trước
    if (non_dir_count > 0) {
        display_entries(non_dirs, non_dir_count, opts, 0);
    }

    int print_headers = (operand_count > 1 || opts->recursive);

    // Duyệt tiếp các thư mục
    for (size_t i = 0; i < dir_count; i++) {
        if (non_dir_count > 0 || i > 0) {
            putchar('\n');
        }
        int r = traverse_directory(dirs[i]->full_path, opts, print_headers);
        if (r != 0) had_error = 1;
    }

    free_file_info_array(non_dirs, non_dir_count);
    free_file_info_array(dirs, dir_count);

    return had_error ? 1 : 0;
}
