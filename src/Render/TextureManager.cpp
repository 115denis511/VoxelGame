#include "TextureManager.h"

GLuint                                                   engine::TextureManager::g_fboTexCopy;
engine::Shader*                                          engine::TextureManager::g_shaderMix_RGB_A;
engine::Mesh*                                            engine::TextureManager::g_fillRect;
std::unordered_map<std::string, engine::TextureArrayRef> engine::TextureManager::g_textureDict;
std::vector<engine::TextureManager::TextureArrayList*>   engine::TextureManager::g_textureArrayList;

bool engine::TextureManager::init(Shader* shaderMix_RGB_A, Mesh* fillRect) {
    g_shaderMix_RGB_A = shaderMix_RGB_A;
    g_fillRect = fillRect;

    glGenFramebuffers(1, &g_fboTexCopy);
	glBindFramebuffer(GL_FRAMEBUFFER, g_fboTexCopy);

    g_textureArrayList.push_back(new TextureArrayList(512, 512, engine_properties::TEXTURE_ARRAY_W512_H512_SIZE));
    g_textureArrayList.push_back(new TextureArrayList(1024, 1024, engine_properties::TEXTURE_ARRAY_W1024_H1024_SIZE));
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void engine::TextureManager::freeResources() {
    for (size_t i = 0; i < g_textureArrayList.size(); i++) {
        delete g_textureArrayList[i];
    }

    glDeleteFramebuffers(1, &g_fboTexCopy);
}

const engine::TextureArrayRef engine::TextureManager::addMixedTexture_RGB_A(
                std::string pathRGB,
                unsigned char* rgbImage, int rgbWidth, int rgbHeight, int rgbNrComponents,
                unsigned char* alphaImage, int alphaWidth, int alphaHeight, int alphaNrComponents) {
    // Выбор списка массивов соответствующего размера
    TextureArrayList* list = nullptr;
    for (size_t i = 0; i < g_textureArrayList.size(); i++) {
        if (g_textureArrayList[i]->isEqualSize(rgbWidth, rgbHeight) && g_textureArrayList[i]->isEqualSize(alphaWidth, alphaHeight)) {
            list = g_textureArrayList[i];
            break;
        }
    }
    if (list == nullptr) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(): ERROR! can't add image - non power of 2 size\nPath: " + 
                           pathRGB + "\nRGB image width, height: " + std::to_string(rgbWidth) + " " + std::to_string(rgbHeight) +
                           "\nALPHA image width, height: " + std::to_string(alphaWidth) + " " + std::to_string(alphaHeight));
        return TextureArrayRef(nullptr, 0);
    }

    // Поиск массива в списке, имеющего свободное место
    TextureArray* textureArray = nullptr;
    std::vector<TextureArray*>& arrays = list->m_textureArrays;
    for (size_t i = 0; i < arrays.size(); i++) {
        if(arrays[i]->isHaveSpace(1)) {
            textureArray = arrays[i];
            break;
        }
    }
    if (textureArray == nullptr) {
        if (!list->addTextureArray()) {
            Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(): ERROR! can't add image");
            return TextureArrayRef(nullptr, 0);
        }
        textureArray = arrays.back();
    }

    // Загрузка текстур во временные слоты
    list->loadTextureFromRaw(list->m_textureTempA, rgbImage, rgbNrComponents);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, list->m_textureTempA);
    list->loadTextureFromRaw(list->m_textureTempB, alphaImage, alphaNrComponents);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, list->m_textureTempB);

    // Смешивание и копирование текстур в текстурный массив
	glBindFramebuffer(GL_FRAMEBUFFER, g_fboTexCopy);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureArray->m_texture, 0, textureArray->m_lastUnusedLayer);
    WindowGLFW::setViewport(list->m_width, list->m_height);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    g_shaderMix_RGB_A->use();
    g_shaderMix_RGB_A->setInt("colorTex", 0);
    g_shaderMix_RGB_A->setInt("alphaTex", 1);

    g_fillRect->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Сохранение
    TextureArrayRef ref(textureArray, textureArray->m_lastUnusedLayer);
    g_textureDict[pathRGB] = ref;
    textureArray->m_lastUnusedLayer++;

    #ifndef NDEBUG
        int errors = 0;
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR) {
            errors++;
            std::string error;
            switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << "engine::TextureManager::addMixedTexture_RGB_A() " + error << std::endl;
        }
    #endif

    return ref;
}

const engine::TextureArrayRef engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha) {
    if (g_textureDict.contains(pathRGB)) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha): ERROR! Texture with this name already exist");
        return TextureArrayRef(nullptr, 0);
    }

    int rgbWidth, rgbHeight, rgbNrComponents;
    unsigned char* rgbImage = stbi_load(pathRGB.c_str(), &rgbWidth, &rgbHeight, &rgbNrComponents, 0);
    if(rgbImage == nullptr) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha): ERROR! stbi_failure_reason: " + 
                           std::string(stbi_failure_reason()) + "\nPath: " + pathRGB);
        stbi_image_free(rgbImage);
        return TextureArrayRef(nullptr, 0);
    }

    int alphaWidth, alphaHeight, alphaNrComponents;
    unsigned char* alphaImage = stbi_load(pathAlpha.c_str(), &alphaWidth, &alphaHeight, &alphaNrComponents, 0);
    if(alphaImage == nullptr) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha): ERROR! stbi_failure_reason: " + 
                           std::string(stbi_failure_reason()) + "\nPath: " + pathAlpha);
        stbi_image_free(rgbImage);
        stbi_image_free(alphaImage);
        return TextureArrayRef(nullptr, 0);
    }

    TextureArrayRef ref = addMixedTexture_RGB_A(pathRGB,
                                                rgbImage, rgbWidth, rgbHeight, rgbNrComponents,
                                                alphaImage, alphaWidth, alphaHeight, alphaNrComponents);

    if (!ref.isValidRef()) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha): ERROR! Failed to add texture");
    }

    stbi_image_free(rgbImage);
    stbi_image_free(alphaImage);

    return ref;
}

