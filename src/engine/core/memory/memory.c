#include "memory.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

struct memory_statistics {
    size_t total_allocated;
    size_t tag_allocations[MEMORY_TAG_MAX_TAGS];
};

static const char * memory_tag_strings[MEMORY_TAG_MAX_TAGS] = {
    "NONE",
    "ARRAY",
    "FILE I/O"
};

static struct memory_statistics stats;

void memory_init() {
    memset(&stats, 0, sizeof(stats));
}

void memory_shutdown() {}

void * memory_allocate(size_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_NONE) {
        WARN("memory_allocate() was called with MEMORY_TAG_NONE.");
    }

    stats.total_allocated += size;
    stats.tag_allocations[tag] += size;

    void * block = malloc(size);
    memset(block, 0, size);

    return block;
}

// if realloc is passed a null pointer, it acts like malloc
void * memory_reallocate(void * ptr, size_t old_size, size_t new_size, memory_tag tag) {
    if (tag == MEMORY_TAG_NONE) {
        WARN("memory_reallocate() was called with MEMORY_TAG_NONE.");
    }
    stats.total_allocated += new_size - old_size;
    stats.tag_allocations[tag] += new_size - old_size;

    return realloc(ptr, new_size);
}

void memory_free(void * block, size_t size, memory_tag tag) {
    if (tag == MEMORY_TAG_NONE) {
        WARN("memory_free() was called with MEMORY_TAG_NONE.");
    }

    stats.total_allocated -= size;
    stats.tag_allocations[tag] -= size;

    free(block);
}

void * memory_zero(void * block, size_t size) {
    return memset(block, 0, size);
}

void * memory_copy(void * dest, const void * source, size_t size) {
    return memcpy(dest, source, size);
}

void * memory_set(void * dest, int32_t value, size_t size) {
    return memset(dest, value, size);
}

// Temporary
char * memory_usage_str() {
    const uint64_t GIB = 1024 * 1024 * 1024; 
    const uint64_t MIB = 1024 * 1024; 
    const uint64_t KIB = 1024;

    char buffer[8000] = "System memory use: \n";
    size_t offset = strlen(buffer);
    for (size_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
        char unit[4] = "XiB";
        float amount = 1.0f;
        if (stats.tag_allocations[i] >= GIB) {
            unit[0] = 'G';
            amount = stats.tag_allocations[i] / (float)GIB;
        } else if (stats.tag_allocations[i] >= MIB) {
            unit[0] = 'M';
            amount = stats.tag_allocations[i] / (float)MIB;
        } else if (stats.tag_allocations[i] >= KIB) {
            unit[0] = 'K';
            amount = stats.tag_allocations[i] / (float)KIB;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (float) stats.tag_allocations[i];
        }
        int length = snprintf(buffer + offset, 8000, " %s: %.2f%s\n", memory_tag_strings[i], amount, unit);
        offset += length;
    }
    char * out = strdup(buffer);
    return out;
}
