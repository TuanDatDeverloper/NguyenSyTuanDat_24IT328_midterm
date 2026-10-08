#ifndef OPTIONS_H
#define OPTIONS_H

typedef enum {
    TIME_MODIFIED = 0, // mtime mặc định
    TIME_STATUS_CHANGE, // -c: ctime
    TIME_ACCESS         // -u: atime
} TimeType;

typedef struct {
    int show_all;           // -a
    int show_almost_all;    // -A
    TimeType time_type;     // -c hoặc -u
    int list_dir_as_plain;  // -d
    int classify;           // -F
    int unsorted;           // -f
    int human_readable;     // -h
    int show_inode;         // -i
    int block_kb;           // -k
    int long_format;        // -l
    int numeric_id;         // -n
    int print_question;     // -q
    int recursive;          // -R
    int reverse_sort;       // -r
    int sort_by_size;       // -S
    int show_blocks;        // -s
    int sort_by_time;       // -t
    int raw_chars;          // -w
    long block_size;        // lấy từ biến môi trường BLOCKSIZE
    int is_terminal;        // kiểm tra output có phải terminal không
} Options;

// Khởi tạo giá trị mặc định cho options
void init_options(Options *opts);

// Xử lý các cờ từ dòng lệnh
int parse_options(int argc, char *argv[], Options *opts);

#endif
