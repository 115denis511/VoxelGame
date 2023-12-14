#include "Shader.h"

engine::Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
    std::string vertexCode = readFromFile(vertexPath);
    std::string fragmentCode = readFromFile(fragmentPath);

    GLuint vertex = compileShader(vertexCode.c_str(), vertexPath, ShaderType::VERTEX);
    GLuint fragment = compileShader(fragmentCode.c_str(), fragmentPath, ShaderType::FRAGMENT);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertex);
    glAttachShader(m_shaderProgram, fragment);
    glLinkProgram(m_shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        Log::addFatalError("engine::Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) ERROR! Linking failed\n" + std::string(infoLog));
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

engine::Shader::~Shader() {
    glDeleteProgram(m_shaderProgram);
}

void engine::Shader::use() {
    glUseProgram(m_shaderProgram);
}

std::string engine::Shader::readFromFile(const GLchar *path) {
    std::ifstream stream;
    std::stringstream strStream;

    try {
        stream.open(path);
        strStream << stream.rdbuf();
        stream.close();

        return strStream.str();
    }
    catch (std::ifstream::failure& e) {
        Log::addFatalError("engine::Shader::readFromFile: ERROR! Failed to read file " + std::string(path));
    }

    return "";
}

GLuint engine::Shader::compileShader(const GLchar *code, const GLchar *path, ShaderType type) {
    GLint success;
    GLchar infoLog[512];

    GLuint shader = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);

        std::string typeStr;
        switch (type)
        {
        case ShaderType::VERTEX:
            typeStr = "VERTEX";
            break;
        case ShaderType::FRAGMENT:
            typeStr = "FRAGMENT";
            break;
        case ShaderType::GEOMETRY:
            typeStr = "GEOMETRY";
            break;
        
        default:
            break;
        }

        Log::addMessage("engine::Shader::compileShader: ERROR! " + std::string(path) + " " + std::string(typeStr) + "::COMPILATION_FAILED\n" + std::string(infoLog));
    }
    return shader;
}
