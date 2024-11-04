#ifndef __COLLISIONS__IS_ON_OR_FORWARD_PLANE_RESULT_H__
#define __COLLISIONS__IS_ON_OR_FORWARD_PLANE_RESULT_H__

namespace engine {
    class IsOnOrForwardPlaneResult {
        public:
            IsOnOrForwardPlaneResult(uint8_t bitset) { m_isOnOrForwardPlane = bitset; } 
            IsOnOrForwardPlaneResult(bool overlapsOrInside, bool completelyInside) {
                m_isOnOrForwardPlane = (uint8_t)overlapsOrInside << 1 | (uint8_t)completelyInside;
            }

            IsOnOrForwardPlaneResult operator && (const IsOnOrForwardPlaneResult& other) { return IsOnOrForwardPlaneResult(m_isOnOrForwardPlane & other.m_isOnOrForwardPlane); }

            inline uint8_t getBitset() { return m_isOnOrForwardPlane; }
            inline bool isInsideOrOverlaps() { return m_isOnOrForwardPlane; }
            inline bool isCompletelyInside() { return m_isOnOrForwardPlane & 0b01; }

        private:
            uint8_t m_isOnOrForwardPlane;
    };
}

#endif
    