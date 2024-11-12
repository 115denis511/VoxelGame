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
            constexpr float offset[8] = { 0.0f, 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f };
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                auto size = marching->getVoxel(hit).size;
                glm::vec3 place(hit.x + offset[size] * face.x, hit.y + offset[size] * face.y, hit.z + offset[size] * face.z);
                resources.transforms.get(0).getObject().setPosition(place);
            }
        }

        if (engine::Controls::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    auto voxel = marching->getVoxel(hit);
                    if (voxel.size > 0) {
                        std::cout << "Voxel size decreased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                        marching->setVoxel(hit, voxel.id, voxel.size - 1);
                    }
                }
                else {
                    std::cout << "Break voxel at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                    marching->setVoxel(hit, 255); 
                }
            }
        }
        else if (engine::Controls::isMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
            glm::vec3 cameraPos = camera.getPosition(), cameraTarget = camera.getFront();
            glm::ivec3 hit, face;
            if (marching->raycastVoxel(cameraPos, cameraTarget, 100.f, hit, face)) {
                if (engine::Controls::isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
                    auto voxel = marching->getVoxel(hit);
                    std::cout << "Voxel size increased at " << hit.x << " " << hit.y << " " << hit.z << "\n";
                    marching->setVoxel(hit, voxel.id, voxel.size + 1);
                }
                else {
                    marching->setVoxel(hit + face, 1, 0);
                }
            }
        }
    }

    // Тест обновления BVH и трансформаций
    /*if (resources.transforms.get(0).isInUse()) {
        resources.transforms.get(0).getObject().movePosition(glm::vec3(-0.001f, 0.f, 0.001f));
    }*/
}