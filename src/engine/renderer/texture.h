#pragma once

#include <stdint.h>

typedef struct {
    uint32_t ref;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
} Texture;

Texture texture_load(const char * path);
void texture_destry(Texture * texture);
void texture_bind(Texture * texture, uint32_t unit); 
