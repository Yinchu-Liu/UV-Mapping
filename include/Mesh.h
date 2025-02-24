#ifndef MESH_H
#define MESH_H

#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

/**
 * @class Mesh
 * @brief Represents a 3D mesh with vertex, UV, normal, and index data.
 *
 * This class is responsible for loading a mesh from a file, managing
 * OpenGL buffers, and providing rendering-related functions.
 */
class Mesh {
public:
    /**
     * @brief Default constructor for the Mesh class.
     *
     * Initializes buffers and attributes.
     */
    Mesh();

    /**
     * @brief Destructor for the Mesh class.
     *
     * Cleans up allocated OpenGL resources.
     */
    ~Mesh();

    /**
     * @brief Loads mesh data from a file.
     *
     * Parses a 3D model file and extracts vertex positions, UV coordinates,
     * normals, and indices.
     *
     * @param filename Path to the 3D model file.
     * @return True if the mesh is successfully loaded, false otherwise.
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Binds the mesh's Vertex Array Object (VAO) for rendering.
     *
     * This function prepares the mesh for rendering by binding its VAO.
     */
    void bind() const;

    /**
     * @brief Unbinds the currently bound VAO.
     *
     * This function resets the bound VAO to prevent unintended modifications.
     */
    void unbind() const;

    /**
     * @brief Gets the number of vertices in the mesh.
     *
     * @return The number of vertices stored in the mesh.
     */
    unsigned int getVertexCount() const { return vertexCount; }

    /**
     * @brief Gets the number of indices in the mesh.
     * @return The number of indices in the mesh.
     */
    unsigned int getIndexCount() const { return indexCount; }

private:
    GLuint VAO, VBO, EBO;  ///< OpenGL buffer IDs for vertex data and indices
    unsigned int vertexCount;  ///< Number of vertices in the mesh
    unsigned int indexCount;  ///< Number of indices in the mesh (if indexed)

    std::vector<glm::vec3> vertices;  ///< List of vertex positions
    std::vector<glm::vec2> uvs;  ///< List of texture coordinates (UV mapping)
    std::vector<glm::vec3> normals;  ///< List of normal vectors for shading
    std::vector<unsigned int> indices;  ///< List of indices for indexed rendering

    /**
     * @brief Initializes OpenGL buffers and configures vertex attributes.
     *
     * This function creates the VAO, VBO, and EBO, then sets up
     * the vertex attributes needed for rendering.
     */
    void setupMesh();

    /**
     * @brief Releases OpenGL resources used by the mesh.
     *
     * Deletes the VAO, VBO, and EBO to free GPU memory.
     */
    void cleanup();
};

#endif // MESH_H
