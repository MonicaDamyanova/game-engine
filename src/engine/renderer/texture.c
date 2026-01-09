#include "texture.h"

#include <stdint.h>

#include <glad/glad.h>
#include <stb_image.h>

#include "logger.h"

Texture texture_load(const char * path) {
    Texture texture = {0};

    stbi_set_flip_vertically_on_load(1);
    unsigned char * data = stbi_load(path, &texture.width, &texture.height, &texture.channels, 0);

    if (!data) {
        ERROR("Failed to load texture: %s", path);
        return texture;
    }
    
    glGenTextures(1, &texture.ref);
    glBindTexture(GL_TEXTURE_2D, texture.ref);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = tex.channels == 4 ? GL_RGBA : tex.channels == 3 ? GL_RGB : GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, format, tex.width, tex.height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void texture_destroy(Texture* texture) {
    if (texture->ref) {
        glDeleteTextures(1, &texture->ref);
        texture->ref = 0;
    }
}

void texture_bind(const Texture* texture, uint32_t unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->ref);
}

