#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "display.h"
#include "utils.h"

#if !defined(_WIN32)
#include <pwd.h>
#include <grp.h>
#if defined(__linux__)
#include <sys/sysmacros.h>
#endif
#ifndef major
#define major(x) ((int)(((uint32_t)(x) & 0x000fff00) >> 8))
#define minor(x) ((int)((((uint32_t)(x) & 0xfff00000) >> 12) | (((uint32_t)(x) & 0x000000ff) >> 0)))
#endif
#endif

// Lưu độ rộng lớn nhất của từng cột để căn lề đẹp mắt
typedef struct {
    int max_inode_w;
    int max_blocks_w;
    int max_links_w;
    int max_user_w;
    int max_group_w;
    int max_size_w;
} ColWidths;

static int count_digits(long long n) {
    if (n <= 0) return 1;
    int d = 0;
    while (n > 0) {
        d++;
        n /= 10;
    }
    return d;
}

static void compute_column_widths(FileInfo **entries, size_t count, const Options *opts, ColWidths *w) {
    memset(w, 0, sizeof(ColWidths));

    for (size_t i = 0; i < count; i++) {
        FileInfo *fi = entries[i];
        if (!fi || !fi->stat_ok) continue;

        if (opts->show_inode) {
            int len = count_digits((long long)fi->st.st_ino);
            if (len > w->max_inode_w) w->max_inode_w = len;
        }

        if (opts->show_blocks) {
            int len;
            if (opts->human_readable) {
                char hbuf[32];
                format_size_human(calculate_blocks(&fi->st, opts) * 512, hbuf, sizeof(hbuf));
                len = (int)strlen(hbuf);
            } else {
                len = count_digits(calculate_blocks(&fi->st, opts));
            }
            if (len > w->max_blocks_w) w->max_blocks_w = len;
        }

        if (opts->long_format) {
            int link_len = count_digits((long long)fi->st.st_nlink);
            if (link_len > w->max_links_w) w->max_links_w = link_len;

            int ulen = 0;
            if (!opts->numeric_id) {
#if !defined(_WIN32)
                struct passwd *pw = getpwuid(fi->st.st_uid);
                if (pw && pw->pw_name) {
                    ulen = (int)strlen(pw->pw_name);
                } else
#endif
                {
                    ulen = count_digits((long long)fi->st.st_uid);
                }
            } else {
                ulen = count_digits((long long)fi->st.st_uid);
            }
            if (ulen > w->max_user_w) w->max_user_w = ulen;

            int glen = 0;
            if (!opts->numeric_id) {
#if !defined(_WIN32)
                struct group *gr = getgrgid(fi->st.st_gid);
                if (gr && gr->gr_name) {
                    glen = (int)strlen(gr->gr_name);
                } else
#endif
                {
                    glen = count_digits((long long)fi->st.st_gid);
                }
            } else {
                glen = count_digits((long long)fi->st.st_gid);
            }
            if (glen > w->max_group_w) w->max_group_w = glen;

            int slen = 0;
#if !defined(_WIN32)
            if (S_ISCHR(fi->st.st_mode) || S_ISBLK(fi->st.st_mode)) {
                char devbuf[32];
                snprintf(devbuf, sizeof(devbuf), "%3d, %3d", major(fi->st.st_rdev), minor(fi->st.st_rdev));
                slen = (int)strlen(devbuf);
            } else
#endif
            if (opts->human_readable) {
                char hbuf[32];
                format_size_human(fi->st.st_size, hbuf, sizeof(hbuf));
                slen = (int)strlen(hbuf);
            } else {
                slen = count_digits((long long)fi->st.st_size);
            }
            if (slen > w->max_size_w) w->max_size_w = slen;
        }
    }
}

