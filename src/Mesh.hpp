#pragma once
#include <vector>
#include <memory>
#include <glad/gl.h>
#include <glm/glm.hpp>

class Mesh {
public:
    void init(); // Set up the geometry buffer
    void render(); // Call in the main rendering loop
    static std::shared_ptr<Mesh> genSphere(const size_t resolution = 16); // Generate unit sphere
    // we need to implement genSphere to generate the sphere.

private:
    std::vector<float> m_vertexPositions;
    std::vector<float> m_vertexNormals;
    std::vector<unsigned int> m_triangleIndices;

    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_ibo = 0;
};