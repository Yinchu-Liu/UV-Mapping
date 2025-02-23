#ifndef RENDERER_H
#define RENDERER_H

#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <string>

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    bool init();
    void render(const Mesh& mesh, const Shader& shader, const Texture& texture);
    void cleanup();
    bool shouldClose() const;
    void processInput();

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // Camera parameters
    glm::vec3 cameraPos;
    glm::vec3 cameraTarget;
    float cameraDistance;
    float cameraPitch;
    float cameraYaw;
    
    // Light parameters
    glm::vec3 lightPos;
    
    // Model parameters
    glm::vec3 modelRotation;
    bool autoRotate;
    float rotationSpeed;

    // UI state
    bool showUI;
    
    void updateCamera();
    void renderUI();
};

#endif // RENDERER_H
