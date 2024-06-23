#include "BVHTreeEntities.h"

std::vector<int> engine::BVHTreeEntities::getOverlapsedRenderComponents(Frustum frustum) {
    if (m_rootNodeId == BVH_TREE_NULL_ID) {
        return std::vector<int>();
    }

    std::vector<int> overlaps;
    overlaps.reserve(engine_properties::SCENE_MAX_RENDER_COMPONENTS);

    m_nodesToProcess.push(m_rootNodeId);

    while(!m_nodesToProcess.empty()) {
        unsigned int nodeId = m_nodesToProcess.top();
        m_nodesToProcess.pop();

        if (nodeId == BVH_TREE_NULL_ID) continue;

        BVHTreeNode<EntityReferences>& node = m_nodes[nodeId];

        if(node.bounds.isInFrustum(frustum)) {
            if(!node.isLeaf()) {
                m_nodesToProcess.push(node.leftNodeId);
                m_nodesToProcess.push(node.rightNodeId);
            }
            else {
                int componentId = node.entityRef->getRenderComponentId();
                overlaps.push_back(componentId);
            }
        }
    }

    return overlaps;
}