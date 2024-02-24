#include "InstancingData.h"

engine::InstancingData::InstancingData() {
    m_ids = new int[engine_properties::SCENE_MAX_RENDER_COMPONENTS];
    m_usedCount = 0;
}

engine::InstancingData::~InstancingData() {
    delete [] m_ids;
}

void engine::InstancingData::add(const int id) {
    m_ids[m_usedCount] = id;
    m_usedCount++;
}

void engine::InstancingData::clear() {
    m_usedCount = 0;
}

const int* engine::InstancingData::getIds() const {
    return m_ids;
}

int engine::InstancingData::getCount() const {
    return m_usedCount;
}
