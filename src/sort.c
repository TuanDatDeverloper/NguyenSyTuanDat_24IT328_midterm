#include <stdlib.h>
#include <string.h>
#include "sort.h"

static const Options *g_sort_opts = NULL;

static time_t get_file_time(const FileInfo *fi, const Options *opts) {
    if (!fi || !opts) return 0;
    if (opts->time_type == TIME_STATUS_CHANGE) {
        return fi->st.st_ctime;
    } else if (opts->time_type == TIME_ACCESS) {
        return fi->st.st_atime;
    }
    return fi->st.st_mtime;
}

static int compare_entries(const void *p1, const void *p2) {
    const FileInfo *a = *(const FileInfo **)p1;
    const FileInfo *b = *(const FileInfo **)p2;
    int res = 0;

    if (g_sort_opts && g_sort_opts->sort_by_size) {
        // Sắp xếp theo dung lượng giảm dần (-S)
        if (a->st.st_size > b->st.st_size) {
            res = -1;
        } else if (a->st.st_size < b->st.st_size) {
            res = 1;
        } else {
            res = strcmp(a->name, b->name);
        }
    } else if (g_sort_opts && g_sort_opts->sort_by_time) {
        // Sắp xếp theo mốc thời gian mới nhất trước (-t)
        time_t ta = get_file_time(a, g_sort_opts);
        time_t tb = get_file_time(b, g_sort_opts);

        if (ta > tb) {
            res = -1;
        } else if (ta < tb) {
            res = 1;
        } else {
            res = strcmp(a->name, b->name);
        }
    } else {
        // Mặc định: sắp xếp theo bảng chữ cái
        res = strcmp(a->name, b->name);
    }

    // Đảo ngược thứ tự sắp xếp nếu có -r
    if (g_sort_opts && g_sort_opts->reverse_sort) {
        res = -res;
    }

    return res;
}

void sort_file_info_array(FileInfo **array, size_t count, const Options *opts) {
    if (!array || count < 2 || !opts) return;

    // Cờ -f: giữ nguyên thứ tự đọc từ thư mục
    if (opts->unsorted) return;

    g_sort_opts = opts;
    qsort(array, count, sizeof(FileInfo *), compare_entries);
    g_sort_opts = NULL;
}
