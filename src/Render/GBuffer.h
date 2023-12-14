#ifndef __RENDER__GBUFFER_H__
#define __RENDER__GBUFFER_H__

#include "../stdafx.h"
#include "../Log.h"
#include "Framebuffer.h"

namespace engine {
    enum class GBufferLayout {
        POSITIONS = 0,
        NORMALS = 1,
        ALBEDO_SPEC = 2
    };

    class GBuffer : public Framebuffer {
    public:
        GBuffer(const glm::ivec2& viewport);
        ~GBuffer();

    void updateViewport(const glm::ivec2& viewport) override;
    void useTexture(int samplerId, GBufferLayout layout);

    private:
        GLuint m_layout[3];

    };
}

#endif