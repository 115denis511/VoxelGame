#ifndef __VOXEL__CHUNK_VISIBILITY_STATE_H__
#define __VOXEL__CHUNK_VISIBILITY_STATE_H__

#include "../stdafx.h"

namespace engine {
    class ChunkVisibilityState {
    public:
        ChunkVisibilityState() { clear(); }

        enum class Side : uint32_t { 
            LEFT_FACE = 0, RIGHT_FACE = 1, TOP_FACE = 2, BOTTOM_FACE = 3, BACK_FACE = 4, FRONT_FACE = 5, NONE = 6
        };

        void set(Side side) { m_visibilityState |= sideBit(side); }
        void set(uint32_t side) { m_visibilityState |= (0b1 << side); }
        void setAllVisible() { m_visibilityState = 0b1111111; }
        void merge(const ChunkVisibilityState &other) { m_visibilityState |= other.m_visibilityState; }
        void clear() { m_visibilityState = 0b1 << static_cast<uint32_t>(Side::NONE); }
        bool isVisible(Side side) const { return (m_visibilityState & (sideBit(side))) != 0; }
        bool isVisible(uint32_t side) const { return (m_visibilityState & (0b1 << side)) != 0; }
        static constexpr uint32_t getSidesCount() { return 6U; }
        static constexpr uint32_t sideBit(Side side) { return 0b1 << static_cast<uint32_t>(side); }

    private:  
        uint32_t m_visibilityState = 0;
    };
}

#endif
