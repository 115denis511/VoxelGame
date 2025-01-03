#include "ChunkVisibilityState.h"

void engine::ChunkVisibilityState::findVisibleCornersAndEdges() {
    constexpr uint32_t FACE_MASK = 0b1 << static_cast<uint32_t>(Side::LEFT_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::RIGHT_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::TOP_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::BOTTOM_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::BACK_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::FRONT_FACE);

    constexpr std::array<uint32_t, 64> CORNERS_AND_EDGES_TABLE = [](){
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
    }();

    uint32_t faces = m_visibilityState & FACE_MASK;
    m_visibilityState = CORNERS_AND_EDGES_TABLE[faces];
}