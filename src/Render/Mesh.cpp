#include "Mesh.h"

engine::Mesh::Mesh(const Vertex *vertices, int vArraySize, const GLuint *indicesArray, int iArraySize, MeshDrawMode mode) {
    m_indicesCount = iArraySize;
    m_drawMode = mode;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
  
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, vArraySize * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCount * sizeof(GLuint), 
                 &indicesArray[0], GL_STATIC_DRAW);

    // Позиции
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Нормали
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // Текстурные координаты
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    // Id костей
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds)); // <- ВАЖНО, ДЛЯ INT ИСПОЛЬЗУЕТСЯ ФУНКЦИЯ С ПРЕФИКСОМ I
    // Веса костей
    glEnableVertexAttribArray(4);	
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));

    glBindVertexArray(0);
}

engine::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) 
    : Mesh(&vertices[0], vertices.size(), &indices[0], indices.size()) {}

engine::Mesh::~Mesh() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
}

void engine::Mesh::draw() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void engine::Mesh::drawInstanced(GLuint count) {
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0, count);
    glBindVertexArray(0);
}

engine::MeshRef engine::Mesh::getMeshRef() {
    return MeshRef(m_VAO, m_indicesCount);
}

engine::MeshRef::MeshRef(GLuint vaoRef, GLuint indicesCount) {
    m_VAO = vaoRef;
    m_indicesCount = indicesCount;
}

void engine::MeshRef::draw() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void engine::MeshRef::drawInstanced(GLuint count) {
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0, count);
    glBindVertexArray(0);
}