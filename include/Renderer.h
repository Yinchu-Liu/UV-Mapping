#ifndef RENDERER_H
#define RENDERER_H

#pragma once

#include <glad/glad.h>   // OpenGL function loader
#include <GLFW/glfw3.h>  // Window and input handling
#include <glm/glm.hpp>   // OpenGL Mathematics library for vectors and matrices
#include <glm/gtc/matrix_transform.hpp> // Transformations (translate, rotate, scale)
#include <glm/gtc/type_ptr.hpp>         // Convert glm types to OpenGL-compatible formats

#include "Mesh.h"    // Mesh class for 3D objects
#include "Shader.h"  // Shader class for managing GLSL programs
#include "Texture.h" // Texture class for loading and binding textures

#include <memory>  
#include <string>  

/**
 * @class Renderer
 * @brief Handles rendering of 3D objects using OpenGL.
 *
 * This class manages OpenGL initialization, rendering pipeline,
 * camera settings, lighting, and UI components.
 */
class Renderer {
public:
    /**
     * @brief Constructor to initialize the rendering context.
     * @param width  Window width
     * @param height Window height
     */
    Renderer(int width, int height);

    /**
     * @brief Destructor to clean up allocated resources.
     */
    ~Renderer();

    /**
     * @brief Initializes OpenGL context, shaders, and buffers.
     * @return true if initialization is successful, false otherwise.
     */
    bool init();

    /**
     * @brief Renders a mesh using the specified shader and texture.
     * @param mesh The 3D mesh to be rendered.
     * @param shader The shader program used for rendering.
     * @param texture The texture applied to the mesh.
     */
    void render(const Mesh& mesh, const Shader& shader, const Texture& texture);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

    /**
     * @brief Checks if the rendering window should close.
     * @return true if the window is closing, false otherwise.
     */
    bool shouldClose() const;

    /**
     * @brief Processes user input (keyboard, mouse).
     */
    void processInput();

private:
    /**
     * @brief Callback function for handling window resizing.
     * @param window Pointer to the GLFW window.
     * @param width New width of the window.
     * @param height New height of the window.
     */
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window; ///< Pointer to the GLFW window instance.
    int windowWidth;    ///< Width of the window.
    int windowHeight;   ///< Height of the window.

    // Camera parameters
    glm::vec3 cameraPos;     ///< Camera position in world space.
    glm::vec3 cameraTarget;  ///< Point the camera is looking at.
    float cameraDistance;    ///< Distance of the camera from the target.
    float cameraPitch;       ///< Vertical angle (up/down) of the camera.
    float cameraYaw;         ///< Horizontal angle (left/right) of the camera.

    // Light parameters
    glm::vec3 lightPos; ///< Position of the light source.

    // Model parameters
    glm::vec3 modelRotation; ///< Rotation of the 3D model.
    bool autoRotate;         ///< Flag to enable/disable automatic rotation.
    float rotationSpeed;     ///< Speed of automatic rotation.

    // UI state
    bool showUI; ///< Flag to toggle UI display.

    /**
     * @brief Updates the camera view matrix based on position and angles.
     */
    void updateCamera();

    /**
     * @brief Renders the graphical user interface (UI).
     */
    void renderUI();
};

#endif // RENDERER_H

