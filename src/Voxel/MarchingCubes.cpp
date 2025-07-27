#include "MarchingCubes.h"

engine::MarchingCubes::MarchingCubes() {
    glm::mat4 rotateYaw[4] = {
        glm::yawPitchRoll(glm::radians(0.0f), 0.0f, 0.0f),
        glm::yawPitchRoll(glm::radians(90.0f), 0.0f, 0.0f),
        glm::yawPitchRoll(glm::radians(180.0f), 0.0f, 0.0f),
        glm::yawPitchRoll(glm::radians(270.0f), 0.0f, 0.0f)
    };
    glm::mat4 rotateFront[4] = {
        glm::yawPitchRoll(0.0f, glm::radians(0.f), 0.f),
        glm::yawPitchRoll(0.0f, glm::radians(90.f), 0.f),
        glm::yawPitchRoll(0.0f, glm::radians(180.f), 0.f),
        glm::yawPitchRoll(0.0f, glm::radians(270.f), 0.f)
    };
    glm::mat4 rotateRollRight[4] = {
        glm::yawPitchRoll(0.0f, 0.f, glm::radians(0.f)),
        glm::yawPitchRoll(0.0f, 0.f, glm::radians(-90.f)),
        glm::yawPitchRoll(0.0f, 0.f, glm::radians(-180.f)),
        glm::yawPitchRoll(0.0f, 0.f, glm::radians(-270.f))
    };

    int baseVertex = 0;
    // TYPE 1
    {
        MarchingCubesCase baseCase(0b00000001);
        constexpr int vetrexCount = 3;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;

        // TOP CASES
        // CASE 00000001 = 1   | MIRRORED CASE 11111110 = 254
        // CASE 00000010 = 2   | MIRRORED CASE 11111101 = 253
        // CASE 00000100 = 4   | MIRRORED CASE 11111011 = 251
        // CASE 00001000 = 8   | MIRRORED CASE 11110111 = 247
        // BOTTOM CASES
        // CASE 00010000 = 16  | MIRRORED CASE 11101111 = 239
        // CASE 00100000 = 32  | MIRRORED CASE 11011111 = 223
        // CASE 01000000 = 64  | MIRRORED CASE 10111111 = 191
        // CASE 10000000 = 128 | MIRRORED CASE 01111111 = 127
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 1);
        currentCaseMirrired.setVertexId(1, 3);
        currentCaseMirrired.setVertexId(2, 4);
        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0current = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1current = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2current = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));

                addTriangle(v0current, v1current, v2current, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // Зеркальный случай
                addTriangle(v2current, v1current, v0current, currentCaseMirrired);

                command.first = baseVertex;
                command.count = vetrexCount;
                caseId = currentCaseMirrired.resolve();
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                baseVertex += vetrexCount;
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }
            // Переворачиваем для нижних случаев
            v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
            currentCase = currentCase.rotateFront();
            currentCaseMirrired = currentCaseMirrired.rotateFront();
        }
    }

    // TYPE 2
    {
        MarchingCubesCase baseCase(0b00000011);
        constexpr int vetrexCount = 6;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;

        // TOP CASES
        // CASE 00000011 = 3  | MIRRORED CASE 11111100 = 252
        // CASE 00000110 = 6  | MIRRORED CASE 11111001 = 249
        // CASE 00001100 = 12 | MIRRORED CASE 11110011 = 243
        // CASE 00001001 = 9  | MIRRORED CASE 11110110 = 246
        // VERTICAL CASES
        // CASE 00100010 = 34  | MIRRORED CASE 11011101 = 221
        // CASE 01000100 = 68  | MIRRORED CASE 10111011 = 187
        // CASE 10001000 = 136 | MIRRORED CASE 01110111 = 119
        // CASE 00010001 = 17  | MIRRORED CASE 11101110 = 238
        // BOTTOM CASES
        // CASE 00110000 = 48  | MIRRORED CASE 11001111 = 207
        // CASE 01100000 = 96  | MIRRORED CASE 10011111 = 159
        // CASE 11000000 = 192 | MIRRORED CASE 00111111 = 63
        // CASE 10010000 = 144 | MIRRORED CASE 01101111 = 111
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 1);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 3);
        currentCaseMirrired.setVertexId(1, 2);
        currentCaseMirrired.setVertexId(2, 4);
        currentCaseMirrired.setVertexId(3, 5);

        for (size_t j = 0; j < 3; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));

                addTriangle(v0c, v1c, v3c, currentCase);
                addTriangle(v0c, v3c, v2c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // Зеркальный случай
                addTriangle(v3c, v1c, v0c, currentCaseMirrired);
                addTriangle(v2c, v3c, v0c, currentCaseMirrired);

                command.first = baseVertex;
                command.count = vetrexCount;
                caseId = currentCaseMirrired.resolve();
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                baseVertex += vetrexCount;
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }

            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            currentCase = currentCase.rotateRollRight();
            currentCaseMirrired = currentCaseMirrired.rotateRollRight();
        }
        
    }

    // TYPE 3
    {
        MarchingCubesCase baseCase(0b00000101);
        constexpr int vetrexCount = 6;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;   

        // TOP CASES
        // CASE 00000101 = 5   
        // CASE 00001010 = 10  
        // BOTTOM CASES
        // CASE 10100000 = 160
        // CASE 01010000 = 80
        // MIDDLE CASES
        // CASE 00010010 = 18
        // CASE 00100100 = 36
        // CASE 01001000 = 72
        // CASE 10000001 = 129
        // CASE 00100001 = 33
        // CASE 01000010 = 66
        // CASE 10000100 = 132
        // CASE 00011000 = 24
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 2);

        for (size_t j = 0; j < 2; j++) {
            for (int i = 0; i < 2; ++i) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v4c, v5c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            // Переворачиваем для нижних случаев
            v0 = roundVector(rotateFront[2] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateFront[2] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateFront[2] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateFront[2] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateFront[2] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateFront[2] * glm::vec4(v5, 1.0f));
            currentCase = currentCase.rotateFront().rotateFront();
        }
        v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
        v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
        v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
        v3 = roundVector(rotateFront[1] * glm::vec4(v3, 1.0f));
        v4 = roundVector(rotateFront[1] * glm::vec4(v4, 1.0f));
        v5 = roundVector(rotateFront[1] * glm::vec4(v5, 1.0f));
        currentCase = currentCase.rotateFront();
        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v4c, v5c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }
           
    }

    // TYPE 4
    {
        MarchingCubesCase baseCase(0b11100000);
        constexpr int vetrexCount = 9;
        glm::vec3 v0 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v4 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;

        // BOTTOM CASES
        // CASE 11100000 = 224 | MIRRORED CASE 00011111 = 31
        // CASE 11010000 = 208 | MIRRORED CASE 00101111 = 47
        // CASE 10110000 = 176 | MIRRORED CASE 01001111 = 79
        // CASE 01110000 = 112 | MIRRORED CASE 10001111 = 143
        // MIDDLE CASES
        // CASE 10011000 = 152 | MIRRORED CASE 10001111 = 103
        // CASE 00110001 = 49  | MIRRORED CASE 10001111 = 206
        // CASE 01100010 = 98  | MIRRORED CASE 10001111 = 157
        // CASE 11000100 = 196 | MIRRORED CASE 10001111 = 59
        //
        // CASE 10001001 = 137 | MIRRORED CASE 01110110 = 118
        // CASE 00010011 = 19  | MIRRORED CASE 11101100 = 236
        // CASE 00100110 = 38  | MIRRORED CASE 11011001 = 217
        // CASE 01001100 = 76  | MIRRORED CASE 10110011 = 179
        // TOP CASES
        // CASE 00001110 = 14  | MIRRORED CASE 11110001 = 241
        // CASE 00001101 = 13  | MIRRORED CASE 11110010 = 242
        // CASE 00001011 = 11  | MIRRORED CASE 11110100 = 244
        // CASE 00000111 = 7   | MIRRORED CASE 00000111 = 248 35 220 185 115
        //
        // CASE 00000111 = 35   | MIRRORED CASE 00000111 = 220
        // CASE 00000111 = 70   | MIRRORED CASE 00000111 = 185
        // CASE 00000111 = 140  | MIRRORED CASE 00000111 = 115
        // CASE 00000111 = 25   | MIRRORED CASE 00000111 = 230
        //
        // CASE 00000111 = 50   | MIRRORED CASE 00000111 = 205
        // CASE 00000111 = 100  | MIRRORED CASE 00000111 = 155
        // CASE 00000111 = 200  | MIRRORED CASE 00000111 = 55
        // CASE 00000111 = 145  | MIRRORED CASE 00000111 = 110
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 7);
        currentCase.setVertexId(1, 6);
        currentCase.setVertexId(2, 5);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 4);
        currentCaseMirrired.setVertexId(1, 3);
        currentCaseMirrired.setVertexId(2, 2);
        currentCaseMirrired.setVertexId(3, 1);

        for (int j = 0; j < 6; j++) {
            for (int i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v4c, v0c, v2c, currentCase);
                addTriangle(v3c, v4c, v2c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // зеркальный случай
                addTriangle(v2c, v1c, v0c, currentCaseMirrired);
                addTriangle(v2c, v0c, v4c, currentCaseMirrired);
                addTriangle(v2c, v4c, v3c, currentCaseMirrired);

                command.first = baseVertex;
                command.count = vetrexCount;
                caseId = currentCaseMirrired.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                baseVertex += vetrexCount;
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }
            if (j == 0 || j == 2) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
                currentCase = currentCase.rotateRollRight();
                currentCaseMirrired = currentCaseMirrired.rotateRollRight();
            }
            else if (j == 1 || j == 3) {
                v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateFront[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateFront[1] * glm::vec4(v4, 1.0f));
                currentCase = currentCase.rotateFront();
                currentCaseMirrired = currentCaseMirrired.rotateFront();
            }
            else if (j == 4) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
                currentCase = currentCase.rotateRollRight();
                currentCaseMirrired = currentCaseMirrired.rotateRollRight();
            }
        }

    }
    
    // TYPE 5
    {
        MarchingCubesCase baseCase(0b11110000);
        constexpr int vetrexCount = 6;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;

        // TOP CASE    11110000 = 240 | BOTTOM CASE 00001111 = 15
        // RIGHT CASE  10011001 = 153 | LEFT CASE   01100110 = 102
        // BACK CASE   00110011 = 51  | FRONT CASE  11001100 = 204
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 7);
        currentCase.setVertexId(1, 6);
        currentCase.setVertexId(2, 5);
        currentCase.setVertexId(3, 4);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 0);
        currentCaseMirrired.setVertexId(1, 1);
        currentCaseMirrired.setVertexId(2, 2);
        currentCaseMirrired.setVertexId(3, 3);

        for (size_t i = 0; i < 3; i++) {
            addTriangle(v0, v1, v2, currentCase);
            addTriangle(v0, v2, v3, currentCase);

            DrawArraysIndirectCommand command;
            command.first = baseVertex;
            command.count = vetrexCount;
            int caseId = currentCase.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

            baseVertex += vetrexCount;

            // зеркальный случай
            addTriangle(v2, v1, v0, currentCaseMirrired);
            addTriangle(v3, v2, v0, currentCaseMirrired);

            command.first = baseVertex;
            command.count = vetrexCount;
            caseId = currentCaseMirrired.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

            baseVertex += vetrexCount;

            if (i == 0) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                currentCase = currentCase.rotateRollRight();
                currentCaseMirrired = currentCaseMirrired.rotateRollRight();
            }
            else if (i == 1) {
                v0 = roundVector(rotateYaw[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateYaw[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateYaw[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateYaw[1] * glm::vec4(v3, 1.0f));
                currentCase = currentCase.rotateRight();
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }
            
        }
        
    }

    // TYPE 6
    // ВОЗМОЖНЫЙ ИСТОЧНИК ПРОБЛЕМ!
    // НИЖНИЕ СЛУЧАИ В ВОКСЕЛЯХ ЯВЛЯЮТСЯ ЗЕРКАЛЬНЫМИ ДЛЯ ВЕРНИХ СЛУЧАЕВ
    // НО ВРАЩЕНИЕ ГЕОМЕТРИИ С НИЖНИХ В ВВЕРХНИЕ ДАЁТ РАЗНЫЙ РЕЗУЛЬТАТ ДЛЯ МЕША В
    // СРАВНЕНИИ С РЕЗУЛЬТАТОМ МЕША ДЛЯ ЗЕРКАЛЬНЫХ НИЖНИХ СЛУЧАЕВ.
    // ИЗ-ЗА ЭТОГО ТАК ЖЕ ПОД ВОПРОСОМ СТАНОВИТСЯ ПРАВИЛЬНОСТЬ ЗЕРКАЛЬНЫХ СРЕДНИХ СЛУЧАЕВ
    //
    // ПРОТЕСТИРОВАВ НА ГЛАЗ ОТЗЕРКАЛИВАНИЕ НИЖНИХ СЛУЧАЕВ ВЫГЛЯДЯТ БОЛЕЕ КОРРЕКТНЫМИ
    // НО ЕЩЁ НУЖНЫ ТЕСТЫ, КОГДА БУДЕТ ГОТОВ СБОРЩИК ЧАНКОВ
    {
        MarchingCubesCase baseCase(0b11100001);
        constexpr int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v4 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v6 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v7 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;

        // BOTTOM CASES
        // CASE 11100001 = 225 | MIRRORED CASE 00011110 = 30
        // CASE 11100001 = 210 | MIRRORED CASE 00011110 = 45
        // CASE 11100001 = 180 | MIRRORED CASE 00011110 = 75
        // CASE 11100001 = 120 | MIRRORED CASE 00011110 = 135
        // MIDDLE CASES
        // CASE 11100001 = 154 | MIRRORED CASE 00011110 = 101
        // CASE 11100001 = 53  | MIRRORED CASE 00011110 = 202
        // CASE 11100001 = 106 | MIRRORED CASE 00011110 = 149
        // CASE 11100001 = 197 | MIRRORED CASE 00011110 = 58
        // CASE 11100001 = 169 | MIRRORED CASE 00011110 = 86
        // CASE 11100001 = 83  | MIRRORED CASE 00011110 = 172
        // CASE 11100001 = 166 | MIRRORED CASE 00011110 = 89
        // CASE 11100001 = 92  | MIRRORED CASE 00011110 = 163
        // TOP CASES (NOT USED)
        // CASE 11100001 = 225 | MIRRORED CASE 00011110 = 30
        // CASE 11100001 = 225 | MIRRORED CASE 00011110 = 30
        // CASE 11100001 = 225 | MIRRORED CASE 00011110 = 30
        // CASE 11100001 = 225 | MIRRORED CASE 00011110 = 30
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 7);
        currentCase.setVertexId(1, 6);
        currentCase.setVertexId(2, 5);
        currentCase.setVertexId(3, 0);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 4);
        currentCaseMirrired.setVertexId(1, 3);
        currentCaseMirrired.setVertexId(2, 2);
        currentCaseMirrired.setVertexId(3, 1);

        for (int j = 0; j < 6; j++) { // если 3, то закоментируй условие для зеркального случая, если 4 то убрать
            for (int i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));
                glm::vec3 v7c = roundVector(rotateYaw[i] * glm::vec4(v7, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v4c, v0c, v2c, currentCase);
                addTriangle(v3c, v4c, v2c, currentCase);
                addTriangle(v5c, v6c, v7c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // зеркальный случай
                /*if (j != 0 && j != 3) {
                    addTriangle(v2c, v1c, v0c, currentCaseMirrired, v2tex, v1tex, v0tex);
                    addTriangle(v2c, v0c, v4c, currentCaseMirrired, v2tex, v0tex, v4tex);
                    addTriangle(v2c, v4c, v3c, currentCaseMirrired, v2tex, v4tex, v3tex);
                    addTriangle(v7c, v6c, v5c, currentCaseMirrired, v7tex, v6tex, v5tex);

                    command.first = baseVertex;
                    command.count = vetrexCount;
                    caseId = currentCaseMirrired.resolve();
                    assert(m_caseDrawCommand[caseId].count == 0);
                    m_caseDrawCommand[caseId] = command;
                    m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                    baseVertex += vetrexCount;
                    currentCaseMirrired = currentCaseMirrired.rotateRight();
                }*/
            }
            if (j == 0 || j == 2 || j == 4) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
                v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
                v7 = roundVector(rotateRollRight[1] * glm::vec4(v7, 1.0f));
                currentCase = currentCase.rotateRollRight();
                currentCaseMirrired = currentCaseMirrired.rotateRollRight();
            }
            else if (j == 1 || j == 3 || j == 5) {
                v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateFront[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateFront[1] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateFront[1] * glm::vec4(v5, 1.0f));
                v6 = roundVector(rotateFront[1] * glm::vec4(v6, 1.0f));
                v7 = roundVector(rotateFront[1] * glm::vec4(v7, 1.0f));
                currentCase = currentCase.rotateFront();
                currentCaseMirrired = currentCaseMirrired.rotateFront();
            }
        }

    }

    // TYPE 7
    {
        MarchingCubesCase baseCase(0b00010100);
        constexpr int vetrexCount = 6;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;

        // CASE 00010100 = 20  | MIRRORED CASE 11101011 = 235
        // CASE 00101000 = 40  | MIRRORED CASE 11010111 = 215
        // CASE 01000001 = 65  | MIRRORED CASE 10111110 = 190
        // CASE 10000010 = 130 | MIRRORED CASE 01111101 = 125
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 2);
        currentCase.setVertexId(1, 4);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(4, 0);
        currentCaseMirrired.setVertexId(5, 6);
        currentCaseMirrired.setVertexId(0, 5);
        currentCaseMirrired.setVertexId(1, 3);
        currentCaseMirrired.setVertexId(2, 7);
        currentCaseMirrired.setVertexId(3, 1);

        for (int i = 0; i < 4; i++) {
            glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
            glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
            glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
            glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
            glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
            glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

            addTriangle(v0c, v1c, v2c, currentCase);
            addTriangle(v3c, v4c, v5c, currentCase);

            DrawArraysIndirectCommand command;
            command.first = baseVertex;
            command.count = vetrexCount;
            int caseId = currentCase.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

            baseVertex += vetrexCount;
            currentCase = currentCase.rotateRight();

            // зеркальный случай
            addTriangle(v2c, v1c, v0c, currentCaseMirrired);
            addTriangle(v5c, v4c, v3c, currentCaseMirrired);

            command.first = baseVertex;
            command.count = vetrexCount;
            caseId = currentCaseMirrired.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

            baseVertex += vetrexCount;
            currentCaseMirrired = currentCaseMirrired.rotateRight();
        }
    }

    // TYPE 8 A
    {
        MarchingCubesCase baseCase(0b01000011);
        constexpr int vetrexCount = 9;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;

        glm::vec3 v4 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.5f, 0.0f, 0.0f) - 0.5f;
        glm::vec3 v6 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;

        // TOP CASES
        // CASE 01000011 = 67
        // CASE 10000110 = 134
        // CASE 00011100 = 28
        // CASE 00101001 = 41
        // VERTICAL CASES
        // CASE 10100010 = 162
        // CASE 00000011 = 84
        // CASE 00000011 = 168
        // CASE 00000011 = 81
        // BOTTOM CASES
        // CASE 00111000 = 56
        // CASE 00000011 = 97
        // CASE 00000011 = 194
        // CASE 00000011 = 148
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 1);
        currentCase.setVertexId(2, 6);

        for (size_t j = 0; j < 3; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));

                addTriangle(v0c, v1c, v3c, currentCase);
                addTriangle(v0c, v3c, v2c, currentCase);
                addTriangle(v4c, v5c, v6c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }

            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }
    }

    // TYPE 8 B
    {
        MarchingCubesCase baseCase(0b10000011);
        constexpr int vetrexCount = 9;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;

        glm::vec3 v4 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v6 = glm::vec3(0.5f, 0.0f, 0.0f) - 0.5f;

        // TOP CASES
        // CASE 10000011 = 131
        // CASE 10000110 = 22
        // CASE 00011100 = 44
        // CASE 00101001 = 73
        // VERTICAL CASES
        // CASE 10100010 = 42
        // CASE 00000011 = 69
        // CASE 00000011 = 138
        // CASE 00000011 = 21
        // BOTTOM CASES
        // CASE 00111000 = 52
        // CASE 00000011 = 104
        // CASE 00000011 = 193
        // CASE 00000011 = 146
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 1);
        currentCase.setVertexId(2, 7);

        for (size_t j = 0; j < 3; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));

                addTriangle(v0c, v1c, v3c, currentCase);
                addTriangle(v0c, v3c, v2c, currentCase);
                addTriangle(v4c, v5c, v6c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }

            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }
    }

    // TYPE 9
    {
        MarchingCubesCase baseCase(0b00100101);
        constexpr int vetrexCount = 9;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v6 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v7 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v8 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;

        // TOP CASES
        // CASE 00100101 = 37
        // CASE 00000000 = 74
        // CASE 00000000 = 133
        // CASE 00000000 = 26
        // MIDDLE CASES A
        // CASE 00000000 = 82
        // CASE 00000000 = 164
        // CASE 00000000 = 88
        // CASE 00000000 = 161
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 2);
        currentCase.setVertexId(2, 5);

        for (int j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));
                glm::vec3 v7c = roundVector(rotateYaw[i] * glm::vec4(v7, 1.0f));
                glm::vec3 v8c = roundVector(rotateYaw[i] * glm::vec4(v8, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v4c, v5c, currentCase);
                addTriangle(v6c, v7c, v8c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
            v7 = roundVector(rotateRollRight[1] * glm::vec4(v7, 1.0f));
            v8 = roundVector(rotateRollRight[1] * glm::vec4(v8, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }
        
    }

    // TYPE 10
    {
        MarchingCubesCase baseCase(0b01010101);
        constexpr int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;

        glm::vec3 v4 = glm::vec3(0.5f, 0.0f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v6 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v7 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;

        // CASES
        // CASE 01010101 = 85
        // CASE 01010101 = 170
        // CASE 01010101 = 195
        // CASE 01010101 = 150
        // CASE 01010101 = 60
        // CASE 01010101 = 105
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 2);
        currentCase.setVertexId(2, 4);
        currentCase.setVertexId(3, 6);

        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                if (j == 0 && i == 2) break;

                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));
                glm::vec3 v7c = roundVector(rotateYaw[i] * glm::vec4(v7, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v2c, v1c, currentCase);
                addTriangle(v4c, v5c, v6c, currentCase);
                addTriangle(v4c, v6c, v7c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            if (j == 0) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
                v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
                v7 = roundVector(rotateRollRight[1] * glm::vec4(v7, 1.0f));
                currentCase = currentCase.rotateRollRight();
            }
        }
        
    }

    // TYPE 11
    {
        MarchingCubesCase baseCase(0b11010100);
        constexpr int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v5 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;

        // BOTTOM CASES
        // CASE 11010100 = 212 | MIRRORED CASE 11101011 = 43
        // CASE 11010100 = 184 | MIRRORED CASE 11101011 = 71
        // CASE 11010100 = 113 | MIRRORED CASE 11101011 = 142
        // CASE 11010100 = 226 | MIRRORED CASE 11101011 = 29
        // MIDDLE CASES
        // CASE 11010100 = 201 | MIRRORED CASE 11101011 = 54
        // CASE 11010100 = 147 | MIRRORED CASE 11101011 = 108
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 2);
        currentCase.setVertexId(1, 4);
        currentCase.setVertexId(2, 6);
        currentCase.setVertexId(3, 7);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 0);
        currentCaseMirrired.setVertexId(1, 1);
        currentCaseMirrired.setVertexId(2, 3);
        currentCaseMirrired.setVertexId(3, 5);

        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                if (j == 1 && i == 2) break;

                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v0c, v4c, currentCase);
                addTriangle(v0c, v2c, v4c, currentCase);
                addTriangle(v4c, v2c, v5c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // зеркальный случай
                addTriangle(v2c, v1c, v0c, currentCaseMirrired);
                addTriangle(v4c, v0c, v3c, currentCaseMirrired);
                addTriangle(v4c, v2c, v0c, currentCaseMirrired);
                addTriangle(v5c, v2c, v4c, currentCaseMirrired);

                command.first = baseVertex;
                command.count = vetrexCount;
                caseId = currentCaseMirrired.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                baseVertex += vetrexCount;
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }
            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            currentCase = currentCase.rotateRollRight();
            currentCaseMirrired = currentCaseMirrired.rotateRollRight();
        }

    }

    // TYPE 12
    {
        MarchingCubesCase baseCase(0b01011010);
        int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;

        glm::vec3 v3 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;

        glm::vec3 v6 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v7 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v8 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;

        glm::vec3 v9 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v10 = glm::vec3(0.5f, 0.0f, 0.0f) - 0.5f;
        glm::vec3 v11 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;

        // CASES
        // CASE 01011010 = 90 | MIRRORED CASE 0b01011010 = 165
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 1);
        currentCase.setVertexId(1, 3);
        currentCase.setVertexId(2, 4);
        currentCase.setVertexId(3, 6);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 0);
        currentCaseMirrired.setVertexId(1, 2);
        currentCaseMirrired.setVertexId(2, 5);
        currentCaseMirrired.setVertexId(3, 7);

        {

            addTriangle(v0, v1, v2, currentCase);
            addTriangle(v3, v4, v5, currentCase);
            addTriangle(v6, v7, v8, currentCase);
            addTriangle(v9, v10, v11, currentCase);

            DrawArraysIndirectCommand command;
            command.first = baseVertex;
            command.count = vetrexCount;
            int caseId = currentCase.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCase.copyVertecesId();
            
            baseVertex += vetrexCount;
            currentCase = currentCase.rotateRight();

            int i = 1;
            v0 = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));
            v7 = roundVector(rotateYaw[i] * glm::vec4(v7, 1.0f));
            v8 = roundVector(rotateYaw[i] * glm::vec4(v8, 1.0f));
            v9 = roundVector(rotateYaw[i] * glm::vec4(v9, 1.0f));
            v10 = roundVector(rotateYaw[i] * glm::vec4(v10, 1.0f));
            v11 = roundVector(rotateYaw[i] * glm::vec4(v11, 1.0f));

            // зеркальный случай
            /*addTriangle(v2, v1, v0, currentCase);
            addTriangle(v5, v4, v3, currentCase);
            addTriangle(v8, v7, v6, currentCase);
            addTriangle(v11, v10, v9, currentCase);*/
            addTriangle(v0, v1, v2, currentCase);
            addTriangle(v3, v4, v5, currentCase);
            addTriangle(v6, v7, v8, currentCase);
            addTriangle(v9, v10, v11, currentCase);

            command.first = baseVertex;
            command.count = vetrexCount;
            caseId = currentCaseMirrired.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

            baseVertex += vetrexCount;
            currentCaseMirrired = currentCaseMirrired.rotateRight();
        }
    }

    // TYPE 13
    {
        MarchingCubesCase baseCase(0b11011000);
        int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;

        // CASES
        // CASE 11011000 = 216 | MIRRORED CASE 11011000 = 39
        // CASE 11010100 = 177 | MIRRORED CASE 11011000 = 78
        // CASE 11011000 = 114 | MIRRORED CASE 11011000 = 141
        // CASE 11011000 = 228 | MIRRORED CASE 11011000 = 27
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 3);
        currentCase.setVertexId(1, 4);
        currentCase.setVertexId(2, 6);
        currentCase.setVertexId(3, 7);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 0);
        currentCaseMirrired.setVertexId(1, 1);
        currentCaseMirrired.setVertexId(2, 2);
        currentCaseMirrired.setVertexId(3, 5);

        for (size_t i = 0; i < 4; i++) {
            glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
            glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
            glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
            glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
            glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
            glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

            addTriangle(v0c, v1c, v2c, currentCase);
            addTriangle(v2c, v1c, v3c, currentCase);
            addTriangle(v2c, v3c, v4c, currentCase);
            addTriangle(v4c, v3c, v5c, currentCase);

            DrawArraysIndirectCommand command;
            command.first = baseVertex;
            command.count = vetrexCount;
            int caseId = currentCase.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

            baseVertex += vetrexCount;
            currentCase = currentCase.rotateRight();

            // зеркальный случай
            addTriangle(v2c, v1c, v0c, currentCaseMirrired);
            addTriangle(v3c, v1c, v2c, currentCaseMirrired);
            addTriangle(v4c, v3c, v2c, currentCaseMirrired);
            addTriangle(v5c, v3c, v4c, currentCaseMirrired);

            command.first = baseVertex;
            command.count = vetrexCount;
            caseId = currentCaseMirrired.resolve();
            assert(m_caseDrawCommand[caseId].count == 0);
            m_caseDrawCommand[caseId] = command;
            m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

            baseVertex += vetrexCount;
            currentCaseMirrired = currentCaseMirrired.rotateRight();
        }
    }

    // TYPE 14
    {
        MarchingCubesCase baseCase(0b11101000);
        int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v1 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;

        // BOTTOM CASES
        // CASE 11101000 = 232 | MIRRORED CASE 00010111 = 23
        // CASE 11101000 = 209 | MIRRORED CASE 00010111 = 46
        // CASE 11101000 = 178 | MIRRORED CASE 00010111 = 77
        // CASE 11101000 = 116 | MIRRORED CASE 00010111 = 139
        // MIDDLE CASES
        // CASE 11101000 = 156 | MIRRORED CASE 00010111 = 99
        // CASE 11101000 = 57  | MIRRORED CASE 00010111 = 198
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 3);
        currentCase.setVertexId(1, 5);
        currentCase.setVertexId(2, 6);
        currentCase.setVertexId(3, 7);
        MarchingCubesCase currentCaseMirrired = baseCase.mirror();
        currentCaseMirrired.setVertexId(0, 0);
        currentCaseMirrired.setVertexId(1, 1);
        currentCaseMirrired.setVertexId(2, 2);
        currentCaseMirrired.setVertexId(3, 4);

        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                if (j == 1 && i == 2) break;

                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v3c, v4c, v1c, currentCase);
                addTriangle(v1c, v4c, v2c, currentCase);
                addTriangle(v4c, v5c, v2c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();

                // зеркальный случай
                /*addTriangle(v2c, v1c, v0c, currentCaseMirrired, v2tex, v1tex, v0tex);
                addTriangle(v4c, v0c, v3c, currentCaseMirrired, v4tex, v0tex, v3tex);
                addTriangle(v4c, v2c, v0c, currentCaseMirrired, v4tex, v2tex, v0tex);
                addTriangle(v5c, v2c, v4c, currentCaseMirrired, v5tex, v2tex, v4tex);*/
                addTriangle(v2c, v1c, v0c, currentCaseMirrired);
                addTriangle(v1c, v4c, v3c, currentCaseMirrired);
                addTriangle(v2c, v4c, v1c, currentCaseMirrired);
                addTriangle(v2c, v5c, v4c, currentCaseMirrired);

                command.first = baseVertex;
                command.count = vetrexCount;
                caseId = currentCaseMirrired.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCaseMirrired.copyVertecesId();

                baseVertex += vetrexCount;
                currentCaseMirrired = currentCaseMirrired.rotateRight();
            }
            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            currentCase = currentCase.rotateRollRight();
            currentCaseMirrired = currentCaseMirrired.rotateRollRight();
        }

    }

    // TYPE 15 (MIRRORED TYPE 9)
    {
        MarchingCubesCase baseCase(0b11011010);
        int vetrexCount = 15;
        glm::vec3 v0 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v6 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v7 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;
        glm::vec3 v8 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;

        // CASES
        // CASE 11011010 = 218
        // CASE 11011010 = 181
        // CASE 11011010 = 122
        // CASE 11011010 = 229
        // CASE 11011010 = 173
        // CASE 11011010 = 91
        // CASE 11011010 = 167
        // CASE 11011010 = 94
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 1);
        currentCase.setVertexId(1, 3);
        currentCase.setVertexId(2, 4);
        currentCase.setVertexId(3, 6);

        for (size_t j = 0; j < 2; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));
                glm::vec3 v7c = roundVector(rotateYaw[i] * glm::vec4(v7, 1.0f));
                glm::vec3 v8c = roundVector(rotateYaw[i] * glm::vec4(v8, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v2c, v1c, v3c, currentCase);
                addTriangle(v2c, v3c, v4c, currentCase);
                addTriangle(v4c, v3c, v5c, currentCase);
                addTriangle(v6c, v7c, v8c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            // Переворачиваем для нижних случаев
            v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateFront[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateFront[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateFront[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateFront[1] * glm::vec4(v6, 1.0f));
            v7 = roundVector(rotateFront[1] * glm::vec4(v7, 1.0f));
            v8 = roundVector(rotateFront[1] * glm::vec4(v8, 1.0f));
            currentCase = currentCase.rotateFront();
        }
    }

    // TYPE 16 A (MIRRORED TYPE 8 A)
    {
        MarchingCubesCase baseCase(0b11001011);
        int vetrexCount = 15;
        glm::vec3 v0 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v6 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;

        // TOP CASES
        // CASE 11001011 = 203
        // CASE 11001011 = 151
        // CASE 11001011 = 62
        // CASE 11001011 = 109
        // VERTICAL CASES
        // CASE 11001011 = 174
        // CASE 11001011 = 93
        // CASE 11001011 = 171
        // CASE 11001011 = 87
        // BOTTOM CASES
        // CASE 11001011 = 124
        // CASE 11001011 = 233
        // CASE 11001011 = 211
        // CASE 11001011 = 182
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 1);
        currentCase.setVertexId(2, 6);
        currentCase.setVertexId(3, 7);
        currentCase.setVertexId(4, 3);

        for (size_t j = 0; j < 3; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));

                addTriangle(v2c, v1c, v0c, currentCase);
                addTriangle(v3c, v2c, v0c, currentCase);
                addTriangle(v2c, v4c, v1c, currentCase);
                addTriangle(v2c, v6c, v4c, currentCase);
                addTriangle(v2c, v5c, v6c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }

            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }

    }

    // TYPE 16 A (MIRRORED TYPE 8 B)
    {
        MarchingCubesCase baseCase(0b11000111);
        int vetrexCount = 15;
        glm::vec3 v0 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(1.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 0.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(0.0f, 0.5f, 0.0f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v6 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;

        // TOP CASES
        // CASE 11000111 = 199
        // CASE 11000111 = 158
        // CASE 11000111 = 61
        // CASE 11000111 = 107
        // VERTICAL CASES
        // CASE 11000111 = 234
        // CASE 11000111 = 213
        // CASE 11000111 = 186
        // CASE 11000111 = 117
        // BOTTOM CASES
        // CASE 11000111 = 188
        // CASE 11000111 = 121
        // CASE 11000111 = 227
        // CASE 11000111 = 214
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 1);
        currentCase.setVertexId(2, 6);
        currentCase.setVertexId(3, 7);
        currentCase.setVertexId(4, 2);

        for (size_t j = 0; j < 3; j++) {
            for (size_t i = 0; i < 4; i++) {
                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));
                glm::vec3 v6c = roundVector(rotateYaw[i] * glm::vec4(v6, 1.0f));

                addTriangle(v0c, v1c, v2c, currentCase);
                addTriangle(v0c, v2c, v3c, currentCase);
                addTriangle(v1c, v4c, v2c, currentCase);
                addTriangle(v4c, v6c, v2c, currentCase);
                addTriangle(v6c, v5c, v2c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }

            v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
            v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
            v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
            v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
            v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
            v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
            v6 = roundVector(rotateRollRight[1] * glm::vec4(v6, 1.0f));
            currentCase = currentCase.rotateRollRight();
        }

    }

    // TYPE 17
    {
        MarchingCubesCase baseCase(0b11101101);
        constexpr int vetrexCount = 12;
        glm::vec3 v0 = glm::vec3(0.0f, 0.0f, 0.5f) - 0.5f;
        glm::vec3 v1 = glm::vec3(0.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v2 = glm::vec3(0.5f, 1.0f, 1.0f) - 0.5f;
        glm::vec3 v3 = glm::vec3(1.0f, 1.0f, 0.5f) - 0.5f;
        glm::vec3 v4 = glm::vec3(1.0f, 0.5f, 1.0f) - 0.5f;
        glm::vec3 v5 = glm::vec3(0.5f, 0.0f, 1.0f) - 0.5f;

        // TOP CASES
        // CASE 11101101 = 245
        // CASE 11101101 = 250
        // BOTTOM CASES
        // CASE 11101101 = 175
        // CASE 11101101 = 95
        // MIDDLE CASES
        // CASE 11101101 = 237
        // CASE 11101101 = 219
        // CASE 11101101 = 183
        // CASE 11101101 = 126
        // CASE 11101101 = 222
        // CASE 11101101 = 189
        // CASE 11101101 = 123
        // CASE 11101101 = 231
        MarchingCubesCase currentCase = baseCase;
        currentCase.setVertexId(0, 0);
        currentCase.setVertexId(1, 2);
        currentCase.setVertexId(2, 5);
        currentCase.setVertexId(3, 7);

        for (size_t j = 0; j < 4; j++) {
            for (size_t i = 0; i < 4; i++) {
                if (j >= 2 && i >= 2) break;

                glm::vec3 v0c = roundVector(rotateYaw[i] * glm::vec4(v0, 1.0f));
                glm::vec3 v1c = roundVector(rotateYaw[i] * glm::vec4(v1, 1.0f));
                glm::vec3 v2c = roundVector(rotateYaw[i] * glm::vec4(v2, 1.0f));
                glm::vec3 v3c = roundVector(rotateYaw[i] * glm::vec4(v3, 1.0f));
                glm::vec3 v4c = roundVector(rotateYaw[i] * glm::vec4(v4, 1.0f));
                glm::vec3 v5c = roundVector(rotateYaw[i] * glm::vec4(v5, 1.0f));

                addTriangle(v0c, v2c, v1c, currentCase);
                addTriangle(v0c, v3c, v2c, currentCase);
                addTriangle(v0c, v4c, v3c, currentCase);
                addTriangle(v0c, v5c, v4c, currentCase);

                DrawArraysIndirectCommand command;
                command.first = baseVertex;
                command.count = vetrexCount;
                int caseId = currentCase.resolve();
                assert(m_caseDrawCommand[caseId].count == 0);
                m_caseDrawCommand[caseId] = command;
                m_caseVertecesIds[caseId] = currentCase.copyVertecesId();

                baseVertex += vetrexCount;
                currentCase = currentCase.rotateRight();
            }
            if (j == 0) {
                v0 = roundVector(rotateRollRight[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateRollRight[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateRollRight[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateRollRight[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateRollRight[1] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateRollRight[1] * glm::vec4(v5, 1.0f));
                currentCase = currentCase.rotateRollRight();
            }
            else if (j == 1) {
                v0 = roundVector(rotateFront[1] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateFront[1] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateFront[1] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateFront[1] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateFront[1] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateFront[1] * glm::vec4(v5, 1.0f));
                currentCase = currentCase.rotateFront();
            }
            else if (j == 2) {
                v0 = roundVector(rotateFront[2] * glm::vec4(v0, 1.0f));
                v1 = roundVector(rotateFront[2] * glm::vec4(v1, 1.0f));
                v2 = roundVector(rotateFront[2] * glm::vec4(v2, 1.0f));
                v3 = roundVector(rotateFront[2] * glm::vec4(v3, 1.0f));
                v4 = roundVector(rotateFront[2] * glm::vec4(v4, 1.0f));
                v5 = roundVector(rotateFront[2] * glm::vec4(v5, 1.0f));
                currentCase = currentCase.rotateFront().rotateFront();
            }
        }
        
    }

    /* // Проверка на неучтённые варианты
    int missedCount = 0;
    for (int i = 1; i < 255; i++) {
        if (m_caseDrawCommand[i].count == 0) {
            std::cout << i << " missed\n";
            missedCount++;
        }
    }
    std::cout << "Missed: " << missedCount << "\n";*/

    m_triangleDataSSBO.init(m_trianglesData.size(), BufferUsage::STATIC_DRAW);
    m_triangleDataSSBO.pushData(&m_trianglesData[0], m_trianglesData.size());

    glGenVertexArrays(1, &m_VAO);

    #ifndef NDEBUG
    int errors = 0;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        errors++;
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "engine::MarchingCubes::MarchingCubes() " << error << std::endl;
    }
    #endif
}

engine::MarchingCubes::~MarchingCubes() {
    glDeleteVertexArrays(1, &m_VAO);
}

glm::vec3 engine::MarchingCubes::roundVector(glm::vec4 vec) {
    //https://ru.stackoverflow.com/questions/209649/Округление-до-n-знаков-после-запятой-в-с
    constexpr float powerOfTen = 100.0;

    float x = std::floor(vec.x * powerOfTen + 0.5) / powerOfTen;
    float y = std::floor(vec.y * powerOfTen + 0.5) / powerOfTen;
    float z = std::floor(vec.z * powerOfTen + 0.5) / powerOfTen;

    return glm::vec3(x, y, z);
}

inline glm::vec3 engine::MarchingCubes::getNormal(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    return glm::normalize(glm::cross(ab, ac));
}

inline void engine::MarchingCubes::addTriangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, MarchingCubesCase& currentCase) {
    // X_LEFT = 0, X_RIGHT = 1, Y_UP = 2, Y_DOWN = 3, Z_FRONT = 4, Z_BACK = 5;
    constexpr float cornerOffset = 0.5f;
    constexpr glm::vec3 directionMul[6] = { glm::vec3(-1.0, 0.0, 0.0), 
                                            glm::vec3(1.0, 0.0, 0.0), 
                                            glm::vec3(0.0, 1.0, 0.0), 
                                            glm::vec3(0.0, -1.0, 0.0), 
                                            glm::vec3(0.0, 0.0, 1.0), 
                                            glm::vec3(0.0, 0.0, -1.0) 
    };

    MarchingCubesVertexData v0data = currentCase.getVertexData(v0);
    MarchingCubesVertexData v1data = currentCase.getVertexData(v1);
    MarchingCubesVertexData v2data = currentCase.getVertexData(v2);

    VoxelTriangleData triangleData;
    for (int vX = 0; vX < 8; vX++) {
        glm::vec4 v0vertexSized(v0.x + cornerOffset + OFFSETS_STRENGTH[vX] * directionMul[v0data.direction].x, 
                                v0.y + cornerOffset + OFFSETS_STRENGTH[vX] * directionMul[v0data.direction].y, 
                                v0.z + cornerOffset + OFFSETS_STRENGTH[vX] * directionMul[v0data.direction].z, 0.f);

        for (int vY = 0; vY < 8; vY++) {
            glm::vec4 v1vertexSized(v1.x + cornerOffset + OFFSETS_STRENGTH[vY] * directionMul[v1data.direction].x, 
                                    v1.y + cornerOffset + OFFSETS_STRENGTH[vY] * directionMul[v1data.direction].y, 
                                    v1.z + cornerOffset + OFFSETS_STRENGTH[vY] * directionMul[v1data.direction].z, 0.f);

            for (int vZ = 0; vZ < 8; vZ++) {
                glm::vec4 v2vertexSized(v2.x + cornerOffset + OFFSETS_STRENGTH[vZ] * directionMul[v2data.direction].x, 
                                        v2.y + cornerOffset + OFFSETS_STRENGTH[vZ] * directionMul[v2data.direction].y, 
                                        v2.z + cornerOffset + OFFSETS_STRENGTH[vZ] * directionMul[v2data.direction].z, 0.f);
                
                glm::vec3 edge_A = v1vertexSized - v0vertexSized;
                glm::vec3 edge_B = v2vertexSized - v0vertexSized;
                glm::vec3 normal = glm::normalize(glm::cross(edge_A, edge_B));

                // get from https://stackoverflow.com/questions/8705201/troubles-with-marching-cubes-and-texture-coordinates
                glm::vec3 normAbs = glm::vec3(std::abs(normal.x), std::abs(normal.y), std::abs(normal.z));
                glm::vec2 uvs[3];
                if (normAbs.x >= normAbs.z && normAbs.x >= normAbs.y) { // x plane
                    uvs[0] = glm::vec2(v0vertexSized.z, v0vertexSized.y);
                    uvs[1] = glm::vec2(v1vertexSized.z, v1vertexSized.y);
                    uvs[2] = glm::vec2(v2vertexSized.z, v2vertexSized.y);
                }
                else if (normAbs.z >= normAbs.x && normAbs.z >= normAbs.y) { // z plane
                    uvs[0] = glm::vec2(v0vertexSized.x, v0vertexSized.y);
                    uvs[1] = glm::vec2(v1vertexSized.x, v1vertexSized.y);
                    uvs[2] = glm::vec2(v2vertexSized.x, v2vertexSized.y);
                }
                else if (normAbs.y >= normAbs.x && normAbs.y >= normAbs.z) { // y plane
                    uvs[0] = glm::vec2(v0vertexSized.x, v0vertexSized.z);
                    uvs[1] = glm::vec2(v1vertexSized.x, v1vertexSized.z);
                    uvs[2] = glm::vec2(v2vertexSized.x, v2vertexSized.z);
                }

                glm::vec2 deltaUV_A = glm::vec2(uvs[1].x - uvs[0].x, uvs[1].y - uvs[0].y);
                glm::vec2 deltaUV_B = glm::vec2(uvs[2].x - uvs[0].x, uvs[2].y - uvs[0].y);
                float f = 1.0f / (deltaUV_A.x * deltaUV_B.y - deltaUV_B.x * deltaUV_A.y);

                glm::vec3 tangent = glm::vec3(
                    f * (deltaUV_B.y * edge_A.x - deltaUV_A.y * edge_B.x),
                    f * (deltaUV_B.y * edge_A.y - deltaUV_A.y * edge_B.y),
                    f * (deltaUV_B.y * edge_A.z - deltaUV_A.y * edge_B.z)  
                );
                tangent = glm::normalize(tangent);

                glm::vec3 bitangent = glm::cross(normal, tangent);


                int index = vZ * (8 * 8) + vY * 8 + vX;
                triangleData.pos_TBN_tex[index] = VoxelTriangleVariant(
                    v0vertexSized, v1vertexSized, v2vertexSized,
                    tangent, bitangent, normal,
                    uvs[0], uvs[1], uvs[2]
                );
            }
        }
    }
    triangleData.figureIds = glm::uvec4((unsigned int)v0data.id, (unsigned int)v1data.id, (unsigned int)v2data.id, 0);

    m_trianglesData.emplace_back(triangleData);
}

void engine::MarchingCubes::draw(int drawCount) {
    glBindVertexArray(m_VAO);
    glMultiDrawArraysIndirect(GL_TRIANGLES, (GLvoid*)0, drawCount, 0);

    #ifndef NDEBUG
    int errors = 0;
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        errors++;
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "engine::MarchingCubes::draw() " << error << std::endl;
    }
    #endif
}
