#ifndef __VOXEL__MARCHING_CUBES_MANAGER_H__
#define __VOXEL__MARCHING_CUBES_MANAGER_H__

#include "../stdafx.h"
#include "../Render/Shader.h"
#include "../Render/UniformManager.h"
#include "../Collisions/AABB.h"
#include "../Collisions/Frustum.h"
#include "../Scene/Camera.h"
#include "MarchingCubes.h"
#include "MarchingCubesRenderBase.h"
#include "MarchingCubesRenderBigBuffer.h"
#include "MarchingCubesRenderSmallBuffers.h"
#include "MarchingCubesSSBOs.h"
#include "VoxelChunk.h"
#include "VoxelPositionConverter.h"
#include "VoxelRaycast.h"
#include "ChunkGrid.h"
#include "ChunkGridBounds.h"
#include "ChunkGridChanger.h"
#include "ChunkGridVisibility.h"
#include "ChunkGridVoxelEditor.h"

namespace engine {
    class Core;
    class Render;

    class MarchingCubesManager {
        friend Core;
        friend Render;

    public:
        ~MarchingCubesManager();

        static MarchingCubesManager* getInstance();

        bool isPositionHasSolidVoxel(const glm::vec3& position);
        bool raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace);
        void setSolidVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);
        void setLiquidVoxel(const glm::vec3& position, uint8_t id, uint8_t size = 3);
        void deleteSolidVoxel(const glm::vec3& position);
        void deleteLiquidVoxel(const glm::vec3& position);
        Voxel getVoxel(const glm::vec3& position);
        void setRenderChunkRadius(int radius);
        int getRenderChunkRadius() const { return (int)m_renderChunkRadius; }
        bool isUningGlobalChunkSSBO() { return m_usingGlobalChunkSSBO; }
        void setChunkLoader(IChunkLoader* newLoader) { m_gridChanger.setChunkLoader(newLoader); }
        const ChunkGridBounds& getGridBounds() { return m_gridBounds; }
        
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
        MarchingCubesRenderBase* m_render{ nullptr };
        MarchingCubesSSBOs m_ssbos;
        VoxelPositionConverter m_converter;
        ChunkGridBounds m_gridBounds;
        ChunkGridVisibility m_gridVisibility;
        ChunkGrid m_grid{ m_gridBounds, m_ssbos.chunkPositionsSSBO, m_converter };
        ChunkGridChanger m_gridChanger{ m_grid };
        ChunkGridVoxelEditor m_editor{ m_grid, m_gridBounds, m_gridChanger, m_converter };
        unsigned int m_renderChunkRadius{ 0 };
        bool m_usingGlobalChunkSSBO;

        static bool init();
        static void freeResources();
        void draw(const CameraVars& cameraVars, Frustum frustum);
        void updateChunks(size_t maxCount = 8);
        bool setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setVoxelTexture(int layer, std::string path);
        bool setVoxelTexture(int layer, glm::vec4 color);
        void startTextureEditing();
        void endTextureEditing();

    };
}

#endif