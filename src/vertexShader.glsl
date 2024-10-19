#version 330 core

layout(location = 0) in vec3 aPosition; // posizione del vertice
layout(location = 1) in vec3 aNormal;   // normale del vertice

uniform mat4 model;     // Matrice di modello
uniform mat4 viewMat;   // Matrice di vista
uniform mat4 projMat;   // Matrice di proiezione

out vec3 fNormal;      // normale interpolata
out vec3 fPosition;    // posizione interpolata

void main() {
    // Posizione finale del vertice trasformata correttamente
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    fPosition = vec3(worldPosition);  // Salva la posizione nello spazio mondo

    // Calcola la normale nello spazio mondo
    fNormal = mat3(transpose(inverse(model))) * aNormal;

    // Trasforma la posizione finale del vertice nello spazio clip (proiezione + vista + modello)
    gl_Position = projMat * viewMat * worldPosition;
}

