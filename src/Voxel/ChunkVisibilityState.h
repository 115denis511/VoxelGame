#ifndef __VOXEL__CHUNK_VISIBILITY_STATE_H__
#define __VOXEL__CHUNK_VISIBILITY_STATE_H__

#include "../stdafx.h"

namespace engine {
    class ChunkVisibilityState {
    public:
        ChunkVisibilityState() { clear(); }

        enum class Side : uint32_t { 
            LEFT_FACE = 0, RIGHT_FACE = 1, TOP_FACE = 2, BOTTOM_FACE = 3, BACK_FACE = 4, FRONT_FACE = 5,
            LEFT_TOP_FRONT_CORNER = 6, RIGHT_TOP_FRONT_CORNER = 7, RIGHT_TOP_BACK_CORNER = 8, LEFT_TOP_BACK_CORNER = 9,
            LEFT_BOTTOM_FRONT_CORNER = 10, RIGHT_BOTTOM_FRONT_CORNER = 11, RIGHT_BOTTOM_BACK_CORNER = 12, LEFT_BOTTOM_BACK_CORNER = 13,
            TOP_FRONT_EDGE = 14, TOP_RIGHT_EDGE = 15, TOP_BACK_EDGE = 16, TOP_LEFT_EDGE = 17, 
            BOTTOM_FRONT_EDGE = 18, BOTTOM_RIGHT_EDGE = 19, BOTTOM_BACK_EDGE = 20, BOTTOM_LEFT_EDGE = 21,
            LEFT_FRONT_EDGE = 22, RIGHT_FRONT_EDGE = 23, LEFT_BACK_EDGE = 24, RIGHT_BACK_EDGE = 25,
            NONE = 26
        };

        void set(Side side) { m_visibilityState |= sideBit(side); }
        void set(uint32_t side) { m_visibilityState |= (0b1 << side); }
        void setAllVisible() { m_visibilityState = 0b11111111111111111111111111111111; }
        void merge(const ChunkVisibilityState &other) { m_visibilityState |= other.m_visibilityState; }
        void clear() { m_visibilityState = 0b1 << static_cast<uint32_t>(Side::NONE); }
        bool isVisible(Side side) const { return (m_visibilityState & (sideBit(side))) != 0; }
        bool isVisible(uint32_t side) const { return (m_visibilityState & (0b1 << side)) != 0; }
        void findVisibleCornersAndEdges();
        uint32_t getSidesCount() const { return 26U; }
        static constexpr uint32_t sideBit(Side side) { return 0b1 << static_cast<uint32_t>(side); }

    private:  
        uint32_t m_visibilityState = 0;
    };
}

#endif
