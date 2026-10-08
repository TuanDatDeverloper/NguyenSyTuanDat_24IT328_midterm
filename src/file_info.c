#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "file_info.h"

#ifdef _WIN32
#define lstat stat
#endif

FileInfo *create_file_info(const char *dir_path, const char *entry_name, const Options *opts) {
    (void)opts;
    FileInfo *fi = (FileInfo *)malloc(sizeof(FileInfo));
    if (!fi) return NULL;
    memset(fi, 0, sizeof(FileInfo));

    fi->name = strdup(entry_name);
    if (!fi->name) {
        free(fi);
        return NULL;
    }

    // Ghép đường dẫn đầy đủ
    size_t full_len = 0;
    if (dir_path && *dir_path && strcmp(dir_path, ".") != 0) {
        size_t dlen = strlen(dir_path);
        int has_slash = (dir_path[dlen - 1] == '/' || dir_path[dlen - 1] == '\\');
        full_len = dlen + (has_slash ? 0 : 1) + strlen(entry_name) + 1;
        fi->full_path = (char *)malloc(full_len);
        if (fi->full_path) {
            snprintf(fi->full_path, full_len, "%s%s%s", dir_path, has_slash ? "" : "/", entry_name);
        }
    } else {
        fi->full_path = strdup(entry_name);
    }

    if (!fi->full_path) {
        free(fi->name);
        free(fi);
        return NULL;
    }

    // Dùng lstat để không bị dereference symlink
    if (lstat(fi->full_path, &fi->st) == 0) {
        fi->stat_ok = 1;

#if !defined(_WIN32)
        // Nếu là symbolic link thì đọc đích đến
        if (S_ISLNK(fi->st.st_mode)) {
            char linkbuf[1024];
            ssize_t len = readlink(fi->full_path, linkbuf, sizeof(linkbuf) - 1);
            if (len != -1) {
                linkbuf[len] = '\0';
                fi->link_target = strdup(linkbuf);
            }
        }
#endif
    } else {
        fi->stat_ok = 0;
        fi->error_num = errno;
    }

    return fi;
}

void free_file_info(FileInfo *fi) {
    if (!fi) return;
    if (fi->name) free(fi->name);
    if (fi->full_path) free(fi->full_path);
    if (fi->link_target) free(fi->link_target);
    free(fi);
}

void free_file_info_array(FileInfo **array, size_t count) {
    if (!array) return;
    for (size_t i = 0; i < count; i++) {
        if (array[i]) {
            free_file_info(array[i]);
        }
    }
    free(array);
}
