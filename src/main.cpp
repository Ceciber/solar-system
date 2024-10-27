// ----------------------------------------------------------------------------
// main.cpp
//
//  Created on: 24 Jul 2020
//      Author: Kiwon Um
//        Mail: kiwon.um@telecom-paris.fr
//
// Description: IGR201 Practical; OpenGL and Shaders (DO NOT distribute!)
//
// Copyright 2020-2024 Kiwon Um
//
// The copyright to the computer program(s) herein is the property of Kiwon Um,
// Telecom Paris, France. The program(s) may be used and/or copied only with
// the written permission of Kiwon Um or in accordance with the terms and
// conditions stipulated in the agreement/contract under which the program(s)
// have been supplied.
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Mesh.hpp"

// constants
const static float kSizeSun = 1;
const static float kSizeEarth = 0.5;
const static float kSizeMoon = 0.25;
const static float kRadOrbitEarth = 10;
const static float kRadOrbitMoon = 2;
// add two planets for final step
const static float kSizeMars = 0.25; //about half the earth size
const static float kSizeVenus = 0.49; //slightly smaller than the earth
const static float kRadOrbitMars = 15; //about 1.5 earth orbit
const static float kRadOrbitVenus = 7.2; //abot 7.2 earth orbit

// Window parameters
GLFWwindow *g_window = nullptr;

// GPU objects
GLuint g_program = 0; // A GPU program contains at least a vertex shader and a fragment shader

// OpenGL identifiers
GLuint g_vao = 0;
GLuint g_posVbo = 0;
GLuint g_ibo = 0;
GLuint g_colorVbo = 0;

GLuint g_earthTexID;
GLuint g_moonTexID;
// add two planets for final step
GLuint g_marsTexID;
GLuint g_venusTexID;

// All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
std::vector<float> g_vertexPositions;
// All triangle indices packed in one array [v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle
std::vector<unsigned int> g_triangleIndices;
// create a new vector for the colors
std::vector<float> g_vertexColors;

// we need to create a vector for the mesh
std::shared_ptr<Mesh> sphereMesh;

//variables for the planets matrices
glm::mat4 modelMatrixSun = glm::mat4(1.0f);
glm::mat4 modelMatrixEarth = glm::mat4(1.0f);
glm::mat4 modelMatrixMoon = glm::mat4(1.0f);
// add two planets for final step
glm::mat4 modelMatrixMars = glm::mat4(1.0f);
glm::mat4 modelMatrixVenus = glm::mat4(1.0f);

// Basic camera model
class Camera {
public:
  inline float getFov() const { return m_fov; }
  inline void setFoV(const float f) { m_fov = f; }
  inline float getAspectRatio() const { return m_aspectRatio; }
  inline void setAspectRatio(const float a) { m_aspectRatio = a; }
  inline float getNear() const { return m_near; }
  inline void setNear(const float n) { m_near = n; }
  inline float getFar() const { return m_far; }
  inline void setFar(const float n) { m_far = n; }
  inline void setPosition(const glm::vec3 &p) { m_pos = p; }
  inline glm::vec3 getPosition() { return m_pos; }

  inline glm::mat4 computeViewMatrix() const {
    return glm::lookAt(m_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  }

  // Returns the projection matrix stemming from the camera intrinsic parameter.
  inline glm::mat4 computeProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
  }

private:
  glm::vec3 m_pos = glm::vec3(0, 0, 0);
  float m_fov = 45.f;        // Field of view, in degrees
  float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
  float m_near = 0.1f; // Distance before which geometry is excluded from the rasterization process
  float m_far = 10.f; // Distance after which the geometry is excluded from the rasterization process
};
Camera g_camera;

GLuint loadTextureFromFileToGPU(const std::string &filename) {
  // flip the axis or we see the planets upside down
  stbi_set_flip_vertically_on_load(true);
  // Loading the image in CPU memory using stb_image
  int width, height, numComponents;

  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &numComponents, 0);
  GLuint texID; // OpenGL texture identifier
  glGenTextures(1, &texID); // generate an OpenGL texture container
  glBindTexture(GL_TEXTURE_2D, texID); // activate the texture
  // Setup the texture filtering option and repeat mode; check www.opengl.org for details.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Fill the GPU texture with the data stored in the CPU image
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  // Free useless CPU memory
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
  
  return texID;
}

