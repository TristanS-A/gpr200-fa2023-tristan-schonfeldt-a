//
// Created by tropi on 9/28/2023.
//

#include "texture.h"
#include "../ew/external/stb_image.h"
#include "ew/external/glad.h"

unsigned int loadTexture(const char* filePath){
    stbi_set_flip_vertically_on_load(false);

    int width, height, numComponents;
    unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
    if (data == NULL) {
        printf("Failed to load image %s", filePath);
        stbi_image_free(data);
        return 0;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLenum fileFormat;
    switch(numComponents){
        case 1:
            fileFormat = GL_RED;
        case 2:
            fileFormat = GL_RG;
        case 3:
            fileFormat = GL_RGB;
        case 4:
            fileFormat = GL_RGBA;
        default:
            fileFormat = GL_RED;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, fileFormat, width, height, 0, fileFormat, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE, 0);
    stbi_image_free(data);
    return  texture;
}