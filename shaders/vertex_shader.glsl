#version 330 core
// Vertex attribute locations
layout (location = 0) in vec3 aPos;      // Vertex position in object space
layout (location = 1) in vec2 aTexCoord; // Texture coordinates
layout (location = 2) in vec3 aNormal;   // Vertex normal in object space

// Outputs to fragment shader
out vec2 TexCoord;  // Texture coordinates
out vec3 Normal;     // Vertex normal in world space
out vec3 FragPos;    // Fragment position in world space

// Transformation matrices
uniform mat4 model;      // Model matrix (object to world space)
uniform mat4 view;       // View matrix (world to camera space)
uniform mat4 projection; // Projection matrix (camera to clip space)

void main() {
    // Transform vertex position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform normal to world space using normal matrix
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass through texture coordinates
    TexCoord = aTexCoord;

    // Transform vertex position to clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
