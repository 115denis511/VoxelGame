#ifndef __VOXEL__MARCHING_CUBES_MANAGER_H__
#define __VOXEL__MARCHING_CUBES_MANAGER_H__

#include "../stdafx.h"
#include "../Render/Shader.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "ChunkGrid.h"

namespace engine {
    class Core;

    class MarchingCubesManager {
        friend Core;

    public:
        static MarchingCubesManager* getInstance();

        bool isPositionHasSolidVoxel(const glm::vec3& position);
        bool raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace);
        void setVoxel(const glm::vec3& position, uint8_t id);
        void resizeChunkGrid(unsigned int size);

    private:
        MarchingCubesManager();

        static MarchingCubesManager* g_instance;
        static constexpr int CHUNCK_DIMENSION_SIZE = 32 - 1;
        static constexpr int VOXEL_SIZE = 1;
        static constexpr int CHUNK_MAX_X_Z_SIZE = 6;
        static constexpr int CHUNK_MAX_Y_SIZE = 1;
        static constexpr int CHUNK_COUNT = ChunkGrid::CHUNK_MAX_X_Z_SIZE * ChunkGrid::CHUNK_MAX_X_Z_SIZE * ChunkGrid::CHUNK_MAX_Y_SIZE;
        MarchingCubes m_marchingCubes;
        MarchingCubesSolver m_solver;
        VoxelPositionConverter m_converter;
        /* Позиция чанка[0, y, 0] из сетки чанков в мировых координатах.
           x - позиция чанка по оси X, y - позиция чанка по оси Z. */
        glm::ivec2 m_currentOriginChunk{ glm::ivec2( -(CHUNK_MAX_X_Z_SIZE / 2), -(CHUNK_MAX_X_Z_SIZE / 2)) };

        ChunkGrid m_grid;
        VoxelChunk m_chunks[CHUNK_COUNT];
        std::stack<size_t> m_freeChunkIndices;
        std::stack<size_t> m_toUpdateQueue;
        std::vector<glm::ivec2> m_toGenerateQueue;

        static bool init();
        static void freeResources();
        void draw(Shader& shader);
        void updateChunks(size_t maxCount = 8);
        
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