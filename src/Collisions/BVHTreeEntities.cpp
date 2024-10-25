#include "BVHTreeEntities.h"

std::vector<int> engine::BVHTreeEntities::getOverlapsedRenderComponents(Frustum frustum) {
    if (m_rootNodeId == BVH_TREE_NULL_ID) {
        return std::vector<int>();
    }

    std::vector<int> overlaps;
    overlaps.reserve(engine_properties::SCENE_MAX_RENDER_COMPONENTS);

    // stack method
    /*m_nodesToProcess.push(m_rootNodeId);

    while(!m_nodesToProcess.empty()) {
        unsigned int nodeId = m_nodesToProcess.top();
        m_nodesToProcess.pop();

        if (nodeId == BVH_TREE_NULL_ID) continue;

        BVHTreeNode<EntityReferences, SphereVolume>& node = m_nodes[nodeId];

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
    }*/

    // recursive method
    try {
        if (m_rootNodeId != BVH_TREE_NULL_ID)
            nodeProcess(m_rootNodeId, overlaps, frustum);
    }
    catch(const std::exception& e) {
        std::string error = "src/Collisions/BVHTreeEntities.cpp::nodeProcess() " + std::string(e.what());
        std::cerr << error << '\n';
        Log::addFatalError(error);
    }

    return overlaps;
}

void engine::BVHTreeEntities::nodeProcess(unsigned int nodeId, std::vector<int>& overlaps, const Frustum& frustum) {
    //if (nodeId == BVH_TREE_NULL_ID) return;

    BVHTreeNode<EntityReferences, SphereVolume>& node = m_nodes[nodeId];

    if(node.bounds.isInFrustum(frustum)) {
        if(!node.isLeaf()) {
            nodeProcess(node.leftNodeId, overlaps, frustum);
            nodeProcess(node.rightNodeId, overlaps, frustum);
        }
        else {
            int componentId = node.entityRef->getRenderComponentId();
            overlaps.push_back(componentId);
        }
    }
}
