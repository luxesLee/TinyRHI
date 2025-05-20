#pragma once
#include <string>
#include <iostream>
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "IShader.h"

namespace TinyRHI
{
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    GLuint ConvertOglStage(IShader::Stage stage)
    {
        switch (stage)
        {
        case IShader::Stage::Vertex: return GL_VERTEX_SHADER;
        case IShader::Stage::Pixel: return GL_FRAGMENT_SHADER;
        case IShader::Stage::Compute: return GL_COMPUTE_SHADER;
        }
        return GL_VERTEX_SHADER;
    }

    class OglShader : public IShader
    {
    public:
        friend class OglShaderProgram;

        OglShader(ShaderDesc& shaderDesc, Stage stage)
        {
            shaderID = glCreateShader(ConvertOglStage(stage));
            glShaderSource(shaderID, 1, ((const GLchar *const *)shaderDesc.codeData), NULL);
            glCompileShader(shaderID);
            checkCompileErrors(shaderID, "SHADER");
        }

    private:
        GLuint shaderID;
    };

    class OglShaderProgram
    {
    public:
        OglShaderProgram(OglShader* pVert, OglShader* pFrag)
        {
            ID = glCreateProgram();
            glAttachShader(ID, pVert->shaderID);
            glAttachShader(ID, pFrag->shaderID);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
        }

        OglShaderProgram(OglShader* pComp)
        {
            ID = glCreateProgram();
            glAttachShader(ID, pComp->shaderID);
            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
        }

        // activate the shader
        // ------------------------------------------------------------------------
        void use() 
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
        void setUInt(const std::string &name, unsigned int value) const
        { 
            glUniform1ui(glGetUniformLocation(ID, name.c_str()), value); 
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
        void setVec4(const std::string &name, float x, float y, float z, float w) 
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

        GLuint ID;
    };
} // namespace TinyRHI
