#ifndef __SCENE__RENDER_COMPONENT_H__
#define __SCENE__RENDER_COMPONENT_H__

#include "../stdafx.h"
#include "../Render/Model.h"
#include "Transform.h"

namespace engine {
    class Scene;

    class RenderComponent {
        friend Scene;

    public:
        RenderComponent();
        RenderComponent(Model* model);

        void draw();
        void pushMatrixToInstancingBuffer();
        const Transform& getTransform();
        Model*     getModel();

    private:
        Model* m_model;
        Transform* m_transform;

        void setTransform(Transform* transform);
    };
}

#endif