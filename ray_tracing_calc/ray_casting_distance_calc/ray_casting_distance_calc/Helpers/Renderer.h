#ifndef _RENDERER_H
#define _RENDERER_H

#include "common.h"

GLuint compileShader(GLenum shaderType, const char* shaderSource);
void performRaycasting(GLuint program, const glm::vec4 rayOrigin, int numRays, int numTriangles, int maxDistance, int perBatch);

GLuint createComputeProgram(GLuint computeShader);
#endif