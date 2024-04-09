#ifndef __TEST__SCENE_H__
#define __TEST__SCENE_H__

#include "../stdafx.h"
#include "../Controls.h"
#include "../WindowGLFW.h"
#include "../Voxel/MarchingCubesManager.h"
#include "../Scene/ISceneLogic.h"

class SceneTest : public engine::ISceneLogic {
public:
    SceneTest() {}

    void update(engine::Camera& camera, engine::SceneResources& resources) override;
};

#endif