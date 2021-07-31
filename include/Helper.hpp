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


#include <random>

#include <stdio.h> 


#include "Shader.hpp"
#include "Camera.hpp"
#include "OBJ_loader.hpp"
#include "Model.hpp"
#include "UserInterface.hpp"

#ifdef __linux__
#include <unistd.h>
#define GetCurrentDir getcwd
#else
#include <direct.h>
#define GetCurrentDir _getcwd
#endif

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}


int getCurrentWorkingDirectory(std::string& currentPath, bool& retflag)
{
    retflag = true;
    char cCurrentPath[FILENAME_MAX];
    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) { return errno; }
    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
    currentPath = std::string(cCurrentPath);
    int found = currentPath.find("build");
    currentPath = currentPath.substr(0, found);
    for (int i = 0; i < (int)currentPath.size(); ++i) { if (currentPath[i] == '\\') { currentPath[i] = '/'; } }
    retflag = false;
    return {};
}

void setIcon(std::string path)
{
    GLFWimage images[2];
    unsigned char* data = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
    images[0].pixels = data;
    images[1].pixels = data;
    glfwSetWindowIcon(glfwGetCurrentContext(), 1, images);

    stbi_image_free(data);
}

int initWindow(GLFWwindow*& window, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, bool& retflag)
{
    retflag = true;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Defered & post process Felix G.", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    retflag = false;
    return {};
}

int createTexture2D(bool& retflag, unsigned int& _textureID, unsigned int _sizeWidth, unsigned int _sizeHeight, GLint _internalFormat, GLenum _Format, GLenum _type, GLint _pxFormat, unsigned int _attachment) {
    retflag = true;
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _sizeWidth, _sizeHeight, 0, _Format, _type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _pxFormat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _pxFormat);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _attachment, GL_TEXTURE_2D, _textureID, 0);
    retflag = false;
    return {};
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



