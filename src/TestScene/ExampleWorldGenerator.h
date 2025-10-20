#ifndef __TEST__EXAMPLE_WORLD_GENERATOR_H__
#define __TEST__EXAMPLE_WORLD_GENERATOR_H__

#include "../stdafx.h"
#include "../Voxel/IChunkLoader.h"
#include "../Utilites/PerlinNoise.h"

class ExampleWorldGenerator : public engine::IChunkLoader {
public:
    virtual void load(const glm::ivec2& worldChunkPosition, std::array<engine::VoxelChunkBase*, 8>& chunkSlice) override {
        //makeTestTerrain(worldChunkPosition, chunkSlice);
        makePerlinNoiseTerrain(worldChunkPosition, chunkSlice);
    }

    void makePerlinNoiseTerrain(const glm::ivec2& worldChunkPosition, std::array<engine::VoxelChunkBase*, 8>& chunkSlice) {
        for (int x = 0; x < engine::VoxelChunkBase::VOXEL_CHUNK_SIZE; x++) {
            for (int y = 0; y < engine::VoxelChunkBase::VOXEL_CHUNK_SIZE; y++) {
                for (int z = 0; z < engine::VoxelChunkBase::VOXEL_CHUNK_SIZE; z++) {
                    chunkSlice[0]->setVoxel(x ,y, z, 3, 7);    
                }
            }
        }

        for (int x = 0; x < engine::VoxelChunkBase::VOXEL_CHUNK_SIZE; x++) {
            for (int z = 0; z < engine::VoxelChunkBase::VOXEL_CHUNK_SIZE; z++) {
                float h = m_perlin.noise((float)x / 32 + worldChunkPosition.x, (float)z / 32 + worldChunkPosition.y, 1, 0.7f);
                int ih = (h + 1) * 16 * 8;
                int y = (ih / 8);
                int t = ih & 0b111;
                //std::cout << h << "\n";
                
                chunkSlice[1]->setVoxel(x ,y, z, 0, t);
                y--;
                while (y >= 0) {
                    chunkSlice[1]->setVoxel(x ,y, z, 3, 7);
                    y--;
                }
            }
        }
    }

    void makeTestTerrain(const glm::ivec2& worldChunkPosition, std::array<engine::VoxelChunkBase*, 8>& chunkSlice) {
        for (size_t x = 0; x < 32; x++) {
            for (size_t z = 0; z < 32; z++){
                chunkSlice[0]->setVoxel(x,1,z, 0);
            }
        }

        if (worldChunkPosition.x == 0 && worldChunkPosition.y == 0) {
            for (size_t y = 0; y < 32; y+=2) {
                for (size_t z = 0; z < 32; z+=2) {
                    for (size_t x = 0; x < 32; x+=2) {
                        chunkSlice[0]->setVoxel(x,y,z, 1);
                    }
                }
            }
        }
        else if (worldChunkPosition.x == 1 && worldChunkPosition.y == -3) {
            for (size_t y = 0; y < 32; y++) {
                for (size_t z = 0; z < 32; z++) {
                    chunkSlice[0]->setVoxel(25,y,z, 1);
                }
            }
        }

        if (worldChunkPosition.x <= -1) { // && (std::abs(worldChunkPosition.x) + std::abs(worldChunkPosition.y) + y) % 2 != 0
            for (size_t y = 0; y < 32; y++) {
                for (size_t z = 0; z < 32; z++){
                    chunkSlice[0]->setVoxel(0,y,z, 1);
                    chunkSlice[0]->setVoxel(31,y,z, 1);
                    chunkSlice[0]->setVoxel(z,y,0, 1);
                    chunkSlice[0]->setVoxel(z,y,31, 1);
                    chunkSlice[0]->setVoxel(z,0,y, 1);
                    chunkSlice[0]->setVoxel(z,31,y, 1);
                }
            }
        }
    }

private:
    utilites::PerlinNoise m_perlin;
};

#endif