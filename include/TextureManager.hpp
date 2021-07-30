#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H


#include <unordered_map>
#include <string>
#include <iostream>
#include <stb_image.cpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

struct Texture2D {
    //texture ID
	GLuint ID;
	//texture image dimensions (in pixels)
	GLuint width, height;
	// Texture Format
	GLuint internal_Format; // Format of texture object
	GLuint image_Format; // Format of loaded image
	// Texture configuration
	GLuint wrap_S; // Wrapping mode on S axis
	GLuint wrap_T; // Wrapping mode on T axis
	GLuint filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint filter_Max; // Filtering mode if texture pixels > screen pixels
	
	int nrComponents;
};

class TextureManager
{
public:
	TextureManager(){}
	~TextureManager(){	clear();_textures.clear();}

	// load a new texture to our storage if we find it and return the ID
	unsigned int add(std::string path, std::string name)
    {
        int i = contains(name);
        if (i != -1) return i;

        std::shared_ptr<Texture2D> texture = loadTextureFromFile(path+"/"+name);
        if (texture != nullptr) {
            _textures.push_back(std::make_pair(name, texture));
            //std::cout << name << "'s id : " << texture->ID << std::endl;
            return texture->ID;
        }
        else
        {
            std::cerr << "TextureManager:: The texture with the name : " << name << " was not find." << std::endl;
            return -1;
        }
    }

	// return the texture by name
	std::shared_ptr<Texture2D> get(std::string name)
    {
        int i = contains(name);
        if (i == -1) return nullptr;
        else return _textures[i].second;
    }

	// bind properly the texture and active it
	void bindAndActive(std::string name)
    {
        std::shared_ptr<Texture2D> text = get(name);
        if (text != nullptr)
        {
            glActiveTexture(GL_TEXTURE0 + text->ID);
            glBindTexture(GL_TEXTURE_2D, text->ID);
        }
    }

	// properly de-allocates all loaded shaders
	void clear(){
        for (auto texture : _textures)
            glDeleteTextures(1, &texture.second->ID);
    }

private:
	// Storage
	std::vector <std::pair <std::string, std::shared_ptr<Texture2D>>> _textures;

    
    unsigned int contains(std::string name)
    {
        for (unsigned int i = 0; i < _textures.size(); ++i)
        {
            if (_textures.at(i).first == name) return i;
        }
        return -1;
    }
    
    
    
	// load and generate a texture from file
	std::shared_ptr<Texture2D> loadTextureFromFile(std::string path)
    {
        std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>();
        glGenTextures(1, &texture->ID);
        
        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        stbi_set_flip_vertically_on_load(false);

        texture->width = width;
        texture->height = height;

        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
                
                
            if (format == GL_RGBA) {
                texture->wrap_S = GL_CLAMP_TO_EDGE;
                texture->wrap_T = GL_CLAMP_TO_EDGE;
            }
            else {
                texture->wrap_S = GL_REPEAT;
                texture->wrap_T = GL_REPEAT;
            }
            texture->internal_Format = format;
            texture->image_Format = format;
            texture->filter_Min = GL_LINEAR_MIPMAP_LINEAR;
            texture->filter_Max = GL_LINEAR;
            
            glBindTexture(GL_TEXTURE_2D, texture->ID);

            // Set Texture wrap and filter modes
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap_S);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap_T);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filter_Min);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filter_Max);

            // Create Texture
            glTexImage2D(GL_TEXTURE_2D, 0, texture->internal_Format, width, height, 0, texture->image_Format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Unbind texture
            glBindTexture(GL_TEXTURE_2D, 0);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "TextureManager:: Texture failed to load at path : " << path << std::endl;
            stbi_image_free(data);
        }
        return texture;
    }
};

#endif
