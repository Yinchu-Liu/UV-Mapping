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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <chrono>
#include <thread>

Renderer::Renderer(int width, int height)
    : window(nullptr)
    , windowWidth(width)
    , windowHeight(height)
    , cameraDistance(15.0f)  // Increased from 5.0f to handle larger models
    , cameraRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , cameraTarget(0.0f)
    , lightPos(5.0f, 5.0f, 5.0f)  // Increased from 2.0f to provide better lighting for larger models
    , modelRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    , autoRotate(true)
    , rotationSpeed(0.5f)
    , modelScale(0.05f)  // Default scale for the armadillo model
    , enhanceDetails(true)
    , detailStrength(0.7f)
    , rimLightStrength(0.3f)
    , showUI(true)
{
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::init() {
    std::cout << "Initializing renderer..." << std::endl;

    // Initialize the GLFW library
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    std::cout << "Initializing GLFW..." << std::endl;

    // Set OpenGL version to 3.3 and Core Profile (deprecated features are not available)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    std::cout << "Creating window..." << std::endl;

    // Create the GLFW window with the specified width, height, and title ("UV Mapping")
    window = glfwCreateWindow(windowWidth, windowHeight, "UV Mapping", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // This makes the created window the current context for OpenGL operations
    // All OpenGL calls will target this window
    glfwMakeContextCurrent(window);

    // Store the renderer instance ("this" pointer) in GLFW window, allowing thw window to access the renderer instance later
    glfwSetWindowUserPointer(window, this);

    std::cout << "Initializing GLAD..." << std::endl;

    // Initialize GLAD, passing the function pointer retrieval mechanism (glfwGetProcAddress)
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
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize ImGui for use with GLFW and OpenGL
    ImGui_ImplOpenGL3_Init("#version 330"); // Initialize ImGui for OpenGL 3.x with GLSL version 330
    ImGui::StyleColorsDark();

    // Enable depth testing, determining which objects are in front and which are behind
    glEnable(GL_DEPTH_TEST);
    // Set the viewport to the size of the window, so OpenGL knows where to render the scene within the window
    glViewport(0, 0, windowWidth, windowHeight);

    // Set the viewport resize callback, when the user resizes the window
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // Set the scroll callback for zooming
    glfwSetScrollCallback(window, scrollCallback);

    // The buffer swap (to show the rendered image) will occur once per vertical refresh
    glfwSwapInterval(1);

    std::cout << "Renderer initialized successfully" << std::endl;
    return true;
}

void Renderer::updateCamera() {
    glm::vec3 direction = cameraRotation * glm::vec3(0.0f, 0.0f, -1.0f);
    cameraPos = cameraTarget - direction * cameraDistance;
}

void Renderer::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showUI) {
        ImGui::Begin("Controls");
        
        if (ImGui::CollapsingHeader("Camera Controls")) {
            ImGui::SliderFloat("Distance", &cameraDistance, 5.0f, 30.0f);
            if (ImGui::SliderFloat3("Rotation", glm::value_ptr(cameraRotation), -1.0f, 1.0f)) {
                cameraRotation = glm::normalize(cameraRotation);
            }
        }

        if (ImGui::CollapsingHeader("Model Controls")) {
            ImGui::Checkbox("Auto Rotate", &autoRotate);
            ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.1f, 2.0f);
            ImGui::SliderFloat("Model Scale", &modelScale, 0.01f, 1.0f, "%.3f");
            if (!autoRotate) {
                float eulerAngles[3];
                glm::vec3 rotation = glm::eulerAngles(modelRotation);
                eulerAngles[0] = glm::degrees(rotation.x);
                eulerAngles[1] = glm::degrees(rotation.y);
                eulerAngles[2] = glm::degrees(rotation.z);
                
                if (ImGui::SliderFloat3("Rotation", eulerAngles, -180.0f, 180.0f)) {
                    glm::vec3 newRotation = glm::radians(glm::vec3(eulerAngles[0], eulerAngles[1], eulerAngles[2]));
                    modelRotation = glm::quat(newRotation);
                }
            }
        }

        if (ImGui::CollapsingHeader("Light Controls")) {
            ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos), -10.0f, 10.0f);
        }
        
        if (ImGui::CollapsingHeader("Visual Enhancement")) {
            ImGui::Checkbox("Enhance Details", &enhanceDetails);
            ImGui::SliderFloat("Detail Strength", &detailStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Rim Lighting", &rimLightStrength, 0.0f, 1.0f);
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
    // Calculate the delta time (deltaTime), which is the time difference between the current frame and the last frame
    // This is used for frame rate-independent animations and smooth motion
    // lastFrameTime is initialized statically so it retains its value across multiple calls to render()
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
    lastFrameTime = currentFrameTime;

    // Clear the screen with a dark teal color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update camera
    updateCamera();

    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    
    // Scale down large models to ensure they fit in the view
    model = glm::scale(model, glm::vec3(modelScale)); // Scale factor can be adjusted
    
    if (autoRotate) {
        float angle = rotationSpeed * deltaTime * 50.0f;
        modelRotation = glm::normalize(glm::angleAxis(glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f)) * modelRotation);
        model = glm::toMat4(modelRotation) * model; // Apply rotation after scaling
    } else {
        model = glm::toMat4(modelRotation) * model; // Apply rotation after scaling
    }

    // Create view matrix
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));

    // Create projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

    // Set shader uniforms
    shader.use(); // Activate the shader program
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection); // Send model, view, and projection matrices to the shader for transformation
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", cameraPos); // Send lighting and camera position to shader for lighting calculation
    
    // Pass visual enhancement parameters to shader
    shader.setFloat("detailStrength", enhanceDetails ? detailStrength : 0.0f);
    shader.setFloat("rimLightStrength", enhanceDetails ? rimLightStrength : 0.0f);

    // Bind texture and draw mesh
    // Binding doesn't upload data, it selects which already-uploaded data to use
    texture.bind();    // Make this texture active for rendering, bind to GL_TEXTURE0 (default)
    mesh.bind();       // Make this mesh's vertex data active
    glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0);

    // Render UI
    if (showUI) {
        renderUI();
    }

    // Swap buffers and poll events
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Frame rate limiting
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
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
    // Ensure that the rendering is correctly mapped to the entire window area due to resizing
    glViewport(0, 0, width, height);
}

void Renderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Get the renderer instance from the window user pointer
    auto* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        // Adjust camera distance (zoom) based on scroll direction
        // Negative yoffset means scroll up (zoom in)
        // Positive yoffset means scroll down (zoom out)
        renderer->cameraDistance -= yoffset * 1.0f; // Adjust sensitivity as needed
        
        // Clamp camera distance to reasonable values
        if (renderer->cameraDistance < 3.0f) renderer->cameraDistance = 3.0f;
        if (renderer->cameraDistance > 40.0f) renderer->cameraDistance = 40.0f;
    }
}
