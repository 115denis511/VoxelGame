#ifndef __VOXEL__MARCHING_CUBES_CASE_H__
#define __VOXEL__MARCHING_CUBES_CASE_H__

#include "../stdafx.h"

namespace engine {
    struct MarchingCubesVertecesIds {
        //int ids[6] = { -1, -1, -1, -1, -1, -1 };
        int ids[6] = { 0, 0, 0, 0, 0, 0 };
    };

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
        /// @brief Установить id для вершины по индексу
        /// @param id Устанавливаемый id. Значение от 0 до 4
        /// @param index Индекс вершины угла марширующего куба. Значение от 0 до 7
        void setVertexId(int id, int index);
        MarchingCubesVertecesIds copyVertecesId() { return vertecesId; }

        struct CaseVertex {
            bool isSolid{ false };
            int id{ -1 };
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
        MarchingCubesVertecesIds vertecesId;

        void swapIds(int aIndex, const CaseVertex& b);
    };
}

#endif