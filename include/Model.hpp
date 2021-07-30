#ifndef MODEL_H
#define MODEL_H

#include "OBJ_loader.hpp"
#include "DAE_loader.hpp"

#include "Shader.hpp"
#include "TextureManager.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Model {
public:
    
    Model(std::string path, std::string obj_name)
    {
        if (obj_name.substr(obj_name.size() - 4, 4) == ".obj") _loader = new OBJ_loader();
        else if (obj_name.substr(obj_name.size() - 4, 4) == ".dae") _loader = new DAE_loader();
        else std::cerr << obj_name << " have not OBJ or DAE format." << std::endl;
        
        if(!_loader->loadFromFile(path+"/"+obj_name)) std::cout << "Can't load " << obj_name <<std::endl;
        //std::cout << __loader.loadedPositions.size()<< std::endl;
        //modelMesh = new Mesh(path);
        _path = path;
        
        for (int i = 0 ; i < int(_loader->loadedMeshes.size()); ++i){
            glGenVertexArrays(1, &_loader->loadedMeshes[i].vao);
            glBindVertexArray(_loader->loadedMeshes[i].vao);
            
            if (!_loader->loadedMeshes[i].positions.empty()) 
            {
                glGenBuffers(1, &_loader->loadedMeshes[i].vbo.vertices);
                 
                glBindBuffer(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].vbo.vertices);
                glBufferData(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].positions.size() * sizeof(glm::vec3), _loader->loadedMeshes[i].positions.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
            }
            
            if (!_loader->loadedMeshes[i].normals.empty()) 
            {
                glGenBuffers(1, &_loader->loadedMeshes[i].vbo.normals);

                glBindBuffer(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].vbo.normals);
                glBufferData(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].normals.size() * sizeof(glm::vec3), _loader->loadedMeshes[i].normals.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
            }

            if (!_loader->loadedMeshes[i].uvs.empty()) 
            {
                glGenBuffers(1, &_loader->loadedMeshes[i].vbo.texturecoords);
                
                glBindBuffer(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].vbo.texturecoords);
                glBufferData(GL_ARRAY_BUFFER, _loader->loadedMeshes[i].uvs.size() * sizeof(glm::vec2), _loader->loadedMeshes[i].uvs.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(2);
            }
        }
        
        first_init = true;
        _textureManager = TextureManager();
    }
    
    ~Model()
    {
        //delete(modelMesh);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO.vertices);
        glDeleteBuffers(1, &VBO.normals);
        glDeleteBuffers(1, &VBO.texturecoords);
        //glDeleteTextures(1, &textureid);
        delete(_loader);
    }
    
    void draw(Shader & shader, Camera & camera, glm::mat4 i_model)
    {
        // activate shader
        shader.use();
        
        for (int i = 0; i < (int)_loader->loadedMeshes.size(); ++i)
        {
            glBindVertexArray(_loader->loadedMeshes[i].vao);

            
            shader.setVec3("u_material.Ka", _loader->loadedMeshes[i].material.Ka);
            shader.setVec3("u_material.Kd", _loader->loadedMeshes[i].material.Kd);
            shader.setVec3("u_material.Ks", _loader->loadedMeshes[i].material.Ks);
            shader.setVec3("u_material.Ke", _loader->loadedMeshes[i].material.Ke);
            shader.setFloat("u_material.Ns", _loader->loadedMeshes[i].material.Ns);
            shader.setFloat("u_material.Ni", _loader->loadedMeshes[i].material.Ni);
            shader.setFloat("u_material.d", _loader->loadedMeshes[i].material.d);
            shader.setInt("u_material.illum", _loader->loadedMeshes[i].material.illum);
            
            shader.setBool("u_have.map_Ka", _loader->loadedMeshes[i].material.have_map_Ka);
            shader.setBool("u_have.map_Kd", _loader->loadedMeshes[i].material.have_map_Kd);
            shader.setBool("u_have.map_Ks", _loader->loadedMeshes[i].material.have_map_Ks);
            shader.setBool("u_have.map_Ns", _loader->loadedMeshes[i].material.have_map_Ns);
            shader.setBool("u_have.map_d", _loader->loadedMeshes[i].material.have_map_d);
            shader.setBool("u_have.map_Bump", _loader->loadedMeshes[i].material.have_map_Bump);
            
            
           
            // diffuse map
            if (_loader->loadedMeshes[i].material.have_map_Kd)
            {
                //std::cout << "OK have texture " << std::endl;
                
                if (first_init)
                {
                    _loader->loadedMeshes[i].material.map_Kd_id = _textureManager.add(_path, _loader->loadedMeshes[i].material.map_Kd);
                    shader.setInt("u_material.map_Kd", 0);
                    
                    std::cout << "load texture " << _path + "/" + _loader->loadedMeshes[i].material.map_Kd << " with id " << _loader->loadedMeshes[i].material.map_Kd_id << std::endl;
                }
                
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, _loader->loadedMeshes[i].material.map_Kd_id);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0) ;
            }
            
            glDrawArrays(GL_TRIANGLES, 0, _loader->loadedMeshes[i].positions.size());
        }
        first_init = false;
    }
    
private:
   // Mesh* modelMesh;
    Loader *_loader;
    std::string _path;
    bool first_init;
    
    unsigned int VAO;
    struct VBOs
    {
        unsigned int vertices;
        unsigned int normals;
        unsigned int texturecoords;
    } VBO;
    //unsigned int textureid = -1;
    TextureManager _textureManager;
    
    
    
    // -----
    //
    // -----
    unsigned int loadTextureFromFile(const char *path)
    {
        //std::string filename = std::string(path);
        std::string filename = std::string(path);//directory + '/' + filename;
        
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true);  
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        stbi_set_flip_vertically_on_load(false);  
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

};

#endif
