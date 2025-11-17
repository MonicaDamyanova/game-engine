#include "io.h"

#include "../logger.h"
#include "../memory.h"

#include <stdio.h>
//#include <stdlib.h>
#include <errno.h>

// 20MiB, but can depend on target platform.
#define IO_READ_CHUNK_SIZE 2097152
#define IO_READ_ERROR_GENERAL "Error reading file: %s. errno: %d\n"
#define IO_READ_ERROR_MEMORY "Not enough free memory to read file: %s\n"

// From this video:  https://www.youtube.com/watch?v=27quVo5wsWo
File io_file_read(const char *path) {
    File file = { .is_valid = false };

    FILE *fp = fopen(path, "rb");

    if(fp == NULL || ferror(fp)) {
        ERROR(IO_READ_ERROR_GENERAL, path, errno);
        return file;
    }

    char *data = NULL;
    char *tmp;
    size_t used = 0;
    size_t old_size = 0;
    size_t size = 0;
    size_t n;

    while (true) {
        if (used + IO_READ_CHUNK_SIZE + 1 > size) {
            size = used + IO_READ_CHUNK_SIZE + 1;

            if (size <= used) {
                memory_free(data, old_size, MEMORY_TAG_FILE_IO);
                ERROR("Input file too large: %s\n", path);
                return file;
            }

            //tmp = realloc(data, size);
            tmp = memory_reallocate(data, old_size, size, MEMORY_TAG_FILE_IO);
            if (!tmp) {
                memory_free(data, size, MEMORY_TAG_FILE_IO);
                ERROR(IO_READ_ERROR_MEMORY, path);
                return file;
            }
            data = tmp;
        }

        old_size = size;

        n = fread(data + used, 1, IO_READ_CHUNK_SIZE, fp);
        if (n == 0) 
            break;

        used += n;
    }

    if (ferror(fp)) {
        memory_free(data, size, MEMORY_TAG_FILE_IO);
        ERROR(IO_READ_ERROR_GENERAL, path, errno);
        return file;
    }

    //tmp = realloc(data, used + 1);
    tmp = memory_reallocate(data, size, used + 1, MEMORY_TAG_FILE_IO);
    if (!tmp) {
        memory_free(data, used + 1, MEMORY_TAG_FILE_IO);
        ERROR(IO_READ_ERROR_MEMORY, path);
        return file;
    }
    data = tmp;
    data[used] = 0; // null terminator

    file.data = data;
    file.len = used;
    file.is_valid = true;

    fclose(fp); // Include?

    return file;
}

int io_file_write(void *buffer, size_t size, const char *path);
