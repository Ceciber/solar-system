#version 330 core
layout(location = 0) in vec3 aPos;  // posizione del vertice
layout(location = 1) in vec3 vNormal; // colore del vertice

out vec3 fNormal; // Passiamo il colore al framment shader

uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{
    gl_Position = projMat * viewMat * vec4(aPos, 1.0); // Trasformiamo il vertice
    fNormal = vNormal;
}
