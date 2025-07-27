#ifndef __RENDER__SHADER_STORAGE_BUFFER_H__
#define __RENDER__SHADER_STORAGE_BUFFER_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"
#include "MappedBuffer.h"

namespace engine {
    template <typename DataType>
    class ShaderStorageBuffer : public utilites::INonCopyable {
    public:
        ShaderStorageBuffer() {}
        ~ShaderStorageBuffer() {
            if (m_dataCount != 0) glDeleteBuffers(1, &m_ssbo);
        }

        void init(GLuint dataCount, BufferUsage usage) {
            assert(dataCount > 0 && "Data count value can't be zero");
            
            if (m_dataCount == 0) {
                glCreateBuffers(1, &m_ssbo);
                GLsizeiptr byteSize = sizeof(DataType) * dataCount;
                GLenum usageVal = static_cast<GLenum>(usage);
                glNamedBufferData(m_ssbo, byteSize, NULL, usageVal);
            }

            m_dataCount = dataCount;
        }
        bool isInited() { return m_dataCount != 0; }
        const GLuint getSSBO() const { return m_ssbo; }
        const GLuint getCount() const { return m_dataCount; }
        void bind(GLuint blockId) { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, blockId, m_ssbo); }
        MappedBuffer<DataType> getMappedBuffer(MapAccess access) { return MappedBuffer<DataType>(m_ssbo, m_dataCount, access); }
        DataType* map(MapAccess access) { 
            GLbitfield accessVal = static_cast<GLbitfield>(access);
            GLsizei length = sizeof(DataType) * m_dataCount;
            return reinterpret_cast<DataType*>(glMapNamedBufferRange(m_ssbo, 0, length, accessVal)); 
        }
        void unmap() { glUnmapNamedBuffer(m_ssbo); }

        template <typename T>
        void pushData(const T* data, GLsizei count, GLsizei startOffset = 0) { glNamedBufferSubData(m_ssbo, startOffset, sizeof(T) * count, data); }

    private:
        GLuint m_ssbo;
        GLuint m_dataCount{ 0 };
    };
}

#endif