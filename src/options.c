#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "options.h"

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#endif

void init_options(Options *opts) {
    if (!opts) return;
    memset(opts, 0, sizeof(Options));

    opts->time_type = TIME_MODIFIED;
    opts->is_terminal = isatty(STDOUT_FILENO);

    // Mặc định: terminal thì dùng -q, chuyển hướng (pipe/file) thì dùng -w
    if (opts->is_terminal) {
        opts->print_question = 1;
        opts->raw_chars = 0;
    } else {
        opts->print_question = 0;
        opts->raw_chars = 1;
    }

    // Kích thước block mặc định là 512 bytes, có thể ghi đè bởi biến môi trường BLOCKSIZE
    opts->block_size = 512;
    const char *bs_env = getenv("BLOCKSIZE");
    if (bs_env && *bs_env) {
        long val = atol(bs_env);
        if (val > 0) {
            opts->block_size = val;
        }
    }
}

int parse_options(int argc, char *argv[], Options *opts) {
    if (!opts) return -1;
    init_options(opts);

    int opt;
    while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (opt) {
            case 'A':
                opts->show_almost_all = 1;
                break;
            case 'a':
                opts->show_all = 1;
                break;
            case 'c':
                // -c và -u ghi đè nhau
                opts->time_type = TIME_STATUS_CHANGE;
                break;
            case 'u':
                opts->time_type = TIME_ACCESS;
                break;
            case 'd':
                // -d và -R ghi đè nhau
                opts->list_dir_as_plain = 1;
                opts->recursive = 0;
                break;
            case 'R':
                opts->recursive = 1;
                opts->list_dir_as_plain = 0;
                break;
            case 'F':
                opts->classify = 1;
                break;
            case 'f':
                // -f không sắp xếp và mặc định bật hiển thị file ẩn
                opts->unsorted = 1;
                opts->show_all = 1;
                break;
            case 'h':
                // -h và -k ghi đè nhau (cờ đứng sau được ưu tiên)
                opts->human_readable = 1;
                opts->block_kb = 0;
                break;
            case 'k':
                opts->block_kb = 1;
                opts->human_readable = 0;
                break;
            case 'i':
                opts->show_inode = 1;
                break;
            case 'l':
                // -l và -n ghi đè nhau
                opts->long_format = 1;
                opts->numeric_id = 0;
                break;
            case 'n':
                opts->long_format = 1;
                opts->numeric_id = 1;
                break;
            case 'q':
                // -q và -w ghi đè nhau
                opts->print_question = 1;
                opts->raw_chars = 0;
                break;
            case 'w':
                opts->raw_chars = 1;
                opts->print_question = 0;
                break;
            case 'r':
                opts->reverse_sort = 1;
                break;
            case 'S':
                // Sắp xếp theo kích thước (hủy cờ theo thời gian)
                opts->sort_by_size = 1;
                opts->sort_by_time = 0;
                break;
            case 's':
                opts->show_blocks = 1;
                break;
            case 't':
                // Sắp xếp theo thời gian (hủy cờ theo kích thước)
                opts->sort_by_time = 1;
                opts->sort_by_size = 0;
                break;
            default:
                return -1;
        }
    }

    return optind;
}
