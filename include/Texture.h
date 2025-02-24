/**
 * @file Texture.h
 * @brief Header file for the Texture class, which handles loading and managing OpenGL textures
 */
#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
#include <glad/glad.h>
#include <string>

/**
 * @class Texture
 * @brief Manages OpenGL texture resources, including loading from file and binding to texture units
 */
class Texture {
public:
    /**
     * @brief Constructs a new Texture object
     */
    Texture();

    /**
     * @brief Destroys the Texture object and releases OpenGL resources
     */
    ~Texture();

    /**
     * @brief Loads texture data from a file
     * @param filename Path to the texture file to load
     * @return true if the texture loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Binds the texture to a specific texture unit
     * @param slot Texture unit to bind to (defaults to 0)
     */
    void bind(unsigned int slot = 0) const;

    /**
     * @brief Unbinds the texture from the current texture unit
     */
    void unbind() const;

private:
    GLuint textureID; ///< OpenGL texture ID
    int width;       ///< Width of the texture in pixels
    int height;      ///< Height of the texture in pixels
    int channels;    ///< Number of color channels in the texture

    /**
     * @brief Releases OpenGL resources associated with the texture
     */
    void cleanup();
};

#endif // TEXTURE_H
