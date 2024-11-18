#ifndef __RENDER__SHADER_H__
#define __RENDER__SHADER_H__

#include "../stdafx.h"
#include "../Log.h"
#include "../Utilites/INonCopyable.h"

namespace engine {
    class Shader : public utilites::INonCopyable {
    public:
        Shader(const GLchar* computePath);
        Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

        virtual ~Shader();

        void use();
        void setBool(const std::string& name, bool value);
        void setInt(const std::string& name, int value);
        void setUInt(const std::string& name, unsigned int value);
        void setBindlessSampler(const std::string& name, GLuint64 handler);
        void setFloat(const std::string& name, float value);
        void setVec2(const std::string& name, glm::vec2 value);
        void setVec2(const std::string& name, float x, float y);
        void setVec3(const std::string& name, glm::vec3 value);
        void setVec3(const std::string& name, float x, float y, float z);
        void setVec4(const std::string& name, glm::vec4 value);
        void setVec4(const std::string& name, float x, float y, float z, float w);
        void setMat4(const std::string& name, glm::mat4& value);
        GLuint getShaderHandler();

    private:
        GLuint m_shaderProgram{ GL_NONE };

        enum class ShaderType{
            VERTEX = GL_VERTEX_SHADER,
            FRAGMENT = GL_FRAGMENT_SHADER,
            GEOMETRY = GL_GEOMETRY_SHADER,
            COMPUTE = GL_COMPUTE_SHADER
        };

        std::string readFromFile(const GLchar* path);
        GLuint      compileShader(const GLchar* code, const GLchar *path, ShaderType type);

    };
}

#endif