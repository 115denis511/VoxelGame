#include "TextureArray.h"

engine::TextureArray::TextureArray(GLuint width, GLuint height, GLuint layers, TextureWrap wrap, TextureFilter filter) {
    m_width = width;
    m_height = height;
    m_layersSize = layers;
    m_lastUnusedLayer = 0;

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texture);
    glTextureStorage3D(m_texture, engine_properties::MIPMAP_LEVELS, GL_RGBA8, width, height, layers);
    glGenerateTextureMipmap(m_texture);

	switch (wrap) {
    case TextureWrap::REPEAT :
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;

    case TextureWrap::MIRRORED_REPEAT :
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;

    case TextureWrap::CLAMP_TO_EDGE :
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;

    case TextureWrap::CLAMP_TO_BORDER :
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        break;
    
    default:
        break;
    }

    switch (filter) {
    case TextureFilter::LINEAR :
        glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;

    case TextureFilter::NEAREST :
        glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    
    default:
        break;
    }

    m_bindlessHandler = glGetTextureHandleARB(m_texture);
}

engine::TextureArray::~TextureArray() {
    glDeleteTextures(1, &m_texture);
}

void engine::TextureArray::use(int samplerId) {
    glBindTextureUnit(samplerId, m_texture);
}

bool engine::TextureArray::isHaveSpace(unsigned int count) {
    if (m_lastUnusedLayer + count - 1 >= m_layersSize)
        return false;
    return true;
}

bool engine::TextureArray::addTexture(unsigned char *rawImage, int width, int height, int nrComponents) {
    if (m_lastUnusedLayer >= m_layersSize) {
        return false;
    }

    if (width != (int)m_width || height != (int)m_height) {
        Log::addMessage(
            "WARNING! can't add image in texture array - wrong size\nImage width, height: " + std::to_string(width) + " " + std::to_string(height) + 
            "\nArray width, height: " + std::to_string(m_width) + " " + std::to_string(m_height));
        return false;
    }

    GLenum format = GL_RGB;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

    glTextureSubImage3D(m_texture, 0, 0, 0, m_lastUnusedLayer, width, height, 1, format, GL_UNSIGNED_BYTE, rawImage);
    m_lastUnusedLayer++;

    return true;
}

bool engine::TextureArray::addTexture(std::string path) {
    if (m_lastUnusedLayer >= m_layersSize) {
        return false;
    }

    int width, height, nrComponents;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (width != (int)m_width || height != (int)m_height) {
        stbi_image_free(image);
        Log::addMessage(
            "WARNING! can't add image in texture array - wrong size\nImage width, height: " + std::to_string(width) + " " + std::to_string(height) + 
            "\nArray width, height: " + std::to_string(m_width) + " " + std::to_string(m_height));
        return false;
    }

    GLenum format = GL_RGB;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

    glTextureSubImage3D(m_texture, 0, 0, 0, m_lastUnusedLayer, width, height, 1, format, GL_UNSIGNED_BYTE, image);
    m_lastUnusedLayer++;

    stbi_image_free(image);

    return true;
}

void engine::TextureArray::makeResident() {
    glMakeTextureHandleResidentARB(m_bindlessHandler);
}

void engine::TextureArray::makeNonResident() {
    glMakeTextureHandleNonResidentARB(m_bindlessHandler);
}

bool engine::TextureArray::isResident() {
    return glIsTextureHandleResidentARB(m_bindlessHandler);
}

GLuint64 engine::TextureArray::getHandler() {
    return m_bindlessHandler;
}

GLuint engine::TextureArray::getLastUsedLayer() {
    return m_lastUnusedLayer - 1;
}

void engine::TextureArray::updateMipmap() {
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

engine::TextureArrayRef::TextureArrayRef(TextureArray *ref, GLuint layer) {
    m_ref = ref;
    m_layer = layer;
}

GLuint64 engine::TextureArrayRef::getHandler() const {
    return m_ref->getHandler();
}

GLuint engine::TextureArrayRef::getLayer() const {
    return m_layer;
}

bool engine::TextureArrayRef::isValidRef() const {
    if (m_ref != nullptr) 
        return true;
    return false;
}
