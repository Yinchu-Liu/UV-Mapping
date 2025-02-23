#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const std::string& filename);
    void bind(unsigned int slot = 0) const;
    void unbind() const;

private:
    GLuint textureID;
    int width, height, channels;

    void cleanup();
};

#endif // TEXTURE_H
