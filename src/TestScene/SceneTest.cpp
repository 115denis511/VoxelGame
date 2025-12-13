#include "SceneTest.h"

void SceneTest::update(engine::Camera& camera, engine::SceneResources &resources) {
    engine::MarchingCubesManager* marching = engine::MarchingCubesManager::getInstance();

    const std::vector<int>& keysDown = engine::Controls::getKeysDown();
    for (size_t i = 0; i < keysDown.size(); i++) {
        switch (keysDown[i]) {
        case GLFW_KEY_ESCAPE:
            if (engine::WindowGLFW::isMouseCursorHidden())
                engine::WindowGLFW::showMouseCursor();
            else
                engine::WindowGLFW::hideMouseCursor();
            break;

        case GLFW_KEY_MINUS: {
            int renderChunkRadius = marching->getRenderChunkRadius();
            marching->setRenderChunkRadius(renderChunkRadius - 1);
        }
        break;

        case GLFW_KEY_EQUAL: {
            int renderChunkRadius = marching->getRenderChunkRadius();
            marching->setRenderChunkRadius(renderChunkRadius + 1);
        }
        break;
        
        default:
            break;
        }
    }

    if (engine::WindowGLFW::isMouseCursorHidden()) {
        glm::vec2 mouseOffset = engine::Controls::getMouseOffset() * 0.05f;
        camera.moveRotation(mouseOffset);

        const glm::vec3 front = camera.getFront();
        const glm::vec3 right = camera.getRight();

        if (engine::Controls::isKeyPressed(GLFW_KEY_W)) {
            camera.movePosition(front * 0.1f);
        }
        if (engine::Controls::isKeyPressed(GLFW_KEY_S)) {
            camera.movePosition(-front * 0.1f);
        }
        if (engine::Controls::isKeyPressed(GLFW_KEY_D)) {
            camera.movePosition(right * 0.1f);
        }
        if (engine::Controls::isKeyPressed(GLFW_KEY_A)) {
            camera.movePosition(-right * 0.1f);
        }

        if (engine::Controls::isKeyPressed(GLFW_KEY_SPACE)) {
            constexpr std::array<float, 8> offset = engine::MarchingCubesManager::getRaycastHitOffset();
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                auto size = marching->getVoxel(hit).getSolidSize();
                glm::vec3 place(hit.x + offset[size] * face.x, hit.y + offset[size] * face.y, hit.z + offset[size] * face.z);
                resources.transforms.get(0).getObject().setPosition(place);
            }
        }

        if (engine::Controls::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL) && engine::Controls::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
                    auto voxel = marching->getVoxel(hit);
                    if (voxel.getLiquidSize() > 0 && voxel.getLiquidId() != engine::Voxel::NO_VOXEL) {
                        std::cout << "Water voxel size decreased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                        marching->setLiquidVoxel(hit, voxel.getLiquidId(), voxel.getLiquidSize() - 1);
                    }
                }
                else if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
                    glm::ivec3 local = engine::VoxelPositionConverter::worldPositionToLocalVoxelPosition(hit, 32);
                    std::cout << "Break water voxel at " << hit.x << " " << hit.y << " " << hit.z << " | Local: " << local.x << " " << local.y << " " << local.z << "\n";
                    marching->deleteLiquidVoxel(hit); 
                }
                else if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    auto voxel = marching->getVoxel(hit);
                    if (voxel.getSolidSize() > 0 && voxel.getSolidId() != engine::Voxel::NO_VOXEL) {
                        std::cout << "Voxel size decreased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                        marching->setSolidVoxel(hit, voxel.getSolidId(), voxel.getSolidSize() - 1);
                    }
                }
                else {
                    glm::ivec3 local = engine::VoxelPositionConverter::worldPositionToLocalVoxelPosition(hit, 32);
                    std::cout << "Break voxel at " << hit.x << " " << hit.y << " " << hit.z << " | Local: " << local.x << " " << local.y << " " << local.z << "\n";
                    marching->deleteSolidVoxel(hit); 
                }
            }
        }
        else if (engine::Controls::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL) && engine::Controls::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
                    auto voxel = marching->getVoxel(hit);
                    if (voxel.getLiquidSize() != 7 && voxel.getLiquidId() != engine::Voxel::NO_VOXEL) {
                        std::cout << "Water voxel size increased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                        marching->setLiquidVoxel(hit, voxel.getLiquidId(), voxel.getLiquidSize() + 1);
                    }
                }
                else if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_ALT)) {
                    marching->setLiquidVoxel(hit, 4, 0);
                }
                else if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    auto voxel = marching->getVoxel(hit);
                    if (voxel.getSolidSize() != 7 && voxel.getSolidId() != engine::Voxel::NO_VOXEL) {
                        std::cout << "Voxel size increased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                        marching->setSolidVoxel(hit, voxel.getSolidId(), voxel.getSolidSize() + 1);
                    }
                }
                else {
                    marching->setSolidVoxel(hit + face, 1, 0);
                }
            }
        }
    }

    // Тест обновления BVH и трансформаций
    /*if (resources.transforms.get(0).isInUse()) {
        resources.transforms.get(0).getObject().movePosition(glm::vec3(-0.001f, 0.f, 0.001f));
    }*/
}