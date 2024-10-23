// Mesh.cpp
#include "Mesh.hpp"
#include <cmath>

#include <iostream>


std::shared_ptr<Mesh> Mesh::genSphere(const size_t resolution) {
    auto mesh = std::make_shared<Mesh>();

    const float PI = 3.14159265359f;
    const float radius = 1.0f; // Sfera unitaria

    size_t sectorCount = resolution;
    size_t stackCount = resolution;

    // Step degli angoli
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;

    // Genera i vertici e le normali
    for(size_t i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep; // Da pi/2 a -pi/2
        float xy = radius * cosf(stackAngle);      // r * cos(phi)
        float z = radius * sinf(stackAngle);       // r * sin(phi)
        
        // Compute V texture coordinate (latitudinal angle)
        float v = (float)i / stackCount; 

        for(size_t j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;    // Da 0 a 2pi

            // Posizione del vertice (coordinate cartesiane)
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            mesh->m_vertexPositions.push_back(x);
            mesh->m_vertexPositions.push_back(-z);
            mesh->m_vertexPositions.push_back(y);

            // Normale del vertice
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            mesh->m_vertexNormals.push_back(nx);
            mesh->m_vertexNormals.push_back(-nz);
            mesh->m_vertexNormals.push_back(ny);

            // Compute U texture coordinate (longitudinal angle)
            float u = (float)j / sectorCount; 

            // Add texture coordinates (u, v)
            mesh->m_vertexTexCoords.push_back(u);
            mesh->m_vertexTexCoords.push_back(v);
        }
    }

    // Genera gli indici per i triangoli
    for(size_t i = 0; i < stackCount; ++i) {
        size_t k1 = i * (sectorCount + 1); // indice della riga corrente
        size_t k2 = k1 + sectorCount + 1;  // indice della riga successiva

        for(size_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0) {
                // Primo triangolo
                mesh->m_triangleIndices.push_back(k1);
                mesh->m_triangleIndices.push_back(k2);
                mesh->m_triangleIndices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1)) {
                // Secondo triangolo
                mesh->m_triangleIndices.push_back(k1 + 1);
                mesh->m_triangleIndices.push_back(k2);
                mesh->m_triangleIndices.push_back(k2 + 1);
            }
        }
    }

    return mesh;
}

/* after creating vertices and indices we need to upload them to the GPU, using VBO (holds the vertex data) and EBO (index data). These 
buffers are associated with a VAO (Vertex Array Object), which keeps track of which vertex attributes (positions, normals, 
texture coordinates, etc.) are stored in which buffers. */
void Mesh::init() {
    // Genera i buffer
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_posVbo);
    glGenBuffers(1, &m_normalVbo);
    glGenBuffers(1, &m_ibo);
    glGenBuffers(1, &m_texCoordVbo);

    // Bind del VAO
    glBindVertexArray(m_vao);

    // Posizioni dei vertici
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexPositions.size() * sizeof(float), m_vertexPositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Layout (location = 0)
    glEnableVertexAttribArray(0);

    // Normali
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexNormals.size() * sizeof(float), m_vertexNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // Layout (location = 1)
    glEnableVertexAttribArray(1);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo); // Bind the texture coordinate buffer
    glBufferData(GL_ARRAY_BUFFER, m_vertexTexCoords.size() * sizeof(float), m_vertexTexCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Layout (location = 2)
    glEnableVertexAttribArray(2);

    // Indici
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleIndices.size() * sizeof(unsigned int), m_triangleIndices.data(), GL_STATIC_DRAW);

    // Unbind del VAO
    glBindVertexArray(0);
}

void Mesh::render() {
    glBindVertexArray(m_vao);  // Bind del VAO

    // Disegna la mesh usando gli indici (IBO)
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);  // Unbind del VAO
}

