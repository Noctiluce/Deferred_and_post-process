#ifndef MESH_H
#define MESH_H

#include <vector>
#include "Material.hpp"

struct Mesh 
{
    std::string name;
    
    std::vector <glm::vec3> positions;
    std::vector <glm::vec3> normals;
    std::vector <glm::vec2> uvs;
    
    Material material;
    
    // OpenGL VAO / VBO
    unsigned int vao;
    struct VBOs
    {
        unsigned int vertices;
        unsigned int normals;
        unsigned int texturecoords;
    } vbo;

    // default constructor
    Mesh() { name = ""; } 
    
};

#endif
