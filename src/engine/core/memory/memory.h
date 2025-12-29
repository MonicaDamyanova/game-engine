#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum memory_tag {
    MEMORY_TAG_NONE,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_FILE_IO,

    MEMORY_TAG_MAX_TAGS
} memory_tag;

void memory_init();
void memory_shutdown();

void * memory_allocate(size_t size, memory_tag tag);
void * memory_reallocate(void * ptr, size_t old_size, size_t new_size, memory_tag tag);
void   memory_free(void * block, size_t size, memory_tag tag);
void * memory_zero(void * block, size_t size);
void * memory_copy(void * dest, const void * source, size_t size);
void * memory_set(void * dest, int32_t value, size_t size);

char * memory_usage_str();
