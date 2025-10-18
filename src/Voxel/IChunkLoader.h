#ifndef __VOXEL__I_CHUNK_LOADER_H__
#define __VOXEL__I_CHUNK_LOADER_H__

#include "VoxelChunkBase.h"

namespace engine {
    class IChunkLoader {
    public:
        virtual ~IChunkLoader() {}

        virtual void load(const glm::ivec2& worldChunkPosition, std::array<VoxelChunkBase*, 8>& chunkSlice) = 0;
        
    protected:
        IChunkLoader() {}
    };

    class NullChunkLoader : public IChunkLoader{
    public:
        NullChunkLoader() {}
        ~NullChunkLoader() {}

        virtual void load(const glm::ivec2& worldChunkPosition, std::array<VoxelChunkBase*, 8>& chunkSlice) override {}
    };
}

#endif