#version 330 core

layout(location = 0) in vec3 aPosition; 
layout(location = 1) in vec3 aNormal;  
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;     
uniform mat4 viewMat;   
uniform mat4 projMat;   

out vec3 fNormal;      
out vec3 fPosition;    
out vec2 fTexCoord;   

void main() {
    vec4 worldPosition = model * vec4(aPosition, 1.0);
    fPosition = vec3(worldPosition); 

    fNormal = mat3(transpose(inverse(model))) * aNormal;

    fTexCoord = aTexCoord;

    gl_Position = projMat * viewMat * worldPosition;
}