void checkOpenGLError(const std::string& message) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error (" << message << "): " << err << std::endl;
    }
}

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window.
void windowSizeCallback(GLFWwindow* window, int width, int height) {
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));
  glViewport(0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_W) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else if (action == GLFW_PRESS && key == GLFW_KEY_F) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, true); // Closes the application if the escape key is pressed
    } else if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_UP) {
        const glm::vec3 camPosition = g_camera.getPosition();
        g_camera.setPosition(glm::vec3(camPosition[0], camPosition[1], camPosition[2] - 0.1));
    } else if ((action == GLFW_PRESS || action == GLFW_REPEAT) && key == GLFW_KEY_DOWN) {
        const glm::vec3 camPosition = g_camera.getPosition();
        g_camera.setPosition(glm::vec3(camPosition[0], camPosition[1], camPosition[2] + 0.1));
    }
}

void errorCallback(int error, const char *desc) {
  std::cout <<  "Error " << error << ": " << desc << std::endl;
}

void initGLFW() {
  glfwSetErrorCallback(errorCallback);

  // Initialize GLFW, the library responsible for window management
  if(!glfwInit()) {
    std::cerr << "ERROR: Failed to init GLFW" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Before creating the window, set some option flags. We need to indicate what version of openGL we're using
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  // Create the window
  g_window = glfwCreateWindow(
    1024, 768,
    "Interactive 3D Applications (OpenGL) - Simple Solar System",
    nullptr, nullptr);
  if(!g_window) {
    std::cerr << "ERROR: Failed to open window" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  // Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
  glfwMakeContextCurrent(g_window);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetKeyCallback(g_window, keyCallback);
}

void initOpenGL() {

  if(!gladLoadGL(glfwGetProcAddress)) {
    std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
    glfwTerminate();
    std::exit(EXIT_FAILURE);
  }

  glCullFace(GL_BACK); // Specifies the faces to cull (here the ones pointing away from the camera)
  glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
  glDepthFunc(GL_LESS);   // Specify the depth test for the z-buffer
  glEnable(GL_DEPTH_TEST);      // Enable the z-buffer test in the rasterization
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String(const std::string &filename) {
    std::ifstream t(filename.c_str());
    if (!t.is_open()) {
        std::cerr << "ERROR: Could not open file " << filename << std::endl;
        return "";  // Return an empty string if the file could not be opened
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

// Loads and compile a shader, before attaching it to a program
void loadShader(GLuint program, GLenum type, const std::string &shaderFilename) {
    GLuint shader = glCreateShader(type); // Create the shader
    std::string shaderSourceString = file2String(shaderFilename); // Load shader source
    
    if (shaderSourceString.empty()) {
        std::cerr << "ERROR: Shader source for " << shaderFilename << " is empty." << std::endl;
        return; // Return early if shader source is empty
    }

    const GLchar *shaderSource = (const GLchar *)shaderSourceString.c_str(); // C pointer to the source

    glShaderSource(shader, 1, &shaderSource, NULL); // Load the shader code
    glCompileShader(shader);
    
    // Check if shader compilation was successful
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR in compiling " << shaderFilename << "\n\t" << infoLog << std::endl;
    }

    checkOpenGLError("Shader attach");

    glAttachShader(program, shader);
    glDeleteShader(shader);
}


void initGPUprogram() {
  g_program = glCreateProgram(); // Create a GPU program, i.e., two central shaders of the graphics pipeline
  loadShader(g_program, GL_VERTEX_SHADER, "D:/IGR/TP00-OpenGL/src/vertexShader.glsl");
  loadShader(g_program, GL_FRAGMENT_SHADER, "D:/IGR/TP00-OpenGL/src/fragmentShader.glsl");
  glLinkProgram(g_program); // The main GPU program is ready to be handle streams of polygons

  // add this code to verify the correct binding
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(g_program, GL_LINK_STATUS, &success);
  if(!success) {
      glGetProgramInfoLog(g_program, 512, NULL, infoLog);
      std::cerr << "ERROR: Shader program linking failed\n" << infoLog << std::endl;
  }
  checkOpenGLError("Program linking"); // Error check after linking

  glUseProgram(g_program);
  checkOpenGLError("Program use"); // Error check after linking

  g_earthTexID = loadTextureFromFileToGPU("D:\\IGR\\TP00-OpenGL\\src\\media\\earth.jpg");
  g_moonTexID = loadTextureFromFileToGPU("D:\\IGR\\TP00-OpenGL\\src\\media\\moon.jpg");
  // add mars and venus textures
  g_marsTexID = loadTextureFromFileToGPU("D:\\IGR\\TP00-OpenGL\\src\\media\\mars.jpg");
  g_venusTexID = loadTextureFromFileToGPU("D:\\IGR\\TP00-OpenGL\\src\\media\\venus.jpg");
  glUniform1i(glGetUniformLocation(g_program, "material.albedoTex"), 0);
  checkOpenGLError("Uniform setup"); // Error check after setting uniform
}

// Define your mesh(es) in the CPU memory
void initCPUgeometry() {
  // TODO: add vertices and indices for your mesh(es)
  g_vertexPositions = { // the array of vertex positions [x0, y0, z0, x1, y1, z1, ...]
  0.f, 0.f, 0.f,
  1.f, 0.f, 0.f,
  0.f, 1.f, 0.f
  };
  g_triangleIndices = { 0, 1, 2 }; // indices just for one triangle
  g_vertexColors = { // the array of vertex colors [r0, g0, b0, r1, g1, b1, ...]
  1.f, 0.f, 0.f,
  0.f, 1.f, 0.f,
  0.f, 0.f, 1.f
  };

}

void initGPUgeometry() {
  // Create a single handle, vertex array object that contains attributes,
  // vertex buffer objects (e.g., vertex's position, normal, and color)
#ifdef _MY_OPENGL_IS_33_
  glGenVertexArrays(1, &g_vao); // If your system doesn't support OpenGL 4.5, you should use this instead of glCreateVertexArrays.
#else
  glCreateVertexArrays(1, &g_vao);
#endif
  glBindVertexArray(g_vao);

  // Generate a GPU buffer to store the positions of the vertices
  size_t vertexBufferSize = sizeof(float)*g_vertexPositions.size(); // Gather the size of the buffer from the CPU-side vector
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &g_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_posVbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, g_vertexPositions.data(), GL_DYNAMIC_READ);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
#else
  glCreateBuffers(1, &g_posVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_posVbo);
  glNamedBufferStorage(g_posVbo, vertexBufferSize, g_vertexPositions.data(), GL_DYNAMIC_STORAGE_BIT); // Create a data storage on the GPU and fill it from a CPU array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
#endif

  // Generate a GPU buffer to store the colors of the vertices
  size_t colorBufferSize = sizeof(float)*g_vertexColors.size(); // Gather the size of the buffer from the CPU-side vector
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &g_colorVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_colorVbo);
  glBufferData(GL_ARRAY_BUFFER, colorBufferSize, g_vertexColors.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);
#else
  glCreateBuffers(1, &g_colorVbo);
  glBindBuffer(GL_ARRAY_BUFFER, g_colorVbo);
  glNamedBufferStorage(g_colorVbo, colorBufferSize, g_vertexColors.data(), GL_DYNAMIC_STORAGE_BIT); // Create a data storage on the GPU and fill it from a CPU array
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(1);

#endif



  // Same for an index buffer object that stores the list of indices of the
  // triangles forming the mesh
  size_t indexBufferSize = sizeof(unsigned int)*g_triangleIndices.size();
#ifdef _MY_OPENGL_IS_33_
  glGenBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, g_triangleIndices.data(), GL_DYNAMIC_READ);
#else
  glCreateBuffers(1, &g_ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
  glNamedBufferStorage(g_ibo, indexBufferSize, g_triangleIndices.data(), GL_DYNAMIC_STORAGE_BIT);
#endif

  glBindVertexArray(0); // deactivate the VAO for now, will be activated again when rendering
}

void initCamera() {
  int width, height;
  glfwGetWindowSize(g_window, &width, &height);
  g_camera.setAspectRatio(static_cast<float>(width)/static_cast<float>(height));

  // we adjust the position of the camera so that it's further from the elements (before it was very close to the sun which had size 1)
  g_camera.setPosition(glm::vec3(0.0, 0.0, 25.0));
  g_camera.setNear(0.1);
  g_camera.setFar(80.1); 
}

void init() {
  initGLFW();
  initOpenGL();

  /*uncomment for triangle
  initCPUgeometry();
  initGPUgeometry(); */

  sphereMesh = Mesh::genSphere(32);   // Crea la mesh per il Sole

  // Inizializza le GPU buffers per ogni mesh
  sphereMesh->init();

  initGPUprogram(); // load and link the shaders

  initCamera();

}

void clear() {
  glDeleteProgram(g_program);

  glfwDestroyWindow(g_window);
  glfwTerminate();
}

// The main rendering call
void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 viewMatrix = g_camera.computeViewMatrix();
    const glm::mat4 projMatrix = g_camera.computeProjectionMatrix();
    const glm::vec3 camPosition = g_camera.getPosition();

    glUniformMatrix4fv(glGetUniformLocation(g_program, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(g_program, "projMat"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    // Posizione della luce (Sole) e della camera
    glm::vec3 lightPos = glm::vec3(0.0f); // Il Sole è all'origine
    glUniform3fv(glGetUniformLocation(g_program, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(g_program, "viewPos"), 1, glm::value_ptr(camPosition));

    // Single global ambient color (e.g., dim gray light)
    glm::vec3 ambientLightColor = glm::vec3(0.4f, 0.4f, 0.4f);
    glUniform3fv(glGetUniformLocation(g_program, "ambientColor"), 1, glm::value_ptr(ambientLightColor));

    // shininess of the material
    glUniform1f(glGetUniformLocation(g_program, "shininess"), 32.0f);

    // --- Renderizza il Sole ---
    modelMatrixSun = glm::scale(modelMatrixSun, glm::vec3(kSizeSun));
    glUniformMatrix4fv(glGetUniformLocation(g_program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrixSun));

    // Colore giallo per il Sole
    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 1.0f, 1.0f, 0.0f);
    glUniform3f(glGetUniformLocation(g_program, "lightColor"), 1.0f, 1.0f, 1.0f); 

    // Setta isSun a true per il Sole
    glUniform1i(glGetUniformLocation(g_program, "isSun"), 1);
    sphereMesh->render();

    // --- Renderizza la Terra ---
    glUniformMatrix4fv(glGetUniformLocation(g_program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrixEarth));

    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.0f, 1.0f, 0.0f);  
    glUniform1i(glGetUniformLocation(g_program, "isSun"), 0); 

    glActiveTexture(GL_TEXTURE0); // activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, g_earthTexID);

    sphereMesh->render();

    // --- Renderizza la Luna ---
    glUniformMatrix4fv(glGetUniformLocation(g_program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrixMoon));

    glUniform3f(glGetUniformLocation(g_program, "objectColor"), 0.0f, 0.0f, 1.0f);
    glUniform1i(glGetUniformLocation(g_program, "isSun"), 0); 

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, g_moonTexID);

    sphereMesh->render();

    // rendering of the two added planets
    glUniformMatrix4fv(glGetUniformLocation(g_program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrixMars));

    glUniform1i(glGetUniformLocation(g_program, "isSun"), 0); 

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, g_marsTexID);

    sphereMesh->render();

    glUniformMatrix4fv(glGetUniformLocation(g_program, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrixVenus));

    glUniform1i(glGetUniformLocation(g_program, "isSun"), 0); 

    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, g_venusTexID);

    sphereMesh->render();
}  

