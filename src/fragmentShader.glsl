#version 330 core

in vec3 fPosition;  // Fragment position in world space
in vec3 fNormal;    // Interpolated normal from vertex shader
in vec2 fTexCoord;

uniform vec3 lightPos;    // Light (Sun) position
uniform vec3 viewPos;     // Camera position
uniform vec3 lightColor;  // Light (Sun) color
uniform vec3 ambientColor; // Ambient light color
uniform vec3 objectColor; // Object's base color
uniform float shininess;  // Material shininess
uniform int isSun;        // Flag to differentiate Sun from other objects

struct Material {
    sampler2D albedoTex;
};

uniform Material material;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(lightPos - fPosition);
    vec3 viewDir = normalize(viewPos - fPosition);

    vec3 ambient = ambientColor; 

    vec3 texColor = texture(material.albedoTex, fTexCoord).rgb; 

    // If the object is the Sun, use only its diffuse light
    if (isSun == 1) {
        FragColor = vec4(objectColor, 1.0);  // Just render Sun's base color
        return;
    }

    // --- Diffuse lighting ---
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // --- Specular lighting ---
    vec3 reflectDir = reflect(-lightDir, norm);  // Reflected light direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);  // Specular strength
    vec3 specular = spec * lightColor;

    // Final color combination
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(texColor * result, 1.0);

}

