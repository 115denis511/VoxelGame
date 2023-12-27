#include "GBuffer.h"

engine::GBuffer::GBuffer(const glm::ivec2 &viewport) {
    m_viewport = viewport;

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(3, &m_layout[0]);

    // буфер позиций
    constexpr int indexPos = static_cast<int>(GBufferLayout::POSITIONS);
    glBindTexture(GL_TEXTURE_2D, m_layout[indexPos]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_viewport.x, m_viewport.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_layout[indexPos], 0);

    // буфер нормалей
    constexpr int indexNormal = static_cast<int>(GBufferLayout::NORMALS);
    glBindTexture(GL_TEXTURE_2D, m_layout[indexNormal]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_viewport.x, m_viewport.y, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_layout[indexNormal], 0);

    // буфер для цвета + коэффициента зеркального отражения
    constexpr int indexAlbedo = static_cast<int>(GBufferLayout::ALBEDO_SPEC);
    glBindTexture(GL_TEXTURE_2D, m_layout[indexAlbedo]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_viewport.x, m_viewport.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_layout[indexAlbedo], 0);

    // укажем OpenGL, какие буферы мы будем использовать при рендеринге
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.x, m_viewport.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Log::addFatalError("engine::GBuffer::GBuffer(const glm::ivec2 &viewport): ERROR! Framebuffer is not complete!");
	}
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

engine::GBuffer::~GBuffer() {
    glDeleteTextures(3, &m_layout[0]);
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteRenderbuffers(1, &m_RBO);
}

void engine::GBuffer::updateViewport(const glm::ivec2 &viewport) {
    m_viewport = viewport;

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	
    // буфер позиций
	constexpr int indexPos = static_cast<int>(GBufferLayout::POSITIONS);
	glBindTexture(GL_TEXTURE_2D, m_layout[indexPos]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_viewport.x, m_viewport.y, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_layout[indexPos], 0);

    // буфер нормалей
    constexpr int indexNormal = static_cast<int>(GBufferLayout::NORMALS);
    glBindTexture(GL_TEXTURE_2D, m_layout[indexNormal]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_viewport.x, m_viewport.y, 0, GL_RGB, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_layout[indexNormal], 0);

    // буфер для цвета + коэффициента зеркального отражения
    constexpr int indexAlbedo = static_cast<int>(GBufferLayout::ALBEDO_SPEC);
    glBindTexture(GL_TEXTURE_2D, m_layout[indexAlbedo]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_viewport.x, m_viewport.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_layout[indexAlbedo], 0);

    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.x, m_viewport.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::GBuffer::useTexture(int samplerId, GBufferLayout layout) {
    int index = static_cast<int>(layout);
	glBindTextureUnit(samplerId, m_layout[index]);
}
