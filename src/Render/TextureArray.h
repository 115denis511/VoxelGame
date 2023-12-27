#ifndef __RENDER__TEXTURE_ARRAY_H__
#define __RENDER__TEXTURE_ARRAY_H__

#include "../stdafx.h"
#include "../Utilites/INonCopyable.h"
#include "../engine_properties.h"
#include "../Log.h"

namespace engine {
    class TextureManager;

    enum class TextureWrap {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter {
        LINEAR,
        NEAREST
    };

    class TextureArray : public utilites::INonCopyable {
    friend TextureManager;
    
    public:
        TextureArray(GLuint width, GLuint height, GLuint layers, TextureWrap wrap = TextureWrap::REPEAT, TextureFilter filter = TextureFilter::LINEAR);
        ~TextureArray();

        void use(int samplerId);
        bool isHaveSpace(unsigned int count = 1);
        bool addTexture(unsigned char* rawImage, int width, int height, int nrComponents);
        bool addTexture(std::string path);
        void makeResident();
        void makeNonResident();
        bool isResident();
        GLuint64 getHandler();
        GLuint   getLastUsedLayer();
        void updateMipmap();

    private:
        GLuint m_texture;
        GLuint m_width;
        GLuint m_height;
        GLuint m_layersSize;
        GLuint m_lastUnusedLayer;
        GLuint64 m_bindlessHandler{ 0 };
    };

    class TextureArrayRef {
    public:
        TextureArrayRef() {}
        TextureArrayRef(TextureArray* ref, GLuint layer);

        GLuint64 getHandler() const;
        GLuint   getLayer() const;
        bool     isValidRef() const;
    private:
        TextureArray* m_ref{ nullptr };
        GLuint        m_layer{ 0 };
    };
}

#endif