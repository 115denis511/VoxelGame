#ifndef __UTILITES__PERLIN_NOISE_H__
#define __UTILITES__PERLIN_NOISE_H__

#include "../stdafx.h"

namespace utilites {
    class PerlinNoise{
    public:
        PerlinNoise(int seed = -1) {
            if (seed < 0) {
                std::random_device dev;
                seed = dev();
            }

            std::mt19937 engine(seed);
            std::uniform_int_distribution<std::mt19937::result_type> dist(0,3);
            for (size_t i = 0; i < std::size(m_fastRandomTable); i++) { m_fastRandomTable[i] = dist(engine); }
        }

        float noise(float fx, float fy) {
            int left = (int)std::floor(fx);
            int top  = (int)std::floor(fy);
            float pointInQuadX = fx - left;
            float pointInQuadY = fy - top;

            glm::vec2 topLeftGradient     = GetPseudoRandomGradientVector(left,   top  );
            glm::vec2 topRightGradient    = GetPseudoRandomGradientVector(left+1, top  );
            glm::vec2 bottomLeftGradient  = GetPseudoRandomGradientVector(left,   top+1);
            glm::vec2 bottomRightGradient = GetPseudoRandomGradientVector(left+1, top+1);

            glm::vec2 distanceToTopLeft     = glm::vec2( pointInQuadX,   pointInQuadY   );
            glm::vec2 distanceToTopRight    = glm::vec2( pointInQuadX-1, pointInQuadY   );
            glm::vec2 distanceToBottomLeft  = glm::vec2( pointInQuadX,   pointInQuadY-1 );
            glm::vec2 distanceToBottomRight = glm::vec2( pointInQuadX-1, pointInQuadY-1 );

            float tx1 = glm::dot(distanceToTopLeft,     topLeftGradient);
            float tx2 = glm::dot(distanceToTopRight,    topRightGradient);
            float bx1 = glm::dot(distanceToBottomLeft,  bottomLeftGradient);
            float bx2 = glm::dot(distanceToBottomRight, bottomRightGradient);

            pointInQuadX = QunticCurve(pointInQuadX);
            pointInQuadY = QunticCurve(pointInQuadY);

            float tx = glm::lerp(tx1, tx2, pointInQuadX);
            float bx = glm::lerp(bx1, bx2, pointInQuadX);
            float tb = glm::lerp(tx, bx, pointInQuadY);

            return tb;
        }

        float noise(float fx, float fy, int octaves, float persistence = 0.5f) {
            float amplitude = 1;
            float max = 0;
            float result = 0;

            while (octaves > 0) {
                max += amplitude;
                result += noise(fx, fy) * amplitude;
                amplitude *= persistence;
                fx *= 2;
                fy *= 2;

                octaves--;
            }

            return result/max;
        }

    private:
        int8_t m_fastRandomTable[1024];

        float QunticCurve(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
        glm::vec2 GetPseudoRandomGradientVector(int x, int y) {
            int v = (int)((((x * 1836311903) ^ (y * 2971215073)) + 4807526976) & 1023);
            v = m_fastRandomTable[v];

            constexpr glm::vec2 vectors[] = {
                glm::vec2(1, 0),
                glm::vec2(-1, 0),
                glm::vec2(0, 1),
                glm::vec2(0, -1)
            };
            return vectors[v];
        }
    };
}

#endif