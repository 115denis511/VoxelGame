#ifndef __VOXEL__MARCHING_CUBES_RENDER_BASE_H__
#define __VOXEL__MARCHING_CUBES_RENDER_BASE_H__

#include "../stdafx.h"
#include "../Collisions/Frustum.h"
#include "../Render/DrawIndirectCommand.h"
#include "../Render/Shader.h"
#include "../Scene/Camera.h"
#include "MarchingCubes.h"
#include "MarchingCubesSSBOs.h"
#include "VoxelTextures.h"

namespace engine {
    class MarchingCubesRenderBase {
    public:
        virtual ~MarchingCubesRenderBase();

        virtual void initSSBOs(MarchingCubesSSBOs& ssbos) = 0;
        virtual void drawSolid(const CameraVars &cameraVars, Frustum frustum, MarchingCubes& marchingCubes) = 0;

        bool setVoxelTexture(int layer, unsigned char *rawImage, int width, int height, int nrComponents);
        bool setVoxelTexture(int layer, std::string path);
        bool setVoxelTexture(int layer, glm::vec4 color);
        void startTextureEditing();
        void endTextureEditing();

    protected:
        MarchingCubesRenderBase(
            GLuint maxChunksPerDraw,
            const GLchar* shaderSolidVertPath, 
            const GLchar* shaderSolidFragPath
        );

        std::vector<DrawArraysIndirectCommand> m_drawCommands;
        std::vector<GLuint> m_drawBufferRefs;
        Shader* m_shaderSolid{ nullptr };
        VoxelTextures m_textures;
        GLuint m_commandBuffer;
    };
}

#endif
