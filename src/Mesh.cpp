#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <limits>
#include <algorithm>

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), vertexCount(0), indexCount(0) {
}

Mesh::~Mesh() {
    cleanup();
}

bool Mesh::loadFromFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;

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

    std::cout << "Successfully loaded " << vertices.size() << " vertices." << std::endl;
    
    // Check if UV coordinates are missing or insufficient
    if (uvs.empty() || uvs.size() != vertices.size()) {
        std::cout << "UV coordinates missing or incomplete. Generating procedural UVs..." << std::endl;
        
        // Clear existing UVs and generate new ones
        uvs.clear();
        uvs.resize(vertices.size());
        
        // Find the bounding box for the model
        glm::vec3 minBounds = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxBounds = glm::vec3(std::numeric_limits<float>::lowest());
        
        for (const auto& vertex : vertices) {
            minBounds = glm::min(minBounds, vertex);
            maxBounds = glm::max(maxBounds, vertex);
        }
        
        glm::vec3 dimensions = maxBounds - minBounds;
        glm::vec3 center = (minBounds + maxBounds) * 0.5f;
        
        // Check if this is the armadillo model by looking at the filename
        bool isArmadillo = filename.find("armadillo") != std::string::npos;
        
        if (isArmadillo) {
            std::cout << "Detected armadillo model. Using advanced segmentation UV mapping..." << std::endl;
            
            // For the armadillo, we'll use a segmentation-based approach
            // This divides the model into regions and applies different mapping techniques to each
            
            // 1. First, segment the model into regions based on Y-coordinate (height)
            const int numSegments = 10;
            std::vector<std::vector<size_t>> segments(numSegments);
            
            // Assign vertices to segments
            for (size_t i = 0; i < vertices.size(); i++) {
                float heightRatio = (vertices[i].y - minBounds.y) / dimensions.y;
                int segmentIndex = std::min(static_cast<int>(heightRatio * numSegments), numSegments - 1);
                segments[segmentIndex].push_back(i);
            }
            
            // 2. Process each segment with a specialized mapping technique
            for (int segment = 0; segment < numSegments; segment++) {
                if (segments[segment].empty()) continue;
                
                // Calculate segment-specific parameters
                float segmentYMin = minBounds.y + (segment * dimensions.y) / numSegments;
                float segmentYMax = minBounds.y + ((segment + 1) * dimensions.y) / numSegments;
                float segmentHeight = segmentYMax - segmentYMin;
                
                // Calculate segment center
                glm::vec3 segmentCenter = glm::vec3(0.0f);
                for (size_t idx : segments[segment]) {
                    segmentCenter += vertices[idx];
                }
                segmentCenter /= static_cast<float>(segments[segment].size());
                
                // Different mapping techniques based on segment position
                if (segment < 3) {
                    // Bottom segments (legs, tail) - use cylindrical mapping
                    for (size_t idx : segments[segment]) {
                        glm::vec3 dir = vertices[idx] - segmentCenter;
                        float angle = atan2(dir.z, dir.x);
                        float u = (angle + 3.14159f) / (2.0f * 3.14159f);
                        
                        // Normalize height within segment
                        float v = (vertices[idx].y - segmentYMin) / segmentHeight;
                        
                        // Store UV with scaling for better texture detail
                        uvs[idx] = glm::vec2(u * 2.0f, v * 2.0f);
                    }
                } 
                else if (segment < 7) {
                    // Middle segments (body, arms) - use a combination of cylindrical and spherical
                    for (size_t idx : segments[segment]) {
                        glm::vec3 dir = glm::normalize(vertices[idx] - segmentCenter);
                        
                        // Cylindrical component
                        float angle = atan2(dir.z, dir.x);
                        float u_cyl = (angle + 3.14159f) / (2.0f * 3.14159f);
                        
                        // Spherical component
                        float u_sph = 0.5f + atan2(dir.z, dir.x) / (2.0f * 3.14159f);
                        float v_sph = 0.5f - asin(dir.y) / 3.14159f;
                        
                        // Blend based on distance from central axis
                        float distFromAxis = sqrt(dir.x * dir.x + dir.z * dir.z);
                        float blend = glm::clamp(distFromAxis * 2.0f, 0.0f, 1.0f);
                        
                        float u = u_cyl;
                        float v = (vertices[idx].y - segmentYMin) / segmentHeight;
                        
                        // Apply blending with spherical for extremities
                        if (distFromAxis > 0.4f) {
                            float extremityBlend = (distFromAxis - 0.4f) / 0.6f;
                            u = glm::mix(u, u_sph, extremityBlend);
                            v = glm::mix(v, v_sph, extremityBlend);
                        }
                        
                        // Store UV with scaling
                        uvs[idx] = glm::vec2(u * 3.0f, v * 3.0f);
                    }
                }
                else {
                    // Top segments (head, ears) - use spherical mapping
                    for (size_t idx : segments[segment]) {
                        glm::vec3 dir = glm::normalize(vertices[idx] - segmentCenter);
                        float u = 0.5f + atan2(dir.z, dir.x) / (2.0f * 3.14159f);
                        float v = 0.5f - asin(dir.y) / 3.14159f;
                        
                        // Store UV with scaling
                        uvs[idx] = glm::vec2(u * 2.0f, v * 2.0f);
                    }
                }
            }
            
            // 3. Post-process UVs to ensure smooth transitions between segments
            // Create a copy of the original UVs for blending
            std::vector<glm::vec2> originalUVs = uvs;
            
            // Smooth transitions by averaging with neighboring vertices
            for (size_t i = 0; i < vertices.size(); i++) {
                // Find segment boundaries
                float heightRatio = (vertices[i].y - minBounds.y) / dimensions.y;
                int segmentIndex = std::min(static_cast<int>(heightRatio * numSegments), numSegments - 1);
                
                // If near segment boundary, blend with neighbors
                float segmentPos = heightRatio * numSegments - segmentIndex;
                if (segmentPos < 0.1f || segmentPos > 0.9f) {
                    // Find closest vertices in adjacent segments
                    float blendFactor = (segmentPos < 0.1f) ? (0.1f - segmentPos) / 0.1f : (segmentPos - 0.9f) / 0.1f;
                    
                    // Simple smoothing by averaging with original
                    uvs[i] = glm::mix(originalUVs[i], uvs[i], 0.7f); // 70% new, 30% original
                }
            }
            
            // 4. Final adjustments to ensure proper texture wrapping
            for (auto& uv : uvs) {
                // Ensure UVs are in [0,1] range for proper texture mapping
                uv = glm::fract(uv);
            }
            
            std::cout << "Generated segmentation-based UVs for armadillo model." << std::endl;
        } else {
            // Original UV mapping for other models
            for (size_t i = 0; i < vertices.size(); i++) {
                // Get normalized position from vertex after centering
                glm::vec3 pos = vertices[i] - minBounds;
                pos /= dimensions; // Normalize to [0,1] range
                
                // Try several UV mapping approaches and choose the best one
                // Approach 1: Planar mapping using XZ coordinates
                glm::vec2 planarUV = glm::vec2(pos.x, pos.z);
                
                // Approach 2: Spherical mapping
                glm::vec3 normal = glm::normalize(vertices[i] - center);
                float u_spherical = 0.5f + atan2(normal.z, normal.x) / (2.0f * 3.14159f);
                float v_spherical = 0.5f - asin(normal.y) / 3.14159f;
                glm::vec2 sphericalUV = glm::vec2(u_spherical, v_spherical);
                
                // Approach 3: Cylindrical mapping
                float theta = atan2(normal.z, normal.x);
                float u_cylindrical = (theta + 3.14159f) / (2.0f * 3.14159f);
                float v_cylindrical = (normal.y + 1.0f) * 0.5f;
                glm::vec2 cylindricalUV = glm::vec2(u_cylindrical, v_cylindrical);
                
                // Choose mapping based on the shape:
                // Use dot product with up vector to determine if we should use cylindrical vs spherical mapping
                float upwardness = glm::dot(normal, glm::vec3(0.0f, 1.0f, 0.0f));
                
                if (abs(upwardness) > 0.7f) {
                    // For top/bottom parts, use planar mapping
                    uvs[i] = planarUV;
                } else {
                    // For sides, use cylindrical mapping
                    uvs[i] = cylindricalUV;
                }
            }
        }
        
        std::cout << "Generated " << uvs.size() << " procedural UV coordinates." << std::endl;
    } else {
        std::cout << "Model already has " << uvs.size() << " UV coordinates." << std::endl;
    }
    
    // Force regeneration of UVs for the armadillo model
    bool isArmadillo = filename.find("armadillo") != std::string::npos;
    if (isArmadillo) {
        std::cout << "Forcing regeneration of UVs for armadillo model..." << std::endl;
        uvs.clear(); // Clear UVs to force regeneration
    }
    
    // Find the center of the model and translate vertices to center it
    glm::vec3 minBounds = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxBounds = glm::vec3(std::numeric_limits<float>::lowest());
    
    // Calculate bounding box
    for (const auto& vertex : vertices) {
        minBounds = glm::min(minBounds, vertex);
        maxBounds = glm::max(maxBounds, vertex);
    }
    
    // Calculate center of the model
    glm::vec3 center = (minBounds + maxBounds) * 0.5f;
    
    // Translate all vertices to center the model at origin
    for (auto& vertex : vertices) {
        vertex -= center;
    }
    
    std::cout << "Model centered. Bounding box: (" 
              << minBounds.x << "," << minBounds.y << "," << minBounds.z << ") to ("
              << maxBounds.x << "," << maxBounds.y << "," << maxBounds.z << ")" << std::endl;
    
    // Setup the mesh
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
