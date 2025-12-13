#include "MarchingCubesManager.h"

engine::MarchingCubesManager* engine::MarchingCubesManager::g_instance{ nullptr };

engine::MarchingCubesManager::~MarchingCubesManager() {
    if (m_render != nullptr) delete m_render;

    if (!m_usingGlobalChunkSSBO) {
        m_grid.freeChunksSSBO();
    }
}

engine::MarchingCubesManager *engine::MarchingCubesManager::getInstance() {
    return g_instance;
}

bool engine::MarchingCubesManager::isPositionHasSolidVoxel(const glm::vec3 &position) {
    return m_grid.isPositionHasSolidVoxel(position);
}

bool engine::MarchingCubesManager::init() {
    g_instance = new MarchingCubesManager();

    return false;
}

void engine::MarchingCubesManager::freeResources() {
    delete g_instance;
}

void engine::MarchingCubesManager::updateChunks(size_t maxCount) {
    m_gridChanger.generateChunks(m_usingGlobalChunkSSBO, 1);
    m_gridChanger.updateChunks(m_marchingCubes, m_ssbos.globalChunkSSBO, m_ssbos.globalChunkGridsSSBO, 8);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents) {
    return m_render->setVoxelTexture(layer, rawImage, width, height, nrComponents);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, std::string path) {
    return m_render->setVoxelTexture(layer, path);
}

bool engine::MarchingCubesManager::setVoxelTexture(int layer, glm::vec4 color) {
    return m_render->setVoxelTexture(layer, color);
}

void engine::MarchingCubesManager::startTextureEditing() {
    m_render->startTextureEditing();
}

void engine::MarchingCubesManager::endTextureEditing() {
    m_render->endTextureEditing();
}

engine::MarchingCubesManager::MarchingCubesManager() {
    GLint ssboMaxSize;
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &ssboMaxSize);
    constexpr GLsizeiptr REQUIRED_SSBO_SIZE_SUPPORT = std::max(
        VoxelChunk::MARCHING_CUBES_BYTE_SIZE * ChunkGridBounds::CHUNK_COUNT, 
        VoxelChunk::GRID_BYTE_SIZE * ChunkGridBounds::CHUNK_COUNT
    );

    //if (true) {
    if (ssboMaxSize < REQUIRED_SSBO_SIZE_SUPPORT) { 
        m_usingGlobalChunkSSBO = false;

        m_render = new MarchingCubesRenderSmallBuffers(m_grid, m_gridBounds, m_gridVisibility, m_ssbos);
        m_render->initSSBOs(m_ssbos);

        m_grid.initChunksSSBO();
    }
    else {
        m_usingGlobalChunkSSBO = true;

        m_render = new MarchingCubesRenderBigBuffer(m_grid, m_gridBounds, m_gridVisibility, m_ssbos);
        m_render->initSSBOs(m_ssbos);

        m_grid.initChunkLocationsInSSBO();
    }
}

void engine::MarchingCubesManager::draw(const CameraVars &cameraVars, Frustum frustum) {
    m_render->drawSolid(cameraVars, frustum, m_marchingCubes);
}

bool engine::MarchingCubesManager::raycastVoxel(const glm::vec3& position, const glm::vec3& direction, float maxDistance, glm::ivec3& hitPosition, glm::ivec3& hitFace) {
    glm::ivec3 voxelPosition = VoxelPositionConverter::worldPositionToVoxelPosition(position, m_gridBounds.VOXEL_SIZE);
    return VoxelRaycast::raycastVoxel(m_grid, m_gridBounds, voxelPosition, direction, maxDistance, hitPosition, hitFace);
}

void engine::MarchingCubesManager::setSolidVoxel(const glm::vec3 &position, uint8_t id, uint8_t size) {
    m_editor.setSolidVoxel(position, id, size);
}

void engine::MarchingCubesManager::setLiquidVoxel(const glm::vec3 &position, uint8_t id, uint8_t size) {
    m_editor.setLiquidVoxel(position, id, size);
}

void engine::MarchingCubesManager::deleteSolidVoxel(const glm::vec3& position) {
    m_editor.deleteSolidVoxel(position);
}

void engine::MarchingCubesManager::deleteLiquidVoxel(const glm::vec3& position) {
    m_editor.deleteLiquidVoxel(position);
}

engine::Voxel engine::MarchingCubesManager::getVoxel(const glm::vec3 &position) {
    glm::ivec3 chunkPos = m_converter.worldPositionToChunkPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);
    if (!m_gridBounds.isWorldChunkInbounds(chunkPos.x, chunkPos.y, chunkPos.z)) {
        return Voxel();
    }
    glm::ivec2 localXZ = m_converter.worldChunkToLocalChunkPosition(chunkPos.x, chunkPos.z, m_gridBounds.currentOriginChunk.x, m_gridBounds.currentOriginChunk.y);
    glm::ivec3 localVoxel = m_converter.worldPositionToLocalVoxelPosition(position, m_gridBounds.CHUNCK_DIMENSION_SIZE);

    VoxelChunk& chunk = m_grid.getChunk(localXZ.x, chunkPos.y, localXZ.y);

    return chunk.getVoxel(localVoxel.x, localVoxel.y, localVoxel.z);
}

void engine::MarchingCubesManager::setRenderChunkRadius(int radius) {
    unsigned int uRadius = (unsigned int)radius;

    if (radius <= 0) uRadius = 0;
    else if (uRadius > m_gridBounds.MAX_RENDER_CHUNK_RADIUS) uRadius = m_gridBounds.MAX_RENDER_CHUNK_RADIUS;

    if (uRadius == m_renderChunkRadius) return;

    m_renderChunkRadius = uRadius;
    m_gridBounds.currentOriginChunk = glm::ivec2(m_gridBounds.currentCenterChunk.x - radius, m_gridBounds.currentCenterChunk.y - radius);

    m_gridChanger.resizeGrid(m_ssbos.chunkPositionsSSBO, m_usingGlobalChunkSSBO, uRadius * 2);
}