const engine::TextureArrayRef engine::TextureManager::addTexture(std::string path, unsigned char* image, int width, int height, int nrComponents) {
    // Выбор списка массивов соответствующего размера
    TextureArrayList* list = nullptr;
    for (size_t i = 0; i < g_textureArrayList.size(); i++) {
        if (g_textureArrayList[i]->isEqualSize(width, height)) {
            list = g_textureArrayList[i];
            break;
        }
    }
    if (list == nullptr) {
        Log::addFatalError("engine::TextureManager::addTexture(): ERROR! can't add image - non power of 2 size\nPath: " + 
                           path + "\nImage width, height: " + std::to_string(width) + " " + std::to_string(height));
        return TextureArrayRef(nullptr, 0);
    }

    // Поиск массива в списке, имеющего свободное место
    TextureArray* textureArray = nullptr;
    std::vector<TextureArray*>& arrays = list->m_textureArrays;
    for (size_t i = 0; i < arrays.size(); i++) {
        if(arrays[i]->isHaveSpace(1)) {
            textureArray = arrays[i];
            break;
        }
    }
    if (textureArray == nullptr) {
        if (!list->addTextureArray()) {
            Log::addFatalError("engine::TextureManager::addTexture(): ERROR! can't add image");
            return TextureArrayRef(nullptr, 0);
        }
        textureArray = arrays.back();
    }

    // Добавление в массив
    if (!textureArray->addTexture(image, width, height, nrComponents)) {
        Log::addFatalError("engine::TextureManager::addTexture(): ERROR! can't add image");
        return TextureArrayRef(nullptr, 0);
    }

    // Сохранение
    TextureArrayRef ref(textureArray, textureArray->getLastUsedLayer());
    g_textureDict[path] = ref;

    return ref;
}

const engine::TextureArrayRef engine::TextureManager::addTexture(std::string path) {
    if (g_textureDict.contains(path)) {
        Log::addFatalError("engine::TextureManager::addTexture(std::string path): ERROR! Texture with this name already exist");
        return TextureArrayRef(nullptr, 0);
    }

    int width, height, nrComponents;
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if(image == nullptr) {
        Log::addFatalError("engine::TextureManager::addTexture(std::string path): ERROR! stbi_failure_reason: " + std::string(stbi_failure_reason()) +
                           "\nPath: " + path);
        stbi_image_free(image);
        return TextureArrayRef(nullptr, 0);
    }

    TextureArrayRef ref = addTexture(path, image, width, height, nrComponents);

    if (!ref.isValidRef()) {
        Log::addFatalError("engine::TextureManager::addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha): ERROR! Failed to add texture");
    }

    stbi_image_free(image);

    return ref;
}

const engine::TextureArrayRef engine::TextureManager::getTexture(std::string name) {
    if (g_textureDict.contains(name)) {
        return g_textureDict[name];
    }
    return TextureArrayRef(nullptr, 0);
}

void engine::TextureManager::updateMipmapsAndMakeResident() {
    for (size_t i = 0; i < g_textureArrayList.size(); i++) {
        for (size_t j = 0; j < g_textureArrayList[i]->m_textureArrays.size(); j++) {
            g_textureArrayList[i]->m_textureArrays[j]->updateMipmap();
            if (!g_textureArrayList[i]->m_textureArrays[j]->isResident()) {
                g_textureArrayList[i]->m_textureArrays[j]->makeResident();
            }
        }
    }
}

engine::TextureManager::TextureArrayList::TextureArrayList(GLuint width, GLuint height, GLuint maxCapacity) {
    m_width = width;
    m_height = height;
    m_maxCapacity = maxCapacity;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureTempA);
    glTextureParameteri(m_textureTempA, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(m_textureTempA, 1, GL_RGBA8, 512, 512);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureTempB);
    glTextureParameteri(m_textureTempB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureStorage2D(m_textureTempB, 1, GL_RGBA8, 512, 512);

    m_textureArrays.push_back(new TextureArray(width, height, maxCapacity));
}

engine::TextureManager::TextureArrayList::~TextureArrayList() {
    glDeleteTextures(1, &m_textureTempA);
    glDeleteTextures(1, &m_textureTempB);

    for (size_t i = 0; i < m_textureArrays.size(); i++) {
        delete m_textureArrays[i];
    }
}

bool engine::TextureManager::TextureArrayList::addTextureArray() {
    TextureArray* array = new TextureArray(m_width, m_height, m_maxCapacity);

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        bool hasError = false;
        switch (errorCode) {
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        default:
            break;
        }

        if (hasError) {
            Log::addFatalError("engine::TextureManager::TextureArrayList::addTextureArray() ERROR! Not enoght GPU memory to allocate texture array");
            delete array;
            return false;
        }
    }

    m_textureArrays.push_back(array);

    return true;
}

bool engine::TextureManager::TextureArrayList::isEqualSize(int width, int height) {
    return width == (int)m_width && height == (int)m_height;
}

void engine::TextureManager::TextureArrayList::loadTextureFromRaw(GLuint textureUnit, unsigned char *rawImage, int components) {
    GLenum format = GL_RGB;
    format = GL_RGB;
	if (components == 1)
		format = GL_RED;
	else if (components == 3)
		format = GL_RGB;
	else if (components == 4)
		format = GL_RGBA;
    glTextureSubImage2D(textureUnit, 0, 0, 0, m_width, m_height, format, GL_UNSIGNED_BYTE, rawImage);
}
