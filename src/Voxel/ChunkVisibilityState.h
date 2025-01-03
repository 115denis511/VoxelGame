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
        static constexpr std::array<uint32_t, 64> getCornersAndEdgesData() {
            std::array<uint32_t, 64> table;

            uint32_t leftBit = sideBit(Side::LEFT_FACE);
            uint32_t rightBit = sideBit(Side::RIGHT_FACE);
            uint32_t topBit = sideBit(Side::TOP_FACE);
            uint32_t bottomBit = sideBit(Side::BOTTOM_FACE);
            uint32_t backBit = sideBit(Side::BACK_FACE);
            uint32_t frontBit = sideBit(Side::FRONT_FACE);

            for (uint32_t i = 0; i < 64U; i++) {
                uint32_t result = i;

                if ((i & (leftBit | frontBit)) == (leftBit | frontBit)) result |= sideBit(Side::LEFT_FRONT_EDGE);
                if ((i & (rightBit | frontBit)) == (rightBit | frontBit)) result |= sideBit(Side::RIGHT_FRONT_EDGE);
                if ((i & (leftBit | backBit)) == (leftBit | backBit)) result |= sideBit(Side::LEFT_BACK_EDGE);
                if ((i & (rightBit | backBit)) == (rightBit | backBit)) result |= sideBit(Side::RIGHT_BACK_EDGE);
                
                if ((i & (topBit | frontBit)) == (topBit | frontBit)) result |= sideBit(Side::TOP_FRONT_EDGE);
                if ((i & (topBit | rightBit)) == (topBit | rightBit)) result |= sideBit(Side::TOP_RIGHT_EDGE);
                if ((i & (topBit | backBit)) == (topBit | backBit)) result |= sideBit(Side::TOP_BACK_EDGE);
                if ((i & (topBit | leftBit)) == (topBit | leftBit)) result |= sideBit(Side::TOP_LEFT_EDGE);
                
                if ((i & (bottomBit | frontBit)) == (bottomBit | frontBit)) result |= sideBit(Side::BOTTOM_FRONT_EDGE);
                if ((i & (bottomBit | rightBit)) == (bottomBit | rightBit)) result |= sideBit(Side::BOTTOM_RIGHT_EDGE);
                if ((i & (bottomBit | backBit)) == (bottomBit | backBit)) result |= sideBit(Side::BOTTOM_BACK_EDGE);
                if ((i & (bottomBit | leftBit)) == (bottomBit | leftBit)) result |= sideBit(Side::BOTTOM_LEFT_EDGE);

                if ((i & (leftBit | topBit | frontBit)) == (leftBit | topBit | frontBit)) result |= sideBit(Side::LEFT_TOP_FRONT_CORNER);
                if ((i & (rightBit | topBit | frontBit)) == (rightBit | topBit | frontBit)) result |= sideBit(Side::RIGHT_TOP_FRONT_CORNER);
                if ((i & (rightBit | topBit | backBit)) == (rightBit | topBit | backBit)) result |= sideBit(Side::RIGHT_TOP_BACK_CORNER);
                if ((i & (leftBit | topBit | backBit)) == (leftBit | topBit | backBit)) result |= sideBit(Side::LEFT_TOP_BACK_CORNER);
                
                if ((i & (leftBit | bottomBit | frontBit)) == (leftBit | bottomBit | frontBit)) result |= sideBit(Side::LEFT_BOTTOM_FRONT_CORNER);
                if ((i & (rightBit | bottomBit | frontBit)) == (rightBit | bottomBit | frontBit)) result |= sideBit(Side::RIGHT_BOTTOM_FRONT_CORNER);
                if ((i & (rightBit | bottomBit | backBit)) == (rightBit | bottomBit | backBit)) result |= sideBit(Side::RIGHT_BOTTOM_BACK_CORNER);
                if ((i & (leftBit | bottomBit | backBit)) == (leftBit | bottomBit | backBit)) result |= sideBit(Side::LEFT_BOTTOM_BACK_CORNER);

                table[i] = result;
            }

            return table;
        }
        static constexpr uint32_t sideBit(Side side) { return 0b1 << static_cast<uint32_t>(side); }

    private:  
        uint32_t m_visibilityState = 0;
    };
}

#endif
