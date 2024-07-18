#include "Buffers.h"

void setupComputeBuffers(const std::vector<Triangle>& triangles,const std::vector<Ray>& rays, GLuint& triangleBuffer,GLuint& rayBuffer, GLuint& resultBuffer, float maxDistance) {
    // Create and bind the triangle buffer
    glGenBuffers(1, &triangleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleBuffer);
    //ratsBuffer
    glGenBuffers(1, &rayBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rayBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, rays.size() * sizeof(Ray), rays.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rayBuffer);
    // Create and bind the result buffer
    int numOfRays=rays.size();
    float* res=new float[numOfRays];
    glGenBuffers(1, &resultBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, resultBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, numOfRays*sizeof(float), res, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, resultBuffer);
    delete res;

    
}

