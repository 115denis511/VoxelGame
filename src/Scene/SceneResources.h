#ifndef __SCENE__SCENE_RESOURCES_H__
#define __SCENE__SCENE_RESOURCES_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "../Utilites/ObjectPool.h"
#include "EntityReferences.h"
#include "Transform.h"
#include "RenderComponent.h"

namespace engine {
    class Scene;

    class SceneResources {
        friend Scene;
        
    public:
        utilites::ObjectPool<EntityReferences, engine_properties::SCENE_MAX_ENTITIES> entities;
        utilites::ObjectPool<Transform, engine_properties::SCENE_MAX_TRANSFORMS> transforms;
        utilites::ObjectPool<RenderComponent, engine_properties::SCENE_MAX_RENDER_COMPONENTS> renderComponents;

    private:
        SceneResources();
    };
}

#endif