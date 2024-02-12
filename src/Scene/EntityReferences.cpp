#include "EntityReferences.h"

engine::EntityReferences::EntityReferences(int transformId, int renderComponentId) {
    m_transformId = transformId;
    m_renderComponentId = renderComponentId;
}

const int engine::EntityReferences::getId() const {
    return m_id;
}

const int engine::EntityReferences::getTransformId() const {
    return m_transformId;
}

const int engine::EntityReferences::getRenderComponentId() const {
    return m_renderComponentId;
}
