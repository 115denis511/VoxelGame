#include "ChunkBuilder.h"

engine::ChunkBuilder::ChunkBuilder() {
    clear();
}

void engine::ChunkBuilder::regenerateChunk(
    MarchingCubes &marchingCubes, 
    ChunkGrid& grid,
    VoxelChunk &chunk, 
    ShaderStorageBuffer<GLuint>& globalChunkSSBO,
    ShaderStorageBuffer<Voxel>& globalChunkGridsSSBO
) {
    clear();
    chunk.clearDrawCommands();

    accumulateCases(marchingCubes, grid, chunk);

    // Твёрдые воксели
    int baseIndex = 0;
    int dataIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_solidCaseData[i].size() == 0) continue;

        for (GLuint caseData : m_solidCaseData[i]) {
            m_dataBuffer[dataIndex] = caseData;
            dataIndex++;
        }

        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_solidCaseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_solidCaseData[i].size();
        chunk.addSolidsDrawCommand(command);
    }

    // Жидкие воксели
    baseIndex = 0;
    dataIndex = 0;
    for (int i = 1; i < 255; i++) {
        if (m_liquidCaseData[i].size() == 0) continue;

        for (GLuint caseData : m_liquidCaseData[i]) {
            m_dataBuffer[dataIndex] |= (caseData << 15);
            dataIndex++;
        }

        DrawArraysIndirectCommand command = marchingCubes.getCommandTemplate(i);
        command.instanceCount = m_liquidCaseData[i].size();
        command.baseInstance = baseIndex;
        baseIndex += m_liquidCaseData[i].size();
        chunk.addLiquidsDrawCommand(command);
    }

    if (chunk.getSolidsDrawCommandsCount() != 0) {
        m_checker.updateVisibilityStates(grid, chunk);
    }
    else {
        chunk.clearVisibilityStatesForEmptyChunk();
    }

    if (globalChunkSSBO.isInited()) {
        constexpr GLsizei dataPerChunk = VoxelChunk::MARCHING_CUBES_BYTE_SIZE;
        GLsizei dataOffset = dataPerChunk * chunk.getIdInSSBO();
        int dataCount = std::max(m_solidCubesCount, m_liquidCubesCount);
        globalChunkSSBO.pushData(&m_dataBuffer[0], dataCount, dataOffset);

        constexpr GLsizei gridSizePerChunk = VoxelChunk::GRID_BYTE_SIZE;
        GLsizei gridOffset = gridSizePerChunk * chunk.getIdInSSBO();
        auto& storage = chunk.getVoxelArray();
        globalChunkGridsSSBO.pushData(storage.getDataPtr(), storage.getSize(), gridOffset);
    }
    else {
        int dataCount = std::max(m_solidCubesCount, m_liquidCubesCount);
        chunk.pushDataInSSBO(m_dataBuffer, dataCount);
    }

    int errors = 0;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        errors++;
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "ChunkBuilder: " << error << std::endl;
    }
}

