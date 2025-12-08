#ifndef __TEST__EXAMPLE_WORLD_GENERATOR_H__
#define __TEST__EXAMPLE_WORLD_GENERATOR_H__

#include "../stdafx.h"
#include "../Voxel/IChunkLoader.h"
#include "../Utilites/PerlinNoise.h"

class ExampleWorldGenerator : public engine::IChunkLoader {
public:
    virtual void load(const glm::ivec2& worldChunkPosition, std::array<engine::VoxelChunkBase*, 8>& chunkSlice) override {
        makeTestTerrain(worldChunkPosition, chunkSlice);
        //makePerlinNoiseTerrain(worldChunkPosition, chunkSlice);
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
        else if (worldChunkPosition.x == 2 && worldChunkPosition.y == -3) {
            for (int y = 2; y < 10; y++) chunkSlice[0]->setVoxel(10,y,10, 1);
            for (int x = 8; x < 10; x++) {
                chunkSlice[0]->setVoxel(x,5,10, 5, 2, engine::Voxel::Type::WATER);
                chunkSlice[0]->setVoxel(x + 3,5,10, 6, 2, engine::Voxel::Type::WATER);

                chunkSlice[0]->setVoxel(10,5,x, 7, 2, engine::Voxel::Type::WATER);
                chunkSlice[0]->setVoxel(10,5,x + 3, 6, 2, engine::Voxel::Type::WATER);

                for (int z = 8; z < 10; z++) {
                    chunkSlice[0]->setVoxel(x,5,z, 8, 2, engine::Voxel::Type::WATER);
                    chunkSlice[0]->setVoxel(x + 3,5,z, 8, 2, engine::Voxel::Type::WATER);

                    chunkSlice[0]->setVoxel(x,5,z + 3, 9, 2, engine::Voxel::Type::WATER);
                    chunkSlice[0]->setVoxel(x + 3,5,z + 3, 9, 2, engine::Voxel::Type::WATER);
                }
            }
        }
        else if (worldChunkPosition.x == 2 && worldChunkPosition.y == 2) {
            for (size_t x = 0; x < 32; x++) {
                for (size_t z = 0; z < 32; z++){
                    chunkSlice[0]->setVoxel(x,1,z, 255);
                }
            }

            for (size_t x = 0; x < 10; x++) {
                for (size_t y = 0; y < 10; y++) {
                    chunkSlice[0]->setVoxel(5 + x,5,5 + y, 4, 3, engine::Voxel::Type::WATER);
                    chunkSlice[0]->setVoxel(5 + x,5 + 10,5 + y, 4, 3, engine::Voxel::Type::WATER);
                    chunkSlice[0]->setVoxel(5 + x,5 + y,5, 4, 3, engine::Voxel::Type::WATER);
                    chunkSlice[0]->setVoxel(5 + x,5 + y,5 + 10, 4, 3, engine::Voxel::Type::WATER);
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