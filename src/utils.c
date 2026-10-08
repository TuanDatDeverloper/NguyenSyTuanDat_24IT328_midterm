#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "utils.h"

#ifndef S_ISUID
#define S_ISUID 04000
#endif
#ifndef S_ISGID
#define S_ISGID 02000
#endif
#ifndef S_ISVTX
#define S_ISVTX 01000
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) 0
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) 0
#endif
#ifndef S_ISLNK
#define S_ISLNK(m) 0
#endif

void format_mode_string(mode_t mode, char *buf) {
    if (!buf) return;

    // Loại file
    if (S_ISDIR(mode)) buf[0] = 'd';
    else if (S_ISLNK(mode)) buf[0] = 'l';
    else if (S_ISCHR(mode)) buf[0] = 'c';
    else if (S_ISBLK(mode)) buf[0] = 'b';
    else if (S_ISSOCK(mode)) buf[0] = 's';
    else if (S_ISFIFO(mode)) buf[0] = 'p';
    else buf[0] = '-';

    // Quyền owner
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
        buf[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    // Quyền group
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
        buf[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    // Quyền others
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
        buf[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    buf[10] = '\0';
}

void format_size_human(off_t bytes, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    const char suffixes[] = {'B', 'K', 'M', 'G', 'T', 'P'};
    double val = (double)bytes;
    int idx = 0;

    while (val >= 1024.0 && idx < 5) {
        val /= 1024.0;
        idx++;
    }

    if (idx == 0) {
        snprintf(buf, buflen, "%lld%c", (long long)bytes, suffixes[idx]);
    } else if (val < 10.0) {
        snprintf(buf, buflen, "%.1f%c", val, suffixes[idx]);
    } else {
        snprintf(buf, buflen, "%.0f%c", val, suffixes[idx]);
    }
}

void format_time(time_t t, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (!tm_info) {
        snprintf(buf, buflen, "            ");
        return;
    }

    // Nếu trong khoảng 6 tháng (180 ngày) thì in giờ phút, xa hơn thì in năm
    long six_months_sec = 180L * 24L * 3600L;
    if (t <= now && (now - t) < six_months_sec) {
        strftime(buf, buflen, "%b %e %H:%M", tm_info);
    } else {
        strftime(buf, buflen, "%b %e  %Y", tm_info);
    }
}

void print_escaped_name(const char *name, const Options *opts) {
    if (!name) return;

    for (const char *p = name; *p != '\0'; p++) {
        unsigned char c = (unsigned char)*p;
        if (opts && opts->print_question && !isprint(c)) {
            putchar('?');
        } else {
            putchar(c);
        }
    }
}

char get_classified_suffix(mode_t mode) {
    if (S_ISDIR(mode)) return '/';
    if (S_ISLNK(mode)) return '@';
    if (S_ISSOCK(mode)) return '=';
    if (S_ISFIFO(mode)) return '|';
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) return '*';
    return '\0';
}

long long calculate_blocks(const struct stat *st, const Options *opts) {
#if defined(_WIN32)
    long long bytes = st->st_size;
    long long raw_512_blocks = (bytes + 511) / 512;
#else
    long long raw_512_blocks = st->st_blocks;
#endif

    if (opts->block_kb) {
        return (raw_512_blocks * 512 + 1023) / 1024;
    }

    if (opts->block_size != 512 && opts->block_size > 0) {
        return (raw_512_blocks * 512 + opts->block_size - 1) / opts->block_size;
    }

    return raw_512_blocks;
}
