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
    // Get texture color with all color channels
    vec4 texColor = texture(texture1, TexCoord);
    vec3 textureRGB = texColor.rgb;  // Use all RGB channels
    
    // Add detail enhancement based on position for models with simple textures
    // This creates a more interesting surface appearance
    vec3 enhancedColor = textureRGB;
    
    // Calculate a detail pattern based on world position
    // This adds some variation to flat textures
    float detailNoise = sin(FragPos.x * 10.0) * sin(FragPos.y * 10.0) * sin(FragPos.z * 10.0) * 0.05;
    enhancedColor = mix(enhancedColor, enhancedColor * (1.0 + detailNoise), 0.7);
    
    // Normal mapping simulation - perturb normal slightly based on texture coordinates
    // This simulates the effect of a normal map for models without one
    vec3 perturbedNormal = normalize(Normal + vec3(
        sin(TexCoord.x * 50.0) * 0.03,
        0.0,
        sin(TexCoord.y * 50.0) * 0.03
    ));
    
    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0);

    // Diffuse lighting
    vec3 norm = normalize(perturbedNormal); // Use perturbed normal for more detail
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Specular lighting
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Higher shininess value
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // Add rim lighting for silhouette enhancement
    float rimStrength = 0.3;
    float rimFactor = 1.0 - max(dot(viewDir, norm), 0.0);
    rimFactor = smoothstep(0.5, 1.0, rimFactor);
    vec3 rim = rimStrength * rimFactor * vec3(0.8, 0.8, 1.0); // Slight blue tint for rim
    
    // Combine all lighting components with enhanced color
    vec3 result = (ambient + diffuse + specular + rim) * enhancedColor;
    
    // Add subtle color variation based on position
    result = mix(result, result * vec3(1.0, 0.95, 0.9), 0.2); // Slight warm tint variation
    
    // Final output with gamma correction
    result = pow(result, vec3(1.0/2.2)); // Gamma correction for better visual fidelity
    FragColor = vec4(result, 1.0);
}
