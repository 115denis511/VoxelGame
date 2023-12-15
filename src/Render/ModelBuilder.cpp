#include "ModelBuilder.h"

engine::Mesh* engine::ModelBuilder::buildPrimitivePlane() {
    std::vector<Vertex> vertices;

    vertices.push_back({glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3( 0.5f, -0.5f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3( 0.5f,  0.5f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3(-0.5f,  0.5f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)});

    constexpr GLuint indeces[] = {
        0, 1, 2,   // Первый треугольник
        0, 2, 3,   // Второй треугольник
    };

    return new Mesh(&vertices[0], vertices.size(), indeces, std::size(indeces));
}

engine::Mesh* engine::ModelBuilder::buildPrimitiveScreenPlane() {
    std::vector<Vertex> vertices;

    vertices.push_back({glm::vec3(-1.f, -1.f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3( 1.f, -1.f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3( 1.f,  1.f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)});
    vertices.push_back({glm::vec3(-1.f,  1.f, 0.f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::ivec4(-1), glm::vec4(0.f)});

    constexpr GLuint indeces[] = {
        0, 1, 2,   // Первый треугольник
        0, 2, 3,   // Второй треугольник
    };

    return new Mesh(&vertices[0], vertices.size(), indeces, std::size(indeces));
}
