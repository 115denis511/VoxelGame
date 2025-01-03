#ifndef __VOXEL__MARCHING_CUBES_MANAGER_H__
#define __VOXEL__MARCHING_CUBES_MANAGER_H__

#include "../stdafx.h"
#include "../Render/Shader.h"
#include "../Collisions/AABB.h"
#include "../Collisions/Frustum.h"
#include "../Scene/Camera.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "VoxelRaycast.h"
#include "VoxelTextures.h"
#include "ChunkGridBounds.h"
#include "ChunkGrid.h"
#include "ChunkGridVisibility.h"

namespace engine {
    class Core;
    class Render;

    class MarchingCubesManager {
        friend Core;
        friend Render;

    public:
        static MarchingCubesManager* getInstance();

        bool isPositionHasSolidVoxel(const glm::vec3& position);
        bool raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace);
        void setVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);
        MarchingCubesVoxel getVoxel(const glm::vec3& position);
        void setRenderChunkRadius(int radius);
        int getRenderChunkRadius() const { return (int)m_renderChunkRadius; }
        void resizeChunkGrid(unsigned int size);

    private:
        MarchingCubesManager();

        static MarchingCubesManager* g_instance;

        MarchingCubes m_marchingCubes;
        MarchingCubesSolver m_solver;
        VoxelPositionConverter m_converter;
        unsigned int m_renderChunkRadius{ 0 };
        ChunkGridBounds m_gridBounds;
        ChunkGridVisibility m_gridVisibility;
        VoxelChunk m_chunks[ChunkGridBounds::CHUNK_COUNT];
        ChunkGrid m_grid { m_chunks };
        std::stack<size_t> m_freeChunkIndices;
        std::stack<size_t> m_toUpdateQueue;
        std::vector<glm::ivec2> m_toGenerateQueue;
        VoxelTextures m_textures;

        static bool init();
        static void freeResources();
        void draw(Shader& shader, const CameraVars& cameraVars, Frustum frustum);
        void updateChunks(size_t maxCount = 8);
        bool setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setVoxelTexture(int layer, std::string path);
        bool setVoxelTexture(int layer, glm::vec4 color);
        void startTextureEditing();
        void endTextureEditing();
        void pushToUpdateQueue(size_t index);
        VoxelChunk& popFromUpdateQueue();

    };
}

#endif