void engine::ChunkBuilder::accumulateCases(MarchingCubes &marchingCubes, ChunkGrid &grid, VoxelChunk &chunk) {
    constexpr int MARCHING_CUBES_PER_AXIS = VoxelChunkBase::MARCHING_CUBES_PER_AXIS;
    
    #ifdef ENGINE_USE_AVX2
        auto& voxelArray = chunk.getVoxelArray();
        for (int z = 0; z < MARCHING_CUBES_PER_AXIS; z++) {
            __m256i solid[8], liquid[8];

            simd_packRow(voxelArray, 0, 1, z + 1, 0, solid[0], liquid[0]);
            simd_packRow(voxelArray, 1, 1, z + 1, 1, solid[1], liquid[1]);
            simd_packRow(voxelArray, 1, 1, z + 0, 2, solid[2], liquid[2]);
            simd_packRow(voxelArray, 0, 1, z + 0, 3, solid[3], liquid[3]);
            simd_packRow(voxelArray, 0, 0, z + 1, 4, solid[4], liquid[4]);
            simd_packRow(voxelArray, 1, 0, z + 1, 5, solid[5], liquid[5]);
            simd_packRow(voxelArray, 1, 0, z + 0, 6, solid[6], liquid[6]);
            simd_packRow(voxelArray, 0, 0, z + 0, 7, solid[7], liquid[7]);
            
            simd_addRow(0, z, solid, liquid);
            simd_moveUp(solid, liquid);

            for (int y = 1; y < MARCHING_CUBES_PER_AXIS; y++) {
                simd_packRow(voxelArray, 0, y + 1, z + 1, 0, solid[0], liquid[0]);
                simd_packRow(voxelArray, 1, y + 1, z + 1, 1, solid[1], liquid[1]);
                simd_packRow(voxelArray, 1, y + 1, z + 0, 2, solid[2], liquid[2]);
                simd_packRow(voxelArray, 0, y + 1, z + 0, 3, solid[3], liquid[3]);
                
                simd_addRow(y, z, solid, liquid);
                simd_moveUp(solid, liquid);
            }
        }
    #else
        for (int z = 0; z < MARCHING_CUBES_PER_AXIS; z++) {
            for (int y = 0; y < MARCHING_CUBES_PER_AXIS; y++) {
                for (int x = 0; x < MARCHING_CUBES_PER_AXIS; x++) {
                    std::array<Voxel, 8> voxels {
                        chunk.getVoxel(x, y + 1, z + 1), // topFrontLeft
                        chunk.getVoxel(x + 1, y + 1, z + 1), // topFrontRight
                        chunk.getVoxel(x + 1, y + 1, z), // topBackRight
                        chunk.getVoxel(x, y + 1, z), // topBackLeft
                        chunk.getVoxel(x, y, z + 1), // bottomFrontLeft
                        chunk.getVoxel(x + 1, y, z + 1), // bottomFrontRight
                        chunk.getVoxel(x + 1, y, z), // bottomBackRight
                        chunk.getVoxel(x, y, z) // bottomBackLeft
                    };
                    addMarchingCube(marchingCubes, voxels, x, y, z);
                }
            }
        }
    #endif
}

void engine::ChunkBuilder::clear() {
    m_solidCubesCount = 0;
    m_liquidCubesCount = 0;

    for (int i = 0; i < CASE_COUNT; i++) {
        m_solidCaseData[i].clear();
        m_liquidCaseData[i].clear();
    }
}

void engine::ChunkBuilder::addMarchingCube(MarchingCubes& marchingCubes, std::array<Voxel, 8> &voxels, int x, int y, int z) {
    uint8_t caseId = getSolidCaseIndex(voxels);
    if (caseId != 0 && caseId != 255) {
        GLuint caseData = packData(x, y, z);
        m_solidCaseData[caseId].push_back(caseData);
        m_solidCubesCount++;
    }

    uint8_t liquidCaseId = getLiquidCaseIndex(voxels);
    if (liquidCaseId != 0 && liquidCaseId != 255) {
        liquidCaseId |= caseId;
        GLuint caseData = packData(x, y, z);
        m_liquidCaseData[liquidCaseId].push_back(caseData);
        m_liquidCubesCount++;
    }
}

uint8_t engine::ChunkBuilder::getSolidCaseIndex(std::array<Voxel, 8>& voxels) {
    uint8_t caseId = voxels[0].isHaveSolid();
    caseId |= voxels[1].isHaveSolid() << 1;
    caseId |= voxels[2].isHaveSolid() << 2;
    caseId |= voxels[3].isHaveSolid() << 3;
    caseId |= voxels[4].isHaveSolid() << 4;
    caseId |= voxels[5].isHaveSolid() << 5;
    caseId |= voxels[6].isHaveSolid() << 6;
    caseId |= voxels[7].isHaveSolid() << 7;
    return caseId;
}

uint8_t engine::ChunkBuilder::getLiquidCaseIndex(std::array<Voxel,8>& voxels) {
    uint8_t caseId = voxels[0].isHaveLiquid();
    caseId |= voxels[1].isHaveLiquid() << 1;
    caseId |= voxels[2].isHaveLiquid() << 2;
    caseId |= voxels[3].isHaveLiquid() << 3;
    caseId |= voxels[4].isHaveLiquid() << 4;
    caseId |= voxels[5].isHaveLiquid() << 5;
    caseId |= voxels[6].isHaveLiquid() << 6;
    caseId |= voxels[7].isHaveLiquid() << 7;
    return caseId;
}

GLuint engine::ChunkBuilder::packData(int x, int y, int z) {
    int left = x;
    left <<= 5;
    left |= y;
    left <<= 5;
    left |= z;

    return left;
}

#ifdef ENGINE_USE_AVX2
void engine::ChunkBuilder::simd_packRow(
    utilites::Array3D<Voxel, VoxelChunk::VOXELS_PER_AXIS, VoxelChunk::VOXELS_PER_AXIS, VoxelChunk::VOXELS_PER_AXIS, 1, 32> &voxels, 
    int x, int y, int z, int bitPos, __m256i &solidIds, __m256i &liquidIds
) {
    __m256i noVoxel = _mm256_set1_epi32((unsigned int)engine::Voxel::NO_VOXEL);
    __m256i allOnes = _mm256_set1_epi32(0xFFFFFFFF);
    __m256i oneBit = _mm256_set1_epi32(1);

    __m256i as, al;
    engine::Voxel::getRowIds(voxels(x, y, z), as, al);
    as = _mm256_cmpeq_epi32(as, noVoxel);
    as = _mm256_xor_si256(as, allOnes);
    as = _mm256_and_si256(as, oneBit);
    al = _mm256_cmpeq_epi32(al, noVoxel);
    al = _mm256_xor_si256(al, allOnes);
    al = _mm256_and_si256(al, oneBit);

    __m256i bs, bl;
    engine::Voxel::getRowIds(voxels(x + 8, y, z), bs, bl);
    bs = _mm256_cmpeq_epi32(bs, noVoxel);
    bs = _mm256_xor_si256(bs, allOnes);
    bs = _mm256_and_si256(bs, oneBit);
    bl = _mm256_cmpeq_epi32(bl, noVoxel);
    bl = _mm256_xor_si256(bl, allOnes);
    bl = _mm256_and_si256(bl, oneBit);

    __m256i cs, cl;
    engine::Voxel::getRowIds(voxels(x + 16, y, z), cs, cl);
    cs = _mm256_cmpeq_epi32(cs, noVoxel);
    cs = _mm256_xor_si256(cs, allOnes);
    cs = _mm256_and_si256(cs, oneBit);
    cl = _mm256_cmpeq_epi32(cl, noVoxel);
    cl = _mm256_xor_si256(cl, allOnes);
    cl = _mm256_and_si256(cl, oneBit);

    __m256i ds, dl;
    engine::Voxel::getRowIds(voxels(x + 24, y, z), ds, dl);
    ds = _mm256_cmpeq_epi32(ds, noVoxel);
    ds = _mm256_xor_si256(ds, allOnes);
    ds = _mm256_and_si256(ds, oneBit);
    dl = _mm256_cmpeq_epi32(dl, noVoxel);
    dl = _mm256_xor_si256(dl, allOnes);
    dl = _mm256_and_si256(dl, oneBit);

    __m256i i16a = _mm256_packus_epi32 (as, bs);
    __m256i i16b = _mm256_packus_epi32 (cs, ds);
    __m256i i8 = _mm256_packus_epi16 (i16a, i16b);
    solidIds = _mm256_permutevar8x32_epi32(i8, _mm256_setr_epi32(0,4, 1,5, 2,6, 3,7));
    solidIds = _mm256_slli_epi16(solidIds, bitPos);

    i16a = _mm256_packus_epi32 (al, bl);
    i16b = _mm256_packus_epi32 (cl, dl);
    i8 = _mm256_packus_epi16 (i16a, i16b);
    liquidIds = _mm256_permutevar8x32_epi32(i8, _mm256_setr_epi32(0,4, 1,5, 2,6, 3,7));
    liquidIds = _mm256_slli_epi16(liquidIds, bitPos);
}

