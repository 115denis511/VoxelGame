#include "MarchingCubesCase.h"

engine::MarchingCubesCase::MarchingCubesCase(uint8_t combinationId) {
    vertices[0].isSolid = combinationId & 0b00000001;
    vertices[1].isSolid = combinationId & 0b00000010;
    vertices[2].isSolid = combinationId & 0b00000100;
    vertices[3].isSolid = combinationId & 0b00001000;
    vertices[4].isSolid = combinationId & 0b00010000;
    vertices[5].isSolid = combinationId & 0b00100000;
    vertices[6].isSolid = combinationId & 0b01000000;
    vertices[7].isSolid = combinationId & 0b10000000;
}

uint8_t engine::MarchingCubesCase::resolve() {
    uint8_t result = static_cast<uint8_t>(topFrontLeft().isSolid);
    result = result | (topFrontRight().isSolid << 1);
    result = result | (topBackRight().isSolid << 2);
    result = result | (topBackLeft().isSolid << 3);
    result = result | (bottomFrontLeft().isSolid << 4);
    result = result | (bottomFrontRight().isSolid << 5);
    result = result | (bottomBackRight().isSolid << 6);
    result = result | (bottomBackLeft().isSolid << 7);

    return result;
}

engine::MarchingCubesCase engine::MarchingCubesCase::rotateRight() {
    MarchingCubesCase result;

    result.topFrontLeft() = topBackLeft();
    result.topFrontRight() = topFrontLeft();
    result.topBackRight() = topFrontRight();
    result.topBackLeft() = topBackRight();
    result.bottomFrontLeft() = bottomBackLeft();
    result.bottomFrontRight() = bottomFrontLeft();
    result.bottomBackRight() = bottomFrontRight();
    result.bottomBackLeft() = bottomBackRight();

    return result;
}

engine::MarchingCubesCase engine::MarchingCubesCase::rotateFront() {
    MarchingCubesCase result;

    result.topFrontLeft() = topBackLeft();
    result.bottomFrontLeft() = topFrontLeft();
    result.bottomBackLeft() = bottomFrontLeft();
    result.topBackLeft() = bottomBackLeft();
    result.topFrontRight() = topBackRight();
    result.bottomFrontRight() = topFrontRight();
    result.bottomBackRight() = bottomFrontRight();
    result.topBackRight() = bottomBackRight();

    return result;
}

engine::MarchingCubesCase engine::MarchingCubesCase::rotateRollRight() {
    MarchingCubesCase result;

    result.topFrontLeft() = bottomFrontLeft();
    result.topFrontRight() = topFrontLeft();
    result.bottomFrontRight() = topFrontRight();
    result.bottomFrontLeft() = bottomFrontRight();
    result.topBackLeft() = bottomBackLeft();
    result.topBackRight() = topBackLeft();
    result.bottomBackRight() = topBackRight();
    result.bottomBackLeft() = bottomBackRight();

    return result;
}

engine::MarchingCubesCase engine::MarchingCubesCase::mirror() {
    MarchingCubesCase result;

    result.topFrontLeft().isSolid = !topFrontLeft().isSolid;
    result.topFrontRight().isSolid = !topFrontRight().isSolid;
    result.topBackRight().isSolid = !topBackRight().isSolid;
    result.topBackLeft().isSolid = !topBackLeft().isSolid;

    result.bottomFrontLeft().isSolid = !bottomFrontLeft().isSolid;
    result.bottomFrontRight().isSolid = !bottomFrontRight().isSolid;
    result.bottomBackRight().isSolid = !bottomBackRight().isSolid;
    result.bottomBackLeft().isSolid = !bottomBackLeft().isSolid;

    return result;
}

engine::MarchingCubesVertexData engine::MarchingCubesCase::getVertexData(const glm::vec3 &position) {
    constexpr uint8_t X_0_0 = 0b00010000, X_0_5 = 0b00100000, X_1_0 = 0b00110000;
    constexpr uint8_t Y_0_0 = 0b00000100, Y_0_5 = 0b00001000, Y_1_0 = 0b00001100;
    constexpr uint8_t Z_0_0 = 0b00000001, Z_0_5 = 0b00000010, Z_1_0 = 0b00000011;

    constexpr int X_LEFT = 0, X_RIGHT = 1, Y_UP = 2, Y_DOWN = 3, Z_FRONT = 4, Z_BACK = 5;

    /*            0
            o-----X-----o TOP
          3/|         1/|
          X 4         X 5
         /  X 2      /  X MIDDLE
        o-----X-----o   |
        |   |     8 |   |
        7   o-----X-6---o BOTTOM
        X11/        X 9/
        | X         | X
        |/   10     |/
        o-----X-----o
    */

    uint16_t positionCase = 0;
    if (position.x == 0.f - 0.5f)       positionCase = X_0_0;
    else if (position.x == 0.5f - 0.5f) positionCase = X_0_5;
    else if (position.x == 1.0f - 0.5f) positionCase = X_1_0;

    if (position.y == 0.f - 0.5f)       positionCase |= Y_0_0;
    else if (position.y == 0.5f - 0.5f) positionCase |= Y_0_5;
    else if (position.y == 1.0f - 0.5f) positionCase |= Y_1_0;

    if (position.z == 0.f - 0.5f)       positionCase |= Z_0_0;
    else if (position.z == 0.5f - 0.5f) positionCase |= Z_0_5;
    else if (position.z == 1.0f - 0.5f) positionCase |= Z_1_0;

    MarchingCubesVertexData result;
    switch (positionCase) {
    case (X_0_5 | Y_1_0 | Z_1_0): // 0.5 1.0 1.0 [0]
        {
            assert(topFrontLeft().isSolid != topFrontRight().isSolid);
            assert((topFrontLeft().isSolid == true) || (topFrontRight().isSolid == true));
            result.direction = topFrontLeft().isSolid ? X_RIGHT : X_LEFT;
            result.id = topFrontLeft().isSolid ? TOP_FRONT_LEFT_ID : TOP_FRONT_RIGHT_ID;
        }
        break;
    
    case (X_1_0 | Y_1_0 | Z_0_5): // 1.0 1.0 0.5 [1]
        {
            assert(topFrontRight().isSolid != topBackRight().isSolid);
            assert((topFrontRight().isSolid == true) || (topBackRight().isSolid == true));
            result.direction = topFrontRight().isSolid ? Z_BACK : Z_FRONT;
            result.id = topFrontRight().isSolid ? TOP_FRONT_RIGHT_ID : TOP_BACK_RIGHT_ID;
        }
        break;

    case (X_0_5 | Y_1_0 | Z_0_0): // 0.5 1.0 0.0 [2]
        {
            assert(topBackLeft().isSolid != topBackRight().isSolid);
            assert((topBackLeft().isSolid == true) || (topBackRight().isSolid == true));
            result.direction = topBackLeft().isSolid ? X_RIGHT : X_LEFT;
            result.id = topBackLeft().isSolid ? TOP_BACK_LEFT_ID : TOP_BACK_RIGHT_ID;
        }
        break;

    case (X_0_0 | Y_1_0 | Z_0_5): // 0.0 1.0 0.5 [3]
        {
            assert(topFrontLeft().isSolid != topBackLeft().isSolid);
            assert((topFrontLeft().isSolid == true) || (topBackLeft().isSolid == true));
            result.direction = topBackLeft().isSolid ? Z_FRONT : Z_BACK;
            result.id = topFrontLeft().isSolid ? TOP_FRONT_LEFT_ID : TOP_BACK_LEFT_ID;
        }
        break;

    case (X_0_0 | Y_0_5 | Z_1_0): // 0.0 0.5 1.0 [4]
        {
            assert(bottomFrontLeft().isSolid != topFrontLeft().isSolid);
            assert((bottomFrontLeft().isSolid == true) || (topFrontLeft().isSolid == true));
            result.direction = bottomFrontLeft().isSolid ? Y_UP : Y_DOWN;
            result.id = bottomFrontLeft().isSolid ? BOTTOM_FRONT_LEFT_ID : TOP_FRONT_LEFT_ID;
        }
        break;

    case (X_1_0 | Y_0_5 | Z_1_0): // 1.0 0.5 1.0 [5]
        {
            assert(bottomFrontRight().isSolid != topFrontRight().isSolid);
            assert((bottomFrontRight().isSolid == true) || (topFrontRight().isSolid == true));
            result.direction = bottomFrontRight().isSolid ? Y_UP : Y_DOWN;
            result.id = bottomFrontRight().isSolid ? BOTTOM_FRONT_RIGHT_ID : TOP_FRONT_RIGHT_ID;
        }
        break;

    case (X_1_0 | Y_0_5 | Z_0_0): // 1.0 0.5 0.0 [6]
        {
            assert(bottomBackRight().isSolid != topBackRight().isSolid);
            assert((bottomBackRight().isSolid == true) || (topBackRight().isSolid == true));
            result.direction = bottomBackRight().isSolid ? Y_UP : Y_DOWN;
            result.id = bottomBackRight().isSolid ? BOTTOM_BACK_RIGHT_ID : TOP_BACK_RIGHT_ID;
        }
        break;

    case (X_0_0 | Y_0_5 | Z_0_0): // 0.0 0.5 0.0 [7]
        {
            assert(bottomBackLeft().isSolid != topBackLeft().isSolid);
            assert((bottomBackLeft().isSolid == true) || (topBackLeft().isSolid == true));
            result.direction = bottomBackLeft().isSolid ? Y_UP : Y_DOWN;
            result.id = bottomBackLeft().isSolid ? BOTTOM_BACK_LEFT_ID : TOP_BACK_LEFT_ID;
        }
        break;

    case (X_0_5 | Y_0_0 | Z_1_0): // 0.5 0.0 1.0 [8]
        {
            assert(bottomFrontLeft().isSolid != bottomFrontRight().isSolid);
            assert((bottomFrontLeft().isSolid == true) || (bottomFrontRight().isSolid == true));
            result.direction = bottomFrontLeft().isSolid ? X_RIGHT : X_LEFT;
            result.id = bottomFrontLeft().isSolid ? BOTTOM_FRONT_LEFT_ID : BOTTOM_FRONT_RIGHT_ID;
        }
        break;
    
    case (X_1_0 | Y_0_0 | Z_0_5): // 1.0 0.0 0.5 [9]
        {
            assert(bottomFrontRight().isSolid != bottomBackRight().isSolid);
            assert((bottomFrontRight().isSolid == true) || (bottomBackRight().isSolid == true));
            result.direction = bottomFrontRight().isSolid ? Z_BACK : Z_FRONT;
            result.id = bottomFrontRight().isSolid ? BOTTOM_FRONT_RIGHT_ID : BOTTOM_BACK_RIGHT_ID;
        }
        break;

    case (X_0_5 | Y_0_0 | Z_0_0): // 0.5 0.0 0.0 [10]
        {
            assert(bottomBackLeft().isSolid != bottomBackRight().isSolid);
            assert((bottomBackLeft().isSolid == true) || (bottomBackRight().isSolid == true));
            result.direction = bottomBackLeft().isSolid ? X_RIGHT : X_LEFT;
            result.id = bottomBackLeft().isSolid ? BOTTOM_BACK_LEFT_ID : BOTTOM_BACK_RIGHT_ID;
        }
        break;

    case (X_0_0 | Y_0_0 | Z_0_5): // 0.0 0.0 0.5 [11]
        {
            assert(bottomFrontLeft().isSolid != bottomBackLeft().isSolid);
            assert((bottomFrontLeft().isSolid == true) || (bottomBackLeft().isSolid == true));
            result.direction = bottomBackLeft().isSolid ? Z_FRONT : Z_BACK;
            result.id = bottomFrontLeft().isSolid ? BOTTOM_FRONT_LEFT_ID : BOTTOM_BACK_LEFT_ID;
        }
        break;
    
    default:
        std::cout << "ERROR engine::MarchingCubesCase::getOffsetDirection no equal case\n";
        assert(false);
        break;
    }
    
    return result;
}
