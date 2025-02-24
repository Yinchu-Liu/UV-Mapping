#version 330 core
out vec4 FragColor; // Output color of the fragment

// Inputs from vertex shader
in vec2 TexCoord;   // Texture coordinates
in vec3 Normal;     // Normal vector in world space
in vec3 FragPos;    // Fragment position in world space

// Uniform variables
uniform sampler2D texture1; // Texture sampler
uniform vec3 lightPos;      // Light position in world space
uniform vec3 viewPos;       // Camera position in world space

void main() {
    // Get texture color and convert to RGB if single channel
    vec4 texColor = texture(texture1, TexCoord);
    vec3 textureRGB = vec3(texColor.r);  // Use red channel for all RGB components

    // Ambient lighting
    float ambientStrength = 0.3;  // Increased ambient light
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse lighting
    vec3 norm = normalize(Normal); // Normalize the normal vector
    vec3 lightDir = normalize(lightPos - FragPos); // Light direction
    float diff = max(dot(norm, lightDir), 0.0); // Diffuse intensity
    vec3 diffuse = diff * vec3(1.0);

    // Specular lighting
    float specularStrength = 0.8;  // Increased specular strength
    vec3 viewDir = normalize(viewPos - FragPos); // View direction
    vec3 reflectDir = reflect(-lightDir, norm); // Reflection direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0); // Specular intensity
    vec3 specular = specularStrength * spec * vec3(1.0);

    // Combine lighting with texture
    vec3 result = (ambient + diffuse + specular) * textureRGB;
    FragColor = vec4(result, 1.0); // Set the final fragment color
}
