// Mesh.cpp
#include "Mesh.hpp"
#include <cmath>

std::shared_ptr<Mesh> Mesh::genSphere(const size_t resolution) {
    auto mesh = std::make_shared<Mesh>();
    
    const float PI = 3.14159265359f;
    const float stepTheta = PI / resolution;  // Step for latitude
    const float stepPhi = 2.0f * PI / resolution;  // Step for longitude

    // Generate vertices and normals. We generate vertices using spherical coordinates
    for (size_t i = 0; i <= resolution; ++i) {
        float theta = i * stepTheta; // from 0 to PI (latitude)

        for (size_t j = 0; j <= resolution; ++j) {
            float phi = j * stepPhi; // from 0 to 2*PI (longitude)

            // Spherical to Cartesian conversion (we consider radius = 1)
            float x = sin(theta) * cos(phi);
            float y = cos(theta);
            float z = sin(theta) * sin(phi);

            // Push the vertex position (unit sphere, so radius is 1)
            mesh->m_vertexPositions.push_back(x);
            mesh->m_vertexPositions.push_back(y);
            mesh->m_vertexPositions.push_back(z);

            // Normals for a unit sphere are the same as the positions
            mesh->m_vertexNormals.push_back(x);
            mesh->m_vertexNormals.push_back(y);
            mesh->m_vertexNormals.push_back(z);
        }
    }

    // Generate indices for how the vertices are connected to form triangles.
    for (size_t i = 0; i < resolution; ++i) {
        for (size_t j = 0; j < resolution; ++j) {
            unsigned int first = i * (resolution + 1) + j;
            unsigned int second = first + resolution + 1;

            // Two triangles per quad
            mesh->m_triangleIndices.push_back(first);
            mesh->m_triangleIndices.push_back(second);
            mesh->m_triangleIndices.push_back(first + 1);

            mesh->m_triangleIndices.push_back(second);
            mesh->m_triangleIndices.push_back(second + 1);
            mesh->m_triangleIndices.push_back(first + 1);
        }
    }

    return mesh;
}

/* after creating vertices and indices we need to upload them to the GPU, using VBO (holds the vertex data) and EBO (index data). These 
buffers are associated with a VAO (Vertex Array Object), which keeps track of which vertex attributes (positions, normals, 
texture coordinates, etc.) are stored in which buffers. */
void Mesh::init() {
    // Create and bind a VAO (Vertex Array Object)
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // Vertex positions (VBO)
    glGenBuffers(1, &m_posVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_posVbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexPositions.size() * sizeof(float), m_vertexPositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Vertex normals (VBO)
    glGenBuffers(1, &m_normalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalVbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexNormals.size() * sizeof(float), m_vertexNormals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Index buffer (IBO)
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleIndices.size() * sizeof(unsigned int), m_triangleIndices.data(), GL_STATIC_DRAW);


    glBindVertexArray(0); // Unbind the VAO
}

void Mesh::render() {
    glBindVertexArray(m_vao); // Bind VAO that stores the buffers
    glDrawElements(GL_TRIANGLES, m_triangleIndices.size(), GL_UNSIGNED_INT, nullptr); // Draw the sphere
    glBindVertexArray(0); // Unbind VAO after rendering
}

