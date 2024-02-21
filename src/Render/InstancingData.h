#ifndef __RENDER__INSTANCING_DATA_H__
#define __RENDER__INSTANCING_DATA_H__

#include "../stdafx.h"
#include "../engine_properties.h"

namespace engine {
    class InstancingData {
    public:
        InstancingData();
        ~InstancingData();

        void add(const glm::mat4& matrix);
        void clear();
        const glm::mat4* getMatrices() const;
        int getCount() const;

    private:
        glm::mat4* m_matrices;
        size_t m_usedCount;
    };
}

#endif