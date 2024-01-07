#ifndef __RENDER__MODEL_H__
#define __RENDER__MODEL_H__

#include "../stdafx.h"
#include "../Utilites/AssimpConverter.h"
#include "../Utilites/String.h"
#include "Vertex.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "UniformManager.h"

#include <assimp/material.h>

namespace engine {
    struct BoneInfo {
        int id;
        glm::mat4 offset;
    };

    class Model {
        public:
            Model(const aiScene *scene, std::string_view path);
            ~Model();

            void draw();

        private:
            std::string                                 m_path;
            std::vector<MeshRef>                        m_meshes;
            std::vector<uniform_structs::TexturePack>   m_textures;
            std::map<std::string, BoneInfo>             m_BoneInfoMap;
            int                                         m_BoneCounter = 0;

            void processNode(aiNode *node, const aiScene *scene);
            void processMesh(aiMesh *mesh, const aiScene *scene);
            std::string refinePath(aiString& path);
    };
}

#endif