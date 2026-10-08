# Bài Tập Giữa Kỳ: Cài Đặt Lệnh ls (NetBSD ls(1))

- **Sinh viên thực hiện**: Nguyễn Sỹ Tuấn Đạt
- **Mã số sinh viên**: 24IT328
- **Repository**: [https://github.com/TuanDatDeverloper/NguyenSyTuanDat_24IT328_midterm](https://github.com/TuanDatDeverloper/NguyenSyTuanDat_24IT328_midterm)
- **Tài liệu tham khảo**: Manual NetBSD 10.1 `ls(1)` (`ls.pdf`).

---

## 1. Giới thiệu đề tài

Bài tập yêu cầu hiện thực lại lệnh `ls` bằng ngôn ngữ C (chuẩn C99) theo đặc tả của hệ điều hành NetBSD/POSIX. Chương trình hỗ trợ 19 cờ (flags), xử lý đúng các quy tắc ghi đè cờ, duyệt đệ quy thư mục và hiển thị chi tiết thông tin file.

## 2. Cấu trúc thư mục dự án

Dự án được chia thành các module với thư mục `include/` chứa file header và `src/` chứa mã nguồn:

```text
NguyenSyTuanDat_24IT328_midterm/
├── Makefile            # Script biên dịch chương trình (make, make clean)
├── README.md           # Báo cáo và hướng dẫn sử dụng
├── .gitignore          # Bỏ qua file binary và file .o
├── include/
│   ├── options.h       # Định nghĩa struct Options và hàm đọc cờ
│   ├── file_info.h     # Định nghĩa struct FileInfo và đọc stat
│   ├── sort.h          # Khai báo hàm sắp xếp danh sách file
│   ├── display.h       # Khai báo hàm hiển thị kết quả ra màn hình
│   ├── traverse.h      # Khai báo hàm duyệt thư mục và xử lý đối số
│   └── utils.h         # Các hàm tiện ích (chuyển quyền, format dung lượng, ngày tháng)
└── src/
    ├── main.c          # Hàm main điều khiển luồng chính
    ├── options.c       # Xử lý getopt và quy tắc ghi đè cờ
    ├── file_info.c     # Lấy thông tin file qua lstat/stat, đọc symlink
    ├── sort.c          # Sắp xếp theo tên, kích thước (-S), thời gian (-t), đảo ngược (-r)
    ├── display.c       # In kết quả, căn chỉnh lề các cột, in định dạng dài (-l, -n)
    ├── traverse.c      # Đọc thư mục (opendir/readdir), duyệt đệ quy (-R)
    └── utils.c         # Format mode, kích thước (-h), ký tự phân loại (-F)
```

## 3. Danh sách các cờ đã hoàn thành (19 cờ)

1. `-a`: Hiển thị tất cả các file, bao gồm cả file ẩn bắt đầu bằng dấu chấm (`.`).
2. `-A`: Hiển thị gần như tất cả các file ẩn, ngoại trừ `.` và `..`.
3. `-c`: Sử dụng thời gian thay đổi trạng thái file (`ctime`) thay vì `mtime` khi sắp xếp (`-t`) hoặc hiển thị (`-l`).
4. `-d`: Xem thư mục như file bình thường, không liệt kê nội dung bên trong thư mục.
5. `-F`: Thêm ký hiệu chỉ loại file sau tên (`/` cho thư mục, `*` cho file thực thi, `@` cho symlink...).
6. `-f`: Không sắp xếp danh sách file (in theo thứ tự đọc trong thư mục) và mặc định bật hiển thị file ẩn.
7. `-h`: Hiển thị kích thước file ở dạng dễ đọc (B, K, M, G...).
8. `-i`: Hiển thị số inode của file.
9. `-k`: Hiển thị số block theo đơn vị Kilobyte (1024 bytes) cho cờ `-s`.
10. `-l`: Liệt kê chi tiết (dạng dài): quyền truy cập, số link, user, group, dung lượng, ngày sửa đổi, tên file.
11. `-n`: Tương tự cờ `-l`, nhưng hiển thị UID và GID dưới dạng số.
12. `-q`: Thay thế các ký tự không in được trong tên file bằng dấu `?`.
13. `-R`: Duyệt đệ quy tất cả các thư mục con bên trong.
14. `-r`: Đảo ngược thứ tự sắp xếp.
15. `-S`: Sắp xếp file theo kích thước giảm dần.
16. `-s`: Hiển thị số block hệ thống của từng file.
17. `-t`: Sắp xếp file theo thời gian sửa đổi (mới nhất lên đầu).
18. `-u`: Sử dụng thời gian truy cập gần nhất (`atime`) thay vì `mtime` khi sắp xếp (`-t`) hoặc hiển thị (`-l`).
19. `-w`: In nguyên bản các ký tự trong tên file (không escape).

## 4. Xử lý quy tắc ghi đè cờ (Overrides)

Theo đúng tài liệu manual NetBSD `ls(1)`, nếu các cờ xung đột xuất hiện cùng nhau trên dòng lệnh, cờ đứng sau sẽ ghi đè cờ đứng trước:

- `-w` và `-q`: Cờ xuất hiện sau sẽ quyết định cách in tên file. Khi xuất ra terminal mặc định là `-q`, khi redirect ra file/pipe mặc định là `-w`.
- `-l` và `-n`: Cờ đứng sau sẽ quyết định hiển thị tên user/group hay UID/GID dạng số.
- `-c` và `-u`: Cờ đứng sau quyết định mốc thời gian sử dụng (`ctime` hay `atime`).
- `-R` và `-d`: Nếu `-d` đứng sau thì tắt duyệt đệ quy `-R` (chỉ in bản thân thư mục).
- `-k` và `-h`: Cờ đứng sau sẽ ghi đè đơn vị hiển thị dung lượng.

## 5. Xử lý đối số dòng lệnh (Operands)

- Khi không truyền đối số: Chương trình mặc định liệt kê thư mục hiện tại (`.`).
- Khi truyền nhiều đối số: Chương trình phân loại các file thường và thư mục riêng biệt. Các file thường được in trước, sau đó mới lần lượt in nội dung của từng thư mục (có in tiêu đề `tên_thư_mục:` phía trước).

## 6. Hướng dẫn biên dịch và chạy thử

### Biên dịch:

Sử dụng `make` (hoặc `mingw32-make` trên Windows):

```bash
# Biên dịch ra file thực thi ls (hoặc ls.exe)
make

# Xóa các file object và file thực thi đã tạo
make clean
```

### Một số lệnh kiểm thử mẫu:

```bash
# 1. Liệt kê cơ bản
./ls

# 2. Liệt kê chi tiết kèm file ẩn
./ls -la

# 3. Liệt kê dạng dài, dung lượng dễ đọc và ký tự phân loại file
./ls -lh -F

# 4. Hiển thị UID/GID dạng số
./ls -n

# 5. Sắp xếp theo dung lượng giảm dần
./ls -lS

# 6. Sắp xếp theo thời gian mới nhất và đảo ngược
./ls -ltr

# 7. Duyệt đệ quy thư mục
./ls -R include

# 8. Xem thông tin của chính thư mục (không vào bên trong)
./ls -ld src
```
