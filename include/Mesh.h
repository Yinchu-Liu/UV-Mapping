#ifndef MESH_H
#define MESH_H

#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool loadFromFile(const std::string& filename);
    void bind() const;
    void unbind() const;
    unsigned int getVertexCount() const { return vertexCount; }

private:
    GLuint VAO, VBO, EBO;
    unsigned int vertexCount;
    unsigned int indexCount;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    void setupMesh();
    void cleanup();
};

#endif // MESH_H
