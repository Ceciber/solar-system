#pragma once
#include <vector>
#include <memory>
#include <glad/gl.h>
#include <glm/glm.hpp>

class Mesh {
public:
    void init(); 
    void render(); 
    static std::shared_ptr<Mesh> genSphere(const size_t resolution); 
    
private:
    std::vector<float> m_vertexPositions;
    std::vector<float> m_vertexNormals;
    std::vector<unsigned int> m_triangleIndices;
    std::vector<float> m_vertexTexCoords;

    GLuint m_vao = 0;
    GLuint m_posVbo = 0;
    GLuint m_normalVbo = 0;
    GLuint m_ibo = 0;
    GLuint m_texCoordVbo = 0;
};