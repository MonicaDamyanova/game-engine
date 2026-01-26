#include "texture.h"

#include <stdint.h>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "logger.h"

Texture texture_load(const char * path) {
    Texture texture = {0};

    //stbi_set_flip_vertically_on_load(1);
    unsigned char * data = stbi_load(path, &texture.width, &texture.height, &texture.channels, 0);

    if (!data) {
        ERROR("Failed to load texture: %s", path);
        return texture;
    }
    INFO("Loaded texture %s", path);
    
    glGenTextures(1, &texture.ref);
    glBindTexture(GL_TEXTURE_2D, texture.ref);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    INFO("Number of channels: %d", texture.channels);
    GLenum format = GL_RED;
    if (texture.channels == 3) format = GL_RGB;
    else if (texture.channels == 4) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void texture_destroy(Texture * texture) {
    if (texture->ref) {
        glDeleteTextures(1, &texture->ref);
        texture->ref = 0;
    }
}

void texture_bind(Texture * texture, uint32_t unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->ref);

}

