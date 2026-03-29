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

    int select = 0;
    int selectedSolid[3] = { 1, 2, 3 };
    int selectedLiquid[3] = { 4, 5, 6 };
};

#endif