#ifndef __SCENE__RENDER_COMPONENT_H__
#define __SCENE__RENDER_COMPONENT_H__

#include "../stdafx.h"
#include "../Render/Model.h"

namespace engine {
    class RenderComponent {
    public:
        RenderComponent();
        RenderComponent(Model* model);

        void draw();
        void updateModelMatrix(const glm::mat4& matrix);
        void pushMatrixToInstancingBuffer();
        glm::mat4& getMatrix();
        Model*     getModel();

    private:
        Model* m_model;
        glm::mat4 m_modelMatrix;

    };
}

#endif