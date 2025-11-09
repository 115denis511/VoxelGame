#ifndef __VOXEL__MARCHING_CUBES_CASE_H__
#define __VOXEL__MARCHING_CUBES_CASE_H__

#include "../stdafx.h"

namespace engine {
    struct MarchingCubesVertexData {
        int direction = -1;
        int id = -1;
    };

    class MarchingCubesCase {
    public:
        /*      0-----------1 TOP FRONT
               /|          /|
              / |         / |
             /  |        /  | 
            3-----------2   | TOP BACK
            |   |       |   |
            |   4-------|---5 BOTTOM FRONT
            |  /        |  /
            | /         | /
            |/          |/
            7-----------6 BOTTOM BACK
        */
        MarchingCubesCase(uint8_t combinationId = 0);

        uint8_t resolve();
        MarchingCubesCase rotateRight();
        MarchingCubesCase rotateFront();
        MarchingCubesCase rotateRollRight();
        MarchingCubesCase mirror();
        MarchingCubesVertexData getVertexData(const glm::vec3& position);

        struct CaseVertex {
            bool isSolid{ false };
        };
        
        CaseVertex vertices[8];
        CaseVertex& topFrontLeft() { return vertices[0]; }
        CaseVertex& topFrontRight() { return vertices[1]; }
        CaseVertex& topBackRight() { return vertices[2]; }
        CaseVertex& topBackLeft() { return vertices[3]; }
        CaseVertex& bottomFrontLeft() { return vertices[4]; }
        CaseVertex& bottomFrontRight() { return vertices[5]; }
        CaseVertex& bottomBackRight() { return vertices[6]; }
        CaseVertex& bottomBackLeft() { return vertices[7]; }

    private:
        static constexpr int TOP_FRONT_LEFT_ID = 0;
        static constexpr int TOP_FRONT_RIGHT_ID = 1;
        static constexpr int TOP_BACK_RIGHT_ID = 2;
        static constexpr int TOP_BACK_LEFT_ID = 3;
        static constexpr int BOTTOM_FRONT_LEFT_ID = 4;
        static constexpr int BOTTOM_FRONT_RIGHT_ID = 5;
        static constexpr int BOTTOM_BACK_RIGHT_ID = 6;
        static constexpr int BOTTOM_BACK_LEFT_ID = 7;
    };
}

#endif