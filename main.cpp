#include "Renderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>

int main() {
    try {
        Renderer renderer(800, 600);
        if (!renderer.init()) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return -1;
        }

        std::cout << "Loading mesh..." << std::endl;
        Mesh mesh;
        if (!mesh.loadFromFile("assets/models/cube.obj")) {
            std::cerr << "Failed to load mesh" << std::endl;
            return -1;
        }

        std::cout << "Loading texture..." << std::endl;
        Texture texture;
        if (!texture.loadFromFile("assets/textures/texture.png")) {
            std::cerr << "Failed to load texture" << std::endl;
            return -1;
        }

        std::cout << "Loading shaders..." << std::endl;
        Shader shader;
        if (!shader.loadFromFiles("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl")) {
            std::cerr << "Failed to load shaders" << std::endl;
            return -1;
        }

        std::cout << "Entering main render loop..." << std::endl;
        while (!renderer.shouldClose()) {
            renderer.processInput();
            renderer.render(mesh, shader, texture);
        }

        std::cout << "Cleaning up..." << std::endl;
        renderer.cleanup();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}
