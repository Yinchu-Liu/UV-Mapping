#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), vertexCount(0), indexCount(0) {
}

Mesh::~Mesh() {
    cleanup();
}

bool Mesh::loadFromFile(const std::string& filename) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::cout << "Loading mesh from file: " << filename << std::endl;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (type == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "f") {
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;

            std::vector<std::string> vertices = {vertex1, vertex2, vertex3};
            for (const auto& vertex : vertices) {
                std::istringstream viss(vertex);
                std::string index_str;
                std::vector<unsigned int> face_indices;

                while (std::getline(viss, index_str, '/')) {
                    if (!index_str.empty()) {
                        face_indices.push_back(std::stoi(index_str));
                    }
                    else {
                        face_indices.push_back(0);
                    }
                }

                // OBJ indices are 1-based, convert to 0-based
                vertexIndices.push_back(face_indices[0] - 1);
                if (face_indices.size() > 1 && face_indices[1] != 0) {
                    uvIndices.push_back(face_indices[1] - 1);
                }
                if (face_indices.size() > 2 && face_indices[2] != 0) {
                    normalIndices.push_back(face_indices[2] - 1);
                }
            }
        }
    }

    // Process the data
    for (size_t i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        vertices.push_back(temp_vertices[vertexIndex]);

        if (!uvIndices.empty()) {
            unsigned int uvIndex = uvIndices[i];
            uvs.push_back(temp_uvs[uvIndex]);
        }
        else {
            uvs.push_back(glm::vec2(0.0f, 0.0f));
        }

        if (!normalIndices.empty()) {
            unsigned int normalIndex = normalIndices[i];
            normals.push_back(temp_normals[normalIndex]);
        }
        else {
            normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        }

        indices.push_back(i);
    }

    vertexCount = vertices.size();
    indexCount = indices.size();

    std::cout << "Mesh loaded successfully. Vertices: " << vertexCount << ", Indices: " << indexCount << std::endl;

    setupMesh();
    return true;
}

void Mesh::setupMesh() {
    // Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Create and fill vertex buffer
    std::vector<float> vertexData;
    for (size_t i = 0; i < vertices.size(); i++) {
        // Position
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);
        // UV
        vertexData.push_back(uvs[i].x);
        vertexData.push_back(uvs[i].y);
        // Normal
        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // UV attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Normal attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    // Element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::bind() const {
    glBindVertexArray(VAO);
}

void Mesh::unbind() const {
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}
