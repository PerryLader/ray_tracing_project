#include "Renderer.h"




GLuint compileShader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        // Handle compilation error
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetShaderInfoLog(shader, length, &length, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed in "+std::to_string(shaderType)+ "  "+ std::string(log.data()));
    }
    return shader;
}


GLuint createComputeProgram(GLuint computeShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length);
        glGetProgramInfoLog(program, length, &length, log.data());
        glDeleteProgram(program);
        throw std::runtime_error("Program linking failed: " + std::string(log.data()));
    }
    return program;
}

void performRaycasting(GLuint program, const glm::vec4 rayOrigin,int numRays , int numTriangles,int maxDistance,int perBatch) {
    glUseProgram(program);

    glUniform4f(glGetUniformLocation(program, "rayOrigin"), rayOrigin.x, rayOrigin.y, rayOrigin.z,0);
    glUniform1i(glGetUniformLocation(program, "numRays"),  numRays);
    glUniform1i(glGetUniformLocation(program, "numTriangles"), numTriangles);
    glUniform1f(glGetUniformLocation(program, "maxDistance"), maxDistance);
    //glDispatchCompute((numTriangles/perBatch)+1 , numRays, 1);
    glDispatchCompute(1, numRays, 1);
    
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}