void display_entries(FileInfo **entries, size_t count, const Options *opts, int is_directory_content) {
    if (!entries || count == 0 || !opts) return;

    ColWidths w;
    compute_column_widths(entries, count, opts, &w);

    // In tổng số blocks nếu là nội dung thư mục khi dùng -l hoặc -s
    if (is_directory_content && (opts->long_format || (opts->show_blocks && opts->is_terminal))) {
        long long total_blocks = 0;
        for (size_t i = 0; i < count; i++) {
            if (entries[i] && entries[i]->stat_ok) {
                total_blocks += calculate_blocks(&entries[i]->st, opts);
            }
        }
        if (opts->human_readable) {
            char hbuf[32];
            format_size_human(total_blocks * 512, hbuf, sizeof(hbuf));
            printf("total %s\n", hbuf);
        } else {
            printf("total %lld\n", total_blocks);
        }
    }

    for (size_t i = 0; i < count; i++) {
        FileInfo *fi = entries[i];
        if (!fi) continue;

        if (opts->show_inode) {
            if (fi->stat_ok) {
                printf("%*llu ", w.max_inode_w, (unsigned long long)fi->st.st_ino);
            } else {
                printf("%*s ", w.max_inode_w, "?");
            }
        }

        if (opts->show_blocks) {
            if (fi->stat_ok) {
                if (opts->human_readable) {
                    char hbuf[32];
                    format_size_human(calculate_blocks(&fi->st, opts) * 512, hbuf, sizeof(hbuf));
                    printf("%*s ", w.max_blocks_w, hbuf);
                } else {
                    printf("%*lld ", w.max_blocks_w, calculate_blocks(&fi->st, opts));
                }
            } else {
                printf("%*s ", w.max_blocks_w, "?");
            }
        }

        if (opts->long_format) {
            if (!fi->stat_ok) {
                printf("??????????  1 ?        ?        ?            ");
            } else {
                char mode_str[12];
                format_mode_string(fi->st.st_mode, mode_str);
                printf("%s ", mode_str);

                printf("%*lu ", w.max_links_w, (unsigned long)fi->st.st_nlink);

                // Tên hoặc UID người sở hữu
                if (!opts->numeric_id) {
#if !defined(_WIN32)
                    struct passwd *pw = getpwuid(fi->st.st_uid);
                    if (pw && pw->pw_name) {
                        printf("%-*s ", w.max_user_w, pw->pw_name);
                    } else
#endif
                    {
                        printf("%-*u ", w.max_user_w, (unsigned int)fi->st.st_uid);
                    }
                } else {
                    printf("%-*u ", w.max_user_w, (unsigned int)fi->st.st_uid);
                }

                // Tên hoặc GID nhóm sở hữu
                if (!opts->numeric_id) {
#if !defined(_WIN32)
                    struct group *gr = getgrgid(fi->st.st_gid);
                    if (gr && gr->gr_name) {
                        printf("%-*s ", w.max_group_w, gr->gr_name);
                    } else
#endif
                    {
                        printf("%-*u ", w.max_group_w, (unsigned int)fi->st.st_gid);
                    }
                } else {
                    printf("%-*u ", w.max_group_w, (unsigned int)fi->st.st_gid);
                }

                // Kích thước file
#if !defined(_WIN32)
                if (S_ISCHR(fi->st.st_mode) || S_ISBLK(fi->st.st_mode)) {
                    printf("%3d, %3d ", major(fi->st.st_rdev), minor(fi->st.st_rdev));
                } else
#endif
                if (opts->human_readable) {
                    char sbuf[32];
                    format_size_human(fi->st.st_size, sbuf, sizeof(sbuf));
                    printf("%*s ", w.max_size_w, sbuf);
                } else {
                    printf("%*lld ", w.max_size_w, (long long)fi->st.st_size);
                }

                // Mốc thời gian
                time_t ftime = fi->st.st_mtime;
                if (opts->time_type == TIME_STATUS_CHANGE) {
                    ftime = fi->st.st_ctime;
                } else if (opts->time_type == TIME_ACCESS) {
                    ftime = fi->st.st_atime;
                }

                char tbuf[32];
                format_time(ftime, tbuf, sizeof(tbuf));
                printf("%s ", tbuf);
            }
        }

        print_escaped_name(fi->name, opts);

        // Hậu tố phân loại theo -F
        if (opts->classify && fi->stat_ok) {
            char suffix = get_classified_suffix(fi->st.st_mode);
            if (suffix != '\0') {
                putchar(suffix);
            }
        }

        // Đích đến nếu là symlink
        if (opts->long_format && fi->link_target) {
            printf(" -> ");
            print_escaped_name(fi->link_target, opts);
        }

        putchar('\n');
    }
}
