#ifndef __VOXEL_TEXTURE_MANAGER_H__
#define __VOXEL_TEXTURE_MANAGER_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "../Utilites/INonCopyable.h"
#include "../Log.h"
#include "../Render/UniformManager.h"

namespace engine {
    class VoxelTextures : public utilites::INonCopyable {
    public:
        VoxelTextures();
        ~VoxelTextures();

        void use();
        bool setTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setTexture(int layer, std::string path);
        bool setTexture(int layer, glm::vec4 color);
        void makeResident();
        void makeNonResident();
        bool isResident();
        GLuint64 getHandler();
        GLuint   getCapacity();
        void updateMipmap();

    private:
        static constexpr GLuint WIDTH = 64, HEIGHT = 64, LAYERS = 128;
        GLuint m_textureArray;
        GLuint64 m_bindlessHandler{ 0 };
    };
}

#endif