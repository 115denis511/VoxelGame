#ifndef __TEST__EXAMPLE_WORLD_GENERATOR_H__
#define __TEST__EXAMPLE_WORLD_GENERATOR_H__

#include "../stdafx.h"
#include "../Voxel/IChunkLoader.h"

class ExampleWorldGenerator : public engine::IChunkLoader {
    virtual void load(const glm::ivec2& worldChunkPosition, std::array<engine::VoxelChunkBase*, 8>& chunkSlice) override {
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
};

#endif