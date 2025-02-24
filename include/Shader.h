#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

/**
 * @class Shader
 * @brief Manages OpenGL shader programs, including loading, compiling, and using shaders.
 */
class Shader {
public:
    /**
     * @brief Default constructor for the Shader class.
     */
    Shader();

    /**
     * @brief Destructor for the Shader class.
     * Cleans up OpenGL resources.
     */
    ~Shader();

    /**
     * @brief Loads and compiles shaders from specified files.
     * @param vertexPath Path to the vertex shader file.
     * @param fragmentPath Path to the fragment shader file.
     * @return True if shaders are successfully loaded and compiled, false otherwise.
     */
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief Activates the shader program for rendering.
     */
    void use() const;

    /**
     * @brief Cleans up the shader program by deleting it from OpenGL.
     */
    void cleanup();

    // Uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint program; ///< OpenGL shader program ID

    /**
     * @brief Checks for shader compilation and linking errors.
     * @param shader The shader or program ID to check
     * @param type The type of shader ("VERTEX", "FRAGMENT", or "PROGRAM")
     */
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif // SHADER_H