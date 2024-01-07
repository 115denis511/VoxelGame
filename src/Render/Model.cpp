#include "Model.h"

engine::Model::Model(const aiScene *scene, std::string_view path) {
    m_path = path;

    processNode(scene->mRootNode, scene);
}

engine::Model::~Model() {
    
}

void engine::Model::draw() {
    for(unsigned int i = 0; i < m_meshes.size(); i++) {
        UniformManager::setTexturePack(m_textures[i]);
        m_meshes[i].draw();
    }
}

void engine::Model::processNode(aiNode *node, const aiScene *scene) {
    // обработать все полигональные сетки в узле(если есть)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        processMesh(mesh, scene);			
    }
    // выполнить ту же обработку и для каждого потомка узла
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void engine::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::string meshName = m_path + "/" + mesh->mName.C_Str();
    utilites::replaceAll(meshName, "%20", " ");

    MeshRelatedData data = MeshManager::getMesh(meshName);
    if (data.mesh != nullptr) {
        m_meshes.emplace_back(data.mesh->getMeshRef());
        m_textures.emplace_back(data.textures);
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // обработка координат, нормалей и текстурных координат вершин
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z; 

        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        else {
            vertex.normal.x = 0;
            vertex.normal.y = 0;
            vertex.normal.z = 0;
        }

        // сетка обладает набором текстурных координат?
        if(mesh->mTextureCoords[0]) {
            vertex.texCoord.x = mesh->mTextureCoords[0][i].x; 
            vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
        }
        else
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        
        constexpr int MAX_BONE_WEIGHTS = 4;
        for (int j = 0; j < MAX_BONE_WEIGHTS; ++j) {
            vertex.boneIds[j] = -1;
            vertex.boneWeights[j] = 0.f;
        }   

        vertices.push_back(vertex);
    }

    // ОБРАБОТКА КОСТЕЙ
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = utilites::assimp_convert_to::glm(mesh->mBones[boneIndex]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else {
            boneID = m_BoneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            unsigned int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            
            constexpr int MAX_BONE_WEIGHTS = 4;
            for (int j = 0; j < MAX_BONE_WEIGHTS; ++j) {
                if (vertices[vertexId].boneIds[j] < 0) {
                    vertices[vertexId].boneWeights[j] = weight;
                    vertices[vertexId].boneIds[j] = boneID;
                    break;
                }
            }
        }
    }

    // Индексы
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Текстуры
    uniform_structs::TexturePack texturePack;
    if(mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString strDiffuse;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &strDiffuse);
            const char* cStr = strDiffuse.C_Str();
            const aiTexture* rawTextureDiff = scene->GetEmbeddedTexture(cStr);

            bool isDiffuseEmbended = true;
            if (rawTextureDiff == nullptr)
                isDiffuseEmbended = false;

            TextureArrayRef ref;

            if(material->GetTextureCount(aiTextureType_METALNESS) > 0) {
                aiString strMetalness;
                material->GetTexture(aiTextureType_METALNESS, 0, &strMetalness);
                cStr = strMetalness.C_Str();
                const aiTexture* rawTextureMet = scene->GetEmbeddedTexture(cStr);

                bool isMetalnessEmbended = true;
                if (rawTextureMet == nullptr)
                    isMetalnessEmbended = false;

                if (isDiffuseEmbended != isMetalnessEmbended) {
                    Log::addFatalError("engine::Model::processMesh(aiMesh *mesh, const aiScene *scene) ERROR! Somehow one texture embedded, and another not");
                }

                if (isDiffuseEmbended) {
                    ref = TextureManager::addMixedTexture_RGB_A(rawTextureDiff, rawTextureMet, m_path + "/diffuse", glm::vec4(0.f, 0.f, 1.f, 0.f));
                }
                else {
                    ref = TextureManager::addMixedTexture_RGB_A(refinePath(strDiffuse),refinePath(strMetalness), glm::vec4(0.f, 0.f, 1.f, 0.f));
                }
            }
            else {
                if (isDiffuseEmbended) {
                    ref = TextureManager::addTexture(rawTextureDiff, m_path + "/diffuse");
                }
                else {
                    ref = TextureManager::addTexture(refinePath(strDiffuse));
                }
            }

            if (ref.isValidRef()) {
                texturePack.colorSpec = ref.getHandler();
                texturePack.colorSpecLayer = ref.getLayer();
                texturePack.colorSpecCapacity = ref.getCapacity();
            }
        }
        else {
            TextureArrayRef ref = TextureManager::getPlaceholderBlack();
            texturePack.colorSpec = ref.getHandler();
            texturePack.colorSpecLayer = ref.getLayer();
            texturePack.colorSpecCapacity = ref.getCapacity();
        }
    }

    m_meshes.emplace_back(MeshManager::addMesh(vertices, indices, texturePack, meshName)->getMeshRef());
    m_textures.emplace_back(texturePack);
}

std::string engine::Model::refinePath(aiString &path) {
    std::string validName = path.C_Str();
    utilites::replaceAll(validName, "%20", " ");

    return utilites::cutStringAfterCharLast(m_path, '/') + validName;
}
