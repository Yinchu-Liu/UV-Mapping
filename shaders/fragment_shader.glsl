#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Get texture color and convert to RGB if single channel
    vec4 texColor = texture(texture1, TexCoord);
    vec3 textureRGB = vec3(texColor.r);  // Use red channel for all RGB components

    // Ambient
    float ambientStrength = 0.3;  // Increased ambient light
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular
    float specularStrength = 0.8;  // Increased specular strength
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);  // Reduced shininess
    vec3 specular = specularStrength * spec * vec3(1.0);

    // Combine lighting with texture
    vec3 result = (ambient + diffuse + specular) * textureRGB;
    FragColor = vec4(result, 1.0);
}
