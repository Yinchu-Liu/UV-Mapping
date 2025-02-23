#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(int width, int height)
    : window(nullptr)
    , windowWidth(width)
    , windowHeight(height)
    , cameraDistance(5.0f)
    , cameraPitch(30.0f)
    , cameraYaw(45.0f)
    , cameraTarget(0.0f)
    , lightPos(2.0f, 2.0f, 2.0f)
    , modelRotation(0.0f)
    , autoRotate(true)
    , rotationSpeed(0.5f)
    , showUI(true)
{
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init() {
    std::cout << "Initializing renderer..." << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    std::cout << "Initializing GLFW..." << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::cout << "Creating window..." << std::endl;

    window = glfwCreateWindow(windowWidth, windowHeight, "UV Mapping", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Store the renderer instance in GLFW window
    glfwSetWindowUserPointer(window, this);

    std::cout << "Initializing GLAD..." << std::endl;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Print OpenGL info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, windowWidth, windowHeight);

    // Set the viewport resize callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSwapInterval(1);

    std::cout << "Renderer initialized successfully" << std::endl;
    return true;
}

void Renderer::updateCamera() {
    // Convert spherical coordinates to Cartesian
    float x = cameraDistance * cos(glm::radians(cameraPitch)) * cos(glm::radians(cameraYaw));
    float y = cameraDistance * sin(glm::radians(cameraPitch));
    float z = cameraDistance * cos(glm::radians(cameraPitch)) * sin(glm::radians(cameraYaw));
    
    cameraPos = glm::vec3(x, y, z);
}

void Renderer::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showUI) {
        ImGui::Begin("Controls");
        
        if (ImGui::CollapsingHeader("Camera Controls")) {
            ImGui::SliderFloat("Distance", &cameraDistance, 2.0f, 10.0f);
            ImGui::SliderFloat("Pitch", &cameraPitch, -89.0f, 89.0f);
            ImGui::SliderFloat("Yaw", &cameraYaw, 0.0f, 360.0f);
        }

        if (ImGui::CollapsingHeader("Model Controls")) {
            ImGui::Checkbox("Auto Rotate", &autoRotate);
            ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.1f, 2.0f);
            if (!autoRotate) {
                ImGui::SliderFloat3("Rotation", glm::value_ptr(modelRotation), 0.0f, 360.0f);
            }
        }

        if (ImGui::CollapsingHeader("Light Controls")) {
            ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos), -5.0f, 5.0f);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        showUI = !showUI;
    }
}

void Renderer::render(const Mesh& mesh, const Shader& shader, const Texture& texture) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    try {
        shader.use();
        texture.bind(0);
        mesh.bind();

        // Update camera position
        updateCamera();

        // Create transformation matrices
        glm::mat4 model = glm::mat4(1.0f);
        
        if (autoRotate) {
            float angle = rotationSpeed * (float)glfwGetTime();
            model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));
        } else {
            model = glm::rotate(model, glm::radians(modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(modelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        glm::mat4 view = glm::lookAt(
            cameraPos,
            cameraTarget,
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)windowWidth / (float)windowHeight,
            0.1f,
            100.0f
        );

        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", cameraPos);

        glDrawElements(GL_TRIANGLES, mesh.getVertexCount(), GL_UNSIGNED_INT, 0);

        // Render UI
        renderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } catch (const std::exception& e) {
        std::cerr << "Error during rendering: " << e.what() << std::endl;
    }
}

void Renderer::cleanup() {
    if (window) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

bool Renderer::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Renderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Get the renderer instance from GLFW
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->windowWidth = width;
        renderer->windowHeight = height;
    }
    glViewport(0, 0, width, height);
}
