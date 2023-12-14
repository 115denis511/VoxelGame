#ifndef __RENDER__SHADER_H__
#define __RENDER__SHADER_H__

#include "../stdafx.h"
#include "../Log.h"
#include "../Utilites/INonCopyable.h"

namespace engine {
    class Shader : public utilites::INonCopyable {
    public:
        Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

        virtual ~Shader();

        void use();

    private:
        GLuint m_shaderProgram{ GL_NONE };

        enum class ShaderType{
            VERTEX = GL_VERTEX_SHADER,
            FRAGMENT = GL_FRAGMENT_SHADER,
            GEOMETRY = GL_GEOMETRY_SHADER
        };

        std::string readFromFile(const GLchar* path);
        GLuint      compileShader(const GLchar* code, const GLchar *path, ShaderType type);
    };
}

#endif