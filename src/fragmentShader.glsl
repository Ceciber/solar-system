#version 330 core

in vec3 fNormal;  // Interpolated normal from the vertex shader
out vec4 finalColor;  // Final output color

void main() {
    finalColor = vec4(normalize(fNormal), 1.0);  // Normalize and display the normal as color
}