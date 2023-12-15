#ifndef __RENDER__FRAMEBUFFER_H__
#define __RENDER__FRAMEBUFFER_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"

namespace engine {
    class Framebuffer : public utilites::INonCopyable {
    public:
        virtual ~Framebuffer() {
            glDeleteFramebuffers(1, &m_FBO);
            glDeleteRenderbuffers(1, &m_RBO);
        }

        virtual void bind() {
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        }
        virtual void bindRead() {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
        }
        virtual void bindDraw() {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
        }
        virtual void copyDepth(Framebuffer& source) {
            const glm::ivec2& sourceViewport = source.getViewport();
            glBlitNamedFramebuffer(source.getFBO(), m_FBO, 0, 0, sourceViewport.x, sourceViewport.y, 0, 0, m_viewport.x, m_viewport.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        virtual void updateViewport(const glm::ivec2& viewport) {}
        
        const glm::ivec2& getViewport() { return m_viewport; }
        const GLuint getFBO() { return m_FBO; };

    protected:
        Framebuffer() {}

        GLuint m_FBO, m_RBO;
        glm::ivec2 m_viewport;
    };
}

#endif