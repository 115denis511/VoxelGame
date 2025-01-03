#include "ChunkVisibilityState.h"

void engine::ChunkVisibilityState::findVisibleCornersAndEdges() {
    constexpr uint32_t FACE_MASK = 0b1 << static_cast<uint32_t>(Side::LEFT_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::RIGHT_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::TOP_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::BOTTOM_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::BACK_FACE) | 
                                   0b1 << static_cast<uint32_t>(Side::FRONT_FACE);
    constexpr std::array<uint32_t, 64> CORNERS_AND_EDGES_TABLE = getCornersAndEdgesData();

    uint32_t faces = m_visibilityState & FACE_MASK;
    m_visibilityState = CORNERS_AND_EDGES_TABLE[faces];
}