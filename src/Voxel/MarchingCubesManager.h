#ifndef __VOXEL__MARCHING_CUBES_MANAGER_H__
#define __VOXEL__MARCHING_CUBES_MANAGER_H__

#include "../stdafx.h"
#include "../Render/Shader.h"
#include "../Render/ShaderStorageManager.h"
#include "../Render/ShaderStorageBuffer.h"
#include "../Render/UniformManager.h"
#include "../Collisions/AABB.h"
#include "../Collisions/Frustum.h"
#include "../Scene/Camera.h"
#include "MarchingCubes.h"
#include "MarchingCubesSolver.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "VoxelRaycast.h"
#include "VoxelTextures.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkGridChanger.h"
#include "ChunkGridVisibility.h"

namespace engine {
    class Core;
    class Render;

    class MarchingCubesManager {
        friend Core;
        friend Render;

    public:
        ~MarchingCubesManager();

        static MarchingCubesManager* getInstance();

        static constexpr GLuint SSBO_BLOCK__GLOBAL_CHUNK_STOTAGE = 12;
        static constexpr GLuint SSBO_BLOCK__CHUNK_POSITIONS = 13;
        static constexpr GLuint SSBO_BLOCK__VOXEL_VERTECES_DATA_IDS = 14;
        static constexpr GLuint SSBO_BLOCK__DRAW_ID_TO_CHUNK = 15;
        static constexpr GLuint CHUNK_BATCH_MAX_SIZE = 14;

        bool isPositionHasSolidVoxel(const glm::vec3& position);
        bool raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace);
        void setVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);
        MarchingCubesVoxel getVoxel(const glm::vec3& position);
        void setRenderChunkRadius(int radius);
        int getRenderChunkRadius() const { return (int)m_renderChunkRadius; }
        void resizeChunkGrid(unsigned int size);
        bool isUningGlobalChunkSSBO() { return m_usingGlobalChunkSSBO; }
        static constexpr std::array<float, 8> getVoxelOffsets() { return MarchingCubes::OFFSETS_STRENGTH; };
        static constexpr std::array<float, 8> getRaycastHitOffset() {
            std::array<float, 8> offsets = getVoxelOffsets();
            for (float& off : offsets) off += 0.5;
            return offsets;
        }

    private:
        MarchingCubesManager();

        static MarchingCubesManager* g_instance;

        MarchingCubes m_marchingCubes;
        MarchingCubesSolver m_solver;
        VoxelPositionConverter m_converter;
        unsigned int m_renderChunkRadius{ 0 };
        ChunkGridBounds m_gridBounds;
        ChunkGridVisibility m_gridVisibility;
        //VoxelChunk m_chunks[ChunkGridBounds::CHUNK_COUNT];
        ChunkGrid m_grid;
        ChunkGridChanger m_gridChanger{ m_grid };
        //std::stack<size_t> m_freeChunkIndices;
        std::vector<glm::ivec2> m_toGenerateQueue; 
        VoxelTextures m_textures;
        GLuint m_commandBuffer;
        std::vector<DrawArraysIndirectCommand> m_drawCommands;
        std::vector<GLuint> m_drawBufferRefs;
        std::vector<glm::vec4> m_drawChunkPositions;
        bool m_usingGlobalChunkSSBO;
        ShaderStorageBuffer<glm::ivec4> m_chunkPositionsSSBO;
        ShaderStorageBuffer<glm::ivec2> m_globalChunkSSBO;
        ShaderStorageBuffer<GLuint> m_drawIdToDataSSBO;
        Shader* m_shader;

        static bool init();
        static void freeResources();
        void draw(const CameraVars& cameraVars, Frustum frustum);
        void drawAllInOne(const CameraVars& cameraVars, Frustum frustum);
        void drawBatches(const CameraVars& cameraVars, Frustum frustum);
        void drawAccumulatedBatches(GLsizei drawCount);
        void updateChunks(size_t maxCount = 8);
        bool setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setVoxelTexture(int layer, std::string path);
        bool setVoxelTexture(int layer, glm::vec4 color);
        void startTextureEditing();
        void endTextureEditing();

    };
}

#endif