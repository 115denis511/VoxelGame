#include "VoxelTextures.h"

engine::VoxelTextures::VoxelTextures() {
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_textureArray);
    glTextureStorage3D(m_textureArray, engine_properties::MIPMAP_LEVELS, GL_RGBA8, WIDTH, HEIGHT, LAYERS);
    glGenerateTextureMipmap(m_textureArray);

    glTextureParameteri(m_textureArray, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureArray, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureParameteri(m_textureArray, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_textureArray, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_bindlessHandler = glGetTextureHandleARB(m_textureArray);
}

engine::VoxelTextures::~VoxelTextures() {
    glDeleteTextures(1, &m_textureArray);
}

void engine::VoxelTextures::use() {
    auto pack = uniform_structs::TexturePack(m_bindlessHandler, 0, LAYERS);
    UniformManager::setTexturePack(pack);
}

bool engine::VoxelTextures::setTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents)
{
    if (layer >= (int)LAYERS) {
        Log::addMessage("src/Voxel/VoxelTextures.cpp: WARNING! can't add image in texture array - layer out of range");
        return false;
    }

    if (width != (int)WIDTH || height != (int)HEIGHT) {
        Log::addMessage(
            "src/Voxel/VoxelTextures.cpp: WARNING! can't add image in texture array - wrong size\nImage width, height: " + std::to_string(width) + " " + std::to_string(height) + 
            "\nArray width, height: " + std::to_string(WIDTH) + " " + std::to_string(HEIGHT));
        return false;
    }

    GLenum format = GL_RGB;
	if (nrComponents == 1)
		format = GL_RED;
	else if (nrComponents == 3)
		format = GL_RGB;
	else if (nrComponents == 4)
		format = GL_RGBA;

    glTextureSubImage3D(m_textureArray, 0, 0, 0, layer, width, height, 1, format, GL_UNSIGNED_BYTE, rawImage);

    return true;
}

bool engine::VoxelTextures::setTexture(int layer, std::string path) {
    int width, height, nrComponents;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    bool success = setTexture(layer, image, width, height, nrComponents);
    if (!success)
        Log::addMessage("src/Voxel/VoxelTextures.cpp: WARNING! Failed to add texture. Path: " + path);

    stbi_image_free(image);

    return success;
}

bool engine::VoxelTextures::setTexture(int layer, glm::vec4 color) {
    if (layer >= (int)LAYERS) {
        return false;
    }

    GLfloat* colorImg = new GLfloat[WIDTH * HEIGHT * 4];
    for (size_t i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        colorImg[i] = color.r;
        colorImg[i+1] = color.g;
        colorImg[i+2] = color.b;
        colorImg[i+3] = color.a;
    }

    glTextureSubImage3D(m_textureArray, 0, 0, 0, layer, WIDTH, HEIGHT, 1, GL_RGBA, GL_FLOAT, colorImg);

    delete [] colorImg;

    return true;
}

void engine::VoxelTextures::makeResident() {
    glMakeTextureHandleResidentARB(m_bindlessHandler);
}

void engine::VoxelTextures::makeNonResident() {
    glMakeTextureHandleNonResidentARB(m_bindlessHandler);
}

bool engine::VoxelTextures::isResident() {
    return glIsTextureHandleResidentARB(m_bindlessHandler);
}

GLuint64 engine::VoxelTextures::getHandler() {
    return m_bindlessHandler;
}

GLuint engine::VoxelTextures::getCapacity() {
    return LAYERS;
}

void engine::VoxelTextures::updateMipmap() {
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureArray);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
