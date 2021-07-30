#pragma once

// Includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <algorithm>

#include <thread>
#include <chrono>


float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}


unsigned int triangleVAO = 0;
unsigned int triangleVBO;
void renderScreenTriangle()
{
    if (triangleVAO == 0)
    {
        float scale = 1.0;
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f * scale,  3.0f * scale, 0.0f, 0.0f, 2.0f,
            -1.0f * scale, -1.0f * scale, 0.0f, 0.0f, 0.0f,
             3.0f * scale,  -1.0f * scale, 0.0f, 2.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &triangleVBO);
        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
    glBindVertexArray(0);
}