// Update function to compute the orbital positions and rotations based on time
void update(const float currentTimeInSec) {
    // std::cout << currentTimeInSec << std::endl;

    float earthRotationSpeed = 1.0f;  // Full rotation of Earth in arbitrary units (1 rotation per unit time)
    float earthOrbitSpeed = 0.5f * earthRotationSpeed;  // Earth's orbital speed (2 times slower than rotation)
    float moonOrbitSpeed = 2.0f * earthRotationSpeed;  // Moon orbits twice as fast as Earth's rotation
    float moonRotationSpeed = moonOrbitSpeed;  // Moon's rotation is synchronous with its orbit

    // Mars and Venus orbit speeds relative to Earth's
    float marsOrbitSpeed = 0.32f * earthOrbitSpeed;   // Mars orbits more slowly than Earth (687 days vs. 365)
    float venusOrbitSpeed = 1.5f * earthOrbitSpeed;   // Venus orbits faster than Earth (225 days vs. 365)

    modelMatrixEarth = glm::mat4(1.0f);

    glm::mat4 earthTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(kRadOrbitEarth * cos(0.3f * currentTimeInSec), 0.0f, kRadOrbitEarth * sin(0.3f * currentTimeInSec)));
    glm::mat4 earthScale = glm::scale(glm::mat4(1.0f), glm::vec3(kSizeEarth));
    glm::mat4 earthRotate = glm::rotate(glm::mat4(1.0f), 0.6f * currentTimeInSec, glm::vec3(sin(glm::radians(23.5)), cos(glm::radians(23.5)), 0.0f));
    modelMatrixEarth = earthTranslate * earthScale * earthRotate;

    modelMatrixMoon = glm::mat4(1.0f); // Reset Moon matrix

    // transformations for the moon
    glm::mat4 moonTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(kRadOrbitMoon * cos(0.6 * currentTimeInSec), 0.0f, kRadOrbitMoon * sin(0.6 * currentTimeInSec)));
    glm::mat4 moonRotate = glm::rotate(glm::mat4(1.0f), 0.6f * currentTimeInSec, glm::vec3(0, 1, 0.0f));
    glm::mat4 moonScale = glm::scale(glm::mat4(1.0f), glm::vec3(kSizeMoon));
    modelMatrixMoon = earthTranslate * moonTranslate * moonRotate * moonScale;

    // Reset Mars model matrix
    modelMatrixMars = glm::mat4(1.0f);

    // Transformations for Mars
    glm::mat4 marsTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(kRadOrbitMars * cos(marsOrbitSpeed * currentTimeInSec), 0.0f, kRadOrbitMars * sin(marsOrbitSpeed * currentTimeInSec)));
    glm::mat4 marsRotate = glm::rotate(glm::mat4(1.0f), earthRotationSpeed * currentTimeInSec, glm::vec3(0.0f, 1.0f, 0.0f));  // Mars rotating on its axis
    glm::mat4 marsScale = glm::scale(glm::mat4(1.0f), glm::vec3(kSizeMars));
    modelMatrixMars = marsTranslate * marsRotate * marsScale;  

    // Reset Venus model matrix
    modelMatrixVenus = glm::mat4(1.0f);

    // Transformations for Venus
    glm::mat4 venusTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(kRadOrbitVenus * cos(venusOrbitSpeed * currentTimeInSec), 0.0f, kRadOrbitVenus * sin(venusOrbitSpeed * currentTimeInSec)));
    glm::mat4 venusRotate = glm::rotate(glm::mat4(1.0f), -earthRotationSpeed * currentTimeInSec, glm::vec3(0.0f, 1.0f, 0.0f));  // Venus rotating in the opposite direction (retrograde)
    glm::mat4 venusScale = glm::scale(glm::mat4(1.0f), glm::vec3(kSizeVenus));
    modelMatrixVenus = venusTranslate * venusRotate * venusScale; 

}


int main(int argc, char ** argv) {
  init(); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)
  /*The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close*/
  while(!glfwWindowShouldClose(g_window)) {
    update(static_cast<float>(glfwGetTime()));
    render();
    /*will swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that is 
    used to render to during this render iteration and show it as output to the screen.*/
    glfwSwapBuffers(g_window);
    /*checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls 
    the corresponding functions*/
    glfwPollEvents();
  }

  /*As soon as we exit the render loop we would like to properly clean/delete all of GLFW's resources that were allocated*/
  clear();
  return EXIT_SUCCESS;
}