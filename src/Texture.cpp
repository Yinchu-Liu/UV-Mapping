/**
 * @file Texture.cpp
 * @brief Implementation of the Texture class for OpenGL texture management
 */
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

/**
 * @brief Constructs a new Texture object and generates an OpenGL texture ID
 */
Texture::Texture() : textureID(0), width(0), height(0), channels(0) {
    // Generate texture ID immediately
    glGenTextures(1, &textureID);
    if (textureID == 0) {
        std::cerr << "Failed to generate texture ID" << std::endl;
    }
}

/**
 * @brief Destroys the Texture object and releases OpenGL resources
 */
Texture::~Texture() {
    cleanup();
}

/**
 * @brief Loads texture data from a file and uploads it to the GPU
 * @param filename Path to the texture file to load
 * @return true if the texture loaded successfully, false otherwise
 */
bool Texture::loadFromFile(const std::string& filename) {
    if (textureID == 0) {
        std::cerr << "Invalid texture ID" << std::endl;
        return false;
    }

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    stbi_set_flip_vertically_on_load(true);
    
    std::cout << "Loading texture from file: " << filename << std::endl;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    std::cout << "Texture loaded successfully. Size: " << width << "x" << height << ", Channels: " << channels << std::endl;

    try {
        GLenum format;
        switch (channels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default:
                std::cerr << "Unsupported number of channels: " << channels << std::endl;
                stbi_image_free(data);
                return false;
        }

        // For single-channel textures, we need to set swizzle mask to repeat R channel
        if (channels == 1) {
            GLint swizzleMask[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
            glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        
        // Check for OpenGL errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL error when creating texture: " << err << std::endl;
            stbi_image_free(data);
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        
        std::cout << "Texture uploaded to GPU successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception when creating texture: " << e.what() << std::endl;
        if (data) {
            stbi_image_free(data);
        }
        return false;
    }
}

/**
 * @brief Binds the texture to a specific texture unit
 * @param slot Texture unit to bind to (defaults to 0)
 */
void Texture::bind(unsigned int slot) const {
    if (textureID == 0) {
        std::cerr << "Attempting to bind invalid texture" << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

/**
 * @brief Unbinds the texture from the current texture unit
 */
void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * @brief Releases OpenGL resources associated with the texture
 */
void Texture::cleanup() {
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}
