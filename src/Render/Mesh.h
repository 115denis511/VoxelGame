#ifndef __RENDER__MESH_H__
#define __RENDER__MESH_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"
#include "Vertex.h"

namespace engine {
    enum class MeshDrawMode {
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP
    };

    class Mesh : public utilites::INonCopyable {
    public:
        Mesh(const Vertex* vertices, int vArraySize, const GLuint* indexesArray, int iArraySize, MeshDrawMode mode = MeshDrawMode::TRIANGLES);
        ~Mesh();

        void draw();
        void drawInstanced(GLuint count);

    private:
        GLuint m_VAO, m_VBO, m_EBO, m_indecesCount;
        MeshDrawMode m_drawMode;
    };
}

#endif