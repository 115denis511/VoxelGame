#ifndef __RENDER__MESH_H__
#define __RENDER__MESH_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"
#include "Vertex.h"

namespace engine {
    /// @brief Вспомогательный класс для хранения копии VAO меша, который можно копировать,
    ///        хранить локально без использования указателя и который не делает удаление VAO при вызове деструктора.
    class MeshRef {
    public:
        MeshRef(GLuint vaoRef = 0, GLuint indicesCount = 0);
        
        void draw();
        void drawInstanced(GLuint count);

    private:
        GLuint m_VAO, m_indicesCount;
    };

    class Mesh : public utilites::INonCopyable {
    public:
        Mesh(const Vertex* vertices, int vArraySize, const GLuint* indicesArray, int iArraySize);
        Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
        ~Mesh();

        void draw();
        void drawInstanced(GLuint count);
        MeshRef getMeshRef() const;

    private:
        GLuint m_VAO, m_VBO, m_EBO, m_indicesCount;
    };
}

#endif