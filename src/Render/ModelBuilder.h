#ifndef __RENDER__MODEL_BUILDER_H__
#define __RENDER__MODEL_BUILDER_H__

#include "Vertex.h"
#include "Mesh.h"

namespace engine {
    class ModelBuilder {
    public:
        static Mesh* buildPrimitivePlane();
        static Mesh* buildPrimitiveScreenPlane();

    private:

    };
}

#endif