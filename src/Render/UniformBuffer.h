#ifndef __RENDER__UNIFORM_BUFFER_H__
#define __RENDER__UNIFORM_BUFFER_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"

namespace engine {
    class UniformBuffer : public utilites::INonCopyable{
    public:
        UniformBuffer(GLuint blockIndex, GLsizeiptr byteSize);
        ~UniformBuffer();

        const GLuint getUBO();
        const GLuint getBlockIndex();

    private:
        GLuint      m_UBO;
        GLuint      m_blockIndex;
        GLsizeiptr  m_byteSize;
    };
}

#endif