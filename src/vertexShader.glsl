#version 330 core

layout(location = 0) in vec3 position;   // Vertex position
layout(location = 1) in vec3 vNormal;    // Vertex normal

out vec3 fNormal;  // Output to the fragment shader

uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
    fNormal = normalize(vNormal);  // Normalize the normal before passing it to the fragment shader
    gl_Position = projMat * viewMat * vec4(position, 1.0);  // Transform vertex position
}
