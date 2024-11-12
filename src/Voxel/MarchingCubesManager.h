#ifndef __VOXEL__MARCHING_CUBES_MANAGER_H__
#define __VOXEL__MARCHING_CUBES_MANAGER_H__

#include "../stdafx.h"
#include "../Render/Shader.h"
#include "../Collisions/AABB.h"
#include "../Collisions/Frustum.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "ChunkGrid.h"
#include "VoxelTextures.h"

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
        static constexpr int CHUNCK_DIMENSION_SIZE = 32 - 1;
        static constexpr int VOXEL_SIZE = 1;
        static constexpr unsigned int MAX_RENDER_CHUNK_RADIUS = ChunkGrid::CHUNK_MAX_X_Z_SIZE / 2;
        static constexpr int CHUNK_MAX_X_Z_SIZE = 24;
        static constexpr int CHUNK_MAX_Y_SIZE = 8;
        static constexpr int CHUNK_COUNT = ChunkGrid::CHUNK_MAX_X_Z_SIZE * ChunkGrid::CHUNK_MAX_X_Z_SIZE * ChunkGrid::CHUNK_MAX_Y_SIZE;
        MarchingCubes m_marchingCubes;
        MarchingCubesSolver m_solver;
        VoxelPositionConverter m_converter;
        /* Позиция чанка[0, y, 0] из сетки чанков в мировых координатах.
           x - позиция чанка по оси X, y - позиция чанка по оси Z. */
        glm::ivec2 m_currentOriginChunk{ glm::ivec2( -(CHUNK_MAX_X_Z_SIZE / 2), -(CHUNK_MAX_X_Z_SIZE / 2)) };
        glm::ivec2 m_currentCenterChunk{ glm::ivec2( 0, 0) };
        unsigned int m_renderChunkRadius{ 0 };

        ChunkGrid m_grid;
        VoxelChunk m_chunks[CHUNK_COUNT];
        std::stack<size_t> m_freeChunkIndices;
        std::stack<size_t> m_toUpdateQueue;
        std::vector<glm::ivec2> m_toGenerateQueue;
        VoxelTextures m_textures;

        static bool init();
        static void freeResources();
        void draw(Shader& shader, Frustum frustum);
        void updateChunks(size_t maxCount = 8);
        bool setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setVoxelTexture(int layer, std::string path);
        bool setVoxelTexture(int layer, glm::vec4 color);
        void startTextureEditing();
        void endTextureEditing();
        
        bool isChunkInbounds(int x, int y, int z);
        bool isPositionInbounds(const glm::vec3& position);
        int signum(float x);
        float mod(int position, int modulus = 1);
        float intbound(float s, float ds);
        void pushToUpdateQueue(size_t index);
        VoxelChunk& popFromUpdateQueue();

    };
}

#endif