#ifndef __RENDER__INSTANCING_DATA_H__
#define __RENDER__INSTANCING_DATA_H__

#include "../stdafx.h"
#include "../engine_properties.h"

namespace engine {
    class InstancingData {
    public:
        InstancingData();
        ~InstancingData();

        void add(int id);
        void clear();
        const int* getIds() const;
        int getCount() const;

    private:
        int* m_ids;
        size_t m_usedCount;
    };
}

#endif