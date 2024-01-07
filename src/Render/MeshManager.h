#ifndef __RENDER__MESH_MANAGER_H__
#define __RENDER__MESH_MANAGER_H__

#include "../stdafx.h"
#include "../Log.h"
#include "Mesh.h"
#include "UniformStructs.h"

namespace engine {
    class AssetManager;

    struct MeshRelatedData {
        Mesh* mesh{ nullptr };
        uniform_structs::TexturePack textures;
    };

    class MeshManager {
    friend AssetManager;
    
    public:
        static Mesh* addMesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, const uniform_structs::TexturePack& textures, const std::string& name);
        static MeshRelatedData getMesh(std::string& name);
        static Mesh* getPrimitiveRect(GLfloat leftX, GLfloat topY, GLfloat rightX, GLfloat bottomY);

    private:
        MeshManager() {}

        static std::unordered_map<std::string, MeshRelatedData> g_meshes;

        static void freeResources();

    };
}

#endif