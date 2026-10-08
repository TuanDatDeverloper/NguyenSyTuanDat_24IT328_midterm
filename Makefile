CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700 -D_NETBSD_SOURCE -Iinclude
TARGET = ls

OBJS = src/main.o \
       src/options.o \
       src/file_info.o \
       src/sort.o \
       src/display.o \
       src/traverse.o \
       src/utils.c

OBJS = src/main.o \
       src/options.o \
       src/file_info.o \
       src/sort.o \
       src/display.o \
       src/traverse.o \
       src/utils.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

src/options.o: src/options.c
	$(CC) $(CFLAGS) -c src/options.c -o src/options.o

src/file_info.o: src/file_info.c
	$(CC) $(CFLAGS) -c src/file_info.c -o src/file_info.o

src/sort.o: src/sort.c
	$(CC) $(CFLAGS) -c src/sort.c -o src/sort.o

src/display.o: src/display.c
	$(CC) $(CFLAGS) -c src/display.c -o src/display.o

src/traverse.o: src/traverse.c
	$(CC) $(CFLAGS) -c src/traverse.c -o src/traverse.o

src/utils.o: src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

clean:
	-rm -f src/*.o $(TARGET) $(TARGET).exe

.PHONY: all clean
