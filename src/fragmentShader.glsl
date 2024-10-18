#version 330 core
in vec3 vertexColor; // Colore del vertice passato dal vertex shader

out vec4 FragColor;  // Colore finale

void main()
{
    FragColor = vec4(vertexColor, 1.0); // Applichiamo il colore
}