void engine::ChunkBuilder::simd_addRow(int y, int z, __m256i *solid, __m256i *liquid) {
    constexpr int MARCHING_CUBES_PER_AXIS = VoxelChunkBase::MARCHING_CUBES_PER_AXIS;

    __m256i resSolid, resLiquid;

    resSolid = _mm256_or_si256(solid[0], solid[1]);
    resSolid = _mm256_or_si256(resSolid, solid[2]);
    resSolid = _mm256_or_si256(resSolid, solid[3]);
    resSolid = _mm256_or_si256(resSolid, solid[4]);
    resSolid = _mm256_or_si256(resSolid, solid[5]);
    resSolid = _mm256_or_si256(resSolid, solid[6]);
    resSolid = _mm256_or_si256(resSolid, solid[7]);

    resLiquid = _mm256_or_si256(liquid[0], liquid[1]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[2]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[3]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[4]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[5]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[6]);
    resLiquid = _mm256_or_si256(resLiquid, liquid[7]);

    __m256i allOnes = _mm256_set1_epi8(0xFF);
    __m256i allZeros = _mm256_set1_epi8(0);

    __m256i full = _mm256_cmpeq_epi8(resLiquid, allOnes);
    __m256i empty = _mm256_cmpeq_epi8(resLiquid, allZeros);
    __m256i merge = _mm256_or_si256(full, empty);
    merge = _mm256_xor_si256(merge, allOnes);
    merge = _mm256_and_si256(merge, resSolid);
    resLiquid = _mm256_or_si256(resLiquid, merge);

    __m256i packedData[2] = {
        simd_packData_epi16(0, y, z),
        simd_packData_epi16(16, y, z)
    };
    
    unsigned short* ptrData = (unsigned short*)&packedData[0];
    unsigned char* ptrSolid = (unsigned char*)&resSolid;
    unsigned char* ptrLiquid = (unsigned char*)&resLiquid;
    for (int x = 0; x < MARCHING_CUBES_PER_AXIS; x++) {
        if (ptrSolid[x] != 0 && ptrSolid[x] != 255) {
            m_solidCaseData[ptrSolid[x]].push_back(ptrData[x]);
            m_solidCubesCount++;
        }
        
        if (ptrLiquid[x] != 0 && ptrLiquid[x] != 255) {
            m_liquidCaseData[ptrLiquid[x]].push_back(ptrData[x]);
            m_liquidCubesCount++;
        }
    }
}

void engine::ChunkBuilder::simd_moveUp(__m256i *solid, __m256i *liquid) {
    solid[4] = _mm256_slli_epi16(solid[0], 4);
    solid[5] = _mm256_slli_epi16(solid[1], 4);
    solid[6] = _mm256_slli_epi16(solid[2], 4);
    solid[7] = _mm256_slli_epi16(solid[3], 4);
    liquid[4] = _mm256_slli_epi16(liquid[0], 4);
    liquid[5] = _mm256_slli_epi16(liquid[1], 4);
    liquid[6] = _mm256_slli_epi16(liquid[2], 4);
    liquid[7] = _mm256_slli_epi16(liquid[3], 4);
}

__m256i engine::ChunkBuilder::simd_packData_epi16(int x, int y, int z) {
    __m256i xRow = _mm256_set_epi16(
        x + 15, x + 14, x + 13, x + 12, x + 11, x + 10, x + 9, x + 8,
        x + 7, x + 6, x + 5, x + 4, x + 3, x + 2, x + 1, x
    );
    xRow = _mm256_slli_epi16(xRow, 10);
    __m256i yRow = _mm256_set1_epi16(y);
    yRow = _mm256_slli_epi16(yRow, 5);
    __m256i zRow = _mm256_set1_epi16(z);
    return _mm256_or_si256(xRow, _mm256_or_si256(yRow, zRow));
}
#endif
