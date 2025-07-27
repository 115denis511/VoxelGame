#ifndef __RENDER__MAPPED_BUFFER_H__
#define __RENDER__MAPPED_BUFFER_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"
#include "GLEnum.h"

namespace engine {
    template <typename DataType>
    class MappedBuffer : public utilites::INonCopyable {
    public:
        MappedBuffer(GLuint buffer, GLuint dataCount, MapAccess access) {
            assert(dataCount > 0 && "Data count value can't be zero");
            
            m_buffer = buffer;

            GLbitfield accessVal = static_cast<GLbitfield>(access);
            m_data = reinterpret_cast<DataType*>(glMapNamedBufferRange(m_buffer, 0, sizeof(DataType) * dataCount, accessVal));

            int errors = 0;
            GLenum errorCode;
            while ((errorCode = glGetError()) != GL_NO_ERROR) {
                errors++;
                std::string error;
                switch (errorCode) {
                case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
                }
                std::cout << "MappedBuffer(dataCount = "<< dataCount <<") " << error << std::endl;
            }
        }

        ~MappedBuffer() {
            glUnmapNamedBuffer(m_buffer);
        }

        DataType* getData() { return m_data; }

    private:
        GLuint m_buffer;
        DataType* m_data;
    };
}

#endif