
#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------

	Shader() {
		ID = 0;
	}

	Shader(const char* computePath){
	    std::string computeCode;
	    std::ifstream cShaderFile;
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try{
            // open files
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << cShaderFile.rdbuf();
            // close file handlers
            cShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure e){
            std::cout << "ERROR::COMPUTE-SHADER::FILE_NOT_SUCCESFULLY_READ at " << computePath << std::endl;
        }
        const char * cShaderCode = computeCode.c_str();
        // compile shaders
        unsigned int compute;
        // vertex shader
        compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        checkCompileErrors(compute, "COMPUTE");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(compute);
	}

	unsigned int generateComputeTexture(int tex_w, int tex_h, unsigned int layout = 0){
        unsigned int tex_norm;
        glGenTextures(1, &tex_norm);
        //glActiveTexture(GL_TEXTURE0+layout);
        //glBindTexture(GL_TEXTURE_2D, tex_norm);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
        //glBindImageTexture(layout, tex_norm, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

        glBindTexture(GL_TEXTURE_2D, tex_norm);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
        return tex_norm;
	}

    //Shader(const char* vertexPath, const char* fragmentPath, const char * geometryPath = nullptr, const char * tessControlPath = nullptr, const char * tessEvalPath = nullptr);
    Shader(const char* vertexPath, const char* fragmentPath, const char * geometryPath = nullptr, const char * tessControlPath = nullptr, const char * tessEvalPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string tessControlCode;
        std::string tessEvalCode;
        std::string geometryCode;
        std::string fragmentCode;

        std::ifstream vShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;
        std::ifstream gShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            if (tessControlPath != nullptr) tcShaderFile.open(tessControlPath);
            if (tessEvalPath != nullptr) teShaderFile.open(tessEvalPath);
            if (geometryPath != nullptr) gShaderFile.open(geometryPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, tcShaderStream, teShaderStream, gShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            if (tessControlPath != nullptr) tcShaderStream << tcShaderFile.rdbuf();
            if (tessEvalPath != nullptr) teShaderStream << teShaderFile.rdbuf();
            if (geometryPath != nullptr) gShaderStream << gShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            if (tessControlPath != nullptr) tcShaderFile.close();
            if (tessEvalPath != nullptr) teShaderFile.close();
            if (geometryPath != nullptr) gShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            if (tessControlPath != nullptr) tessControlCode = tcShaderStream.str();
            if (tessEvalPath != nullptr) tessEvalCode = teShaderStream.str();
            if (geometryPath != nullptr) geometryCode = gShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char * vShaderCode = vertexCode.c_str();
        const char *gShaderCode, *tcShaderCode, *teShaderCode;
        if (tessControlPath != nullptr) tcShaderCode = tessControlCode.c_str();
        if (tessEvalPath != nullptr) teShaderCode = tessEvalCode.c_str();
        if (geometryPath != nullptr) gShaderCode = geometryCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, tessC, tessE, geometry, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // tesselation control
        if (tessControlPath != nullptr) {
            tessC = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessC, 1, &tcShaderCode, NULL);
            glCompileShader(tessC);
            checkCompileErrors(tessC, "TESS_CONTROL");
        }
        // tesselation evaluation
        if (tessEvalPath != nullptr) {
            tessE = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessE, 1, &teShaderCode, NULL);
            glCompileShader(tessE);
            checkCompileErrors(tessE, "TESS_EVALUATION");
        }
        // geometry shader
        if (geometryPath != nullptr) {
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        if (tessControlPath != nullptr) glAttachShader(ID, tessC);
        if (tessEvalPath != nullptr) glAttachShader(ID, tessE);
        if (geometryPath != nullptr) glAttachShader(ID, geometry);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        if (tessControlPath != nullptr) glDeleteShader(tessC);
        if (tessEvalPath != nullptr) glDeleteShader(tessE);
        if (geometryPath != nullptr) glDeleteShader(geometry);
        glDeleteShader(fragment);
    }

	// activate the shader
	// ------------------------------------------------------------------------
	void use() const
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};






#endif
