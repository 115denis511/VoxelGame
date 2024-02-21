#include "InstancingData.h"

engine::InstancingData::InstancingData() {
    m_matrices = new glm::mat4[engine_properties::SCENE_MAX_RENDER_COMPONENTS];
    m_usedCount = 0;
}

engine::InstancingData::~InstancingData() {
    delete [] m_matrices;
}

void engine::InstancingData::add(const glm::mat4 &matrix) {
    m_matrices[m_usedCount] = matrix;
    m_usedCount++;
}

void engine::InstancingData::clear() {
    m_usedCount = 0;
}

const glm::mat4* engine::InstancingData::getMatrices() const {
    return m_matrices;
}

int engine::InstancingData::getCount() const {
    return m_usedCount;
}
