#include "Shader.h"

engine::Shader::Shader(const GLchar *computePath) {
    std::string computeCode = readFromFile(computePath);

    GLuint compute = compileShader(computeCode.c_str(), computePath, ShaderType::COMPUTE);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, compute);
    glLinkProgram(m_shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        Log::addFatalError("engine::Shader::Shader(const GLchar *computePath) ERROR! Linking failed\n" + std::string(infoLog));
    }

    glDeleteShader(compute);
}

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

void engine::Shader::Shader::setBool(const std::string& name, bool value) {
    glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}

void engine::Shader::Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}

void engine::Shader::setUnsignedInt(const std::string &name, unsigned int value) {
    glUniform1ui(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}

void engine::Shader::setBindlessSampler(const std::string &name, GLuint64 handler) {
    glUniformHandleui64ARB(glGetUniformLocation(m_shaderProgram, name.c_str()), handler);
}

void engine::Shader::Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
}

void engine::Shader::Shader::setVec2(const std::string& name, glm::vec2 value) {
    glUniform2fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
}

void engine::Shader::Shader::setVec2(const std::string& name, float x, float y) {
    glUniform2f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y);
}

void engine::Shader::Shader::setVec3(const std::string& name, glm::vec3 value) {
    glUniform3fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
}

void engine::Shader::Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y, z);
}

void engine::Shader::Shader::setVec4(const std::string& name, glm::vec4 value) {
    glUniform4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
}

void engine::Shader::Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y, z, w);
}

void engine::Shader::Shader::setMat4(const std::string& name, glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

GLuint engine::Shader::getShaderHandler() {
    return m_shaderProgram;
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
        case ShaderType::COMPUTE:
            typeStr = "COMPUTE";
            break;
        
        default:
            break;
        }

        Log::addMessage("engine::Shader::compileShader: ERROR! " + std::string(path) + " " + std::string(typeStr) + "::COMPILATION_FAILED\n" + std::string(infoLog));
    }
    return shader;
}
