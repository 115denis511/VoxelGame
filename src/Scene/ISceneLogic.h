#ifndef __SCENE__I_SCENE_LOGIC_H__
#define __SCENE__I_SCENE_LOGIC_H__

#include "Camera.h"
#include "SceneResources.h"

namespace engine {
    class ISceneLogic {
    public:
        virtual void update(Camera& camera, SceneResources& resources) = 0;

    protected:
        ISceneLogic() {}
    };
}

#endif