#include "BVHTree.h"

engine::BVHTree::BVHTree() {
    m_nodes.resize(engine_properties::SCENE_MAX_ENTITIES);
    for (size_t i = 0; i < m_nodes.size() - 1; i++) {
        m_nodes[i].nextNodeId = i + 1;
    }
    m_nodes[m_nodes.size() - 1].nextNodeId = BVH_TREE_NULL_ID;
    m_nextFreeNodeId = 0;
}

void engine::BVHTree::insertObject(EntityReferences *entity, const SphereVolume &volume) {
    unsigned nodeId = allocateNode();
	BVHTreeNode& node = m_nodes[nodeId];

	node.bounds = volume;
	node.entityRef = entity;
    entity->m_BVHNodeId = nodeId;

	insertLeaf(nodeId);
}

void engine::BVHTree::removeObject(EntityReferences *entity) {
    unsigned nodeId = entity->m_BVHNodeId;
    removeLeaf(nodeId);
	deallocateNode(nodeId);
}

void engine::BVHTree::updateObject(const EntityReferences& object, const SphereVolume& newVolume) {
    updateLeaf(object.m_BVHNodeId, newVolume);
}

std::vector<int> engine::BVHTree::getOverlapsedRenderComponents(Frustum frustum) {
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

        BVHTreeNode& node = m_nodes[nodeId];

        if(node.bounds.isInFrustum(frustum)) {
            if(!node.isLeaf()) {
                m_nodesToProcess.push(node.leftNodeId);
                m_nodesToProcess.push(node.rightNodeId);
            }
            else {
                int componentId = node.entityRef->m_renderComponentId;
                overlaps.push_back(componentId);
            }
        }
    }

    return overlaps;
}

unsigned int engine::BVHTree::allocateNode() {
    if (m_nextFreeNodeId == BVH_TREE_NULL_ID) {
        size_t oldCapacity = m_nodes.size();
        m_nodes.resize(oldCapacity * 2);

        for (size_t i = oldCapacity; i < m_nodes.size(); i++) {
            m_nodes[i].nextNodeId = i + 1;
        }
        m_nodes[m_nodes.size() - 1].nextNodeId = BVH_TREE_NULL_ID;
        m_nextFreeNodeId = oldCapacity;    
    }

    unsigned int newNodeId = m_nextFreeNodeId;
    BVHTreeNode& newNode = m_nodes[newNodeId];

    newNode.parentNodeId = BVH_TREE_NULL_ID;
    newNode.leftNodeId = BVH_TREE_NULL_ID;
    newNode.rightNodeId = BVH_TREE_NULL_ID;
    newNode.entityRef = nullptr;
    m_nextFreeNodeId = newNode.nextNodeId;

    return newNodeId;
}

void engine::BVHTree::deallocateNode(unsigned int nodeId) {
    m_nodes[nodeId] = BVHTreeNode();
    m_nodes[nodeId].nextNodeId = m_nextFreeNodeId;
    m_nextFreeNodeId = nodeId;
}

void engine::BVHTree::insertLeaf(unsigned int nodeId) {
    assert(m_nodes[nodeId].parentNodeId == BVH_TREE_NULL_ID);
	assert(m_nodes[nodeId].leftNodeId == BVH_TREE_NULL_ID);
	assert(m_nodes[nodeId].rightNodeId == BVH_TREE_NULL_ID);

    if (m_rootNodeId == BVH_TREE_NULL_ID) {
        m_rootNodeId = nodeId;
        return;
    }

    unsigned int treeNodeId = m_rootNodeId;
    BVHTreeNode& leafNode = m_nodes[nodeId];
    while (!m_nodes[treeNodeId].isLeaf()) {
        const BVHTreeNode& treeNode = m_nodes[treeNodeId];
        const BVHTreeNode& leftNode = m_nodes[treeNode.leftNodeId];
        const BVHTreeNode& rightNode = m_nodes[treeNode.rightNodeId];

        SphereVolume combinedBounds = treeNode.bounds.merge(leafNode.bounds);

        float newParentNodeCost = 2.0f * combinedBounds.getArea();
		float minimumPushDownCost = 2.0f * (combinedBounds.getArea() - treeNode.bounds.getArea());

        float costLeft;
		float costRight;
        if (leftNode.isLeaf()) {
            costLeft = leafNode.bounds.merge(leftNode.bounds).getArea() + minimumPushDownCost;
        }
        else {
            SphereVolume newLeftBounds = leafNode.bounds.merge(leftNode.bounds);
            costLeft = (newLeftBounds.getArea() - leftNode.bounds.getRadius()) + minimumPushDownCost;	
        }

        if (rightNode.isLeaf()) {
            costRight = leafNode.bounds.merge(rightNode.bounds).getArea() + minimumPushDownCost;
        }
        else {
            SphereVolume newRightBounds = leafNode.bounds.merge(rightNode.bounds);
            costRight = (newRightBounds.getArea() - rightNode.bounds.getRadius()) + minimumPushDownCost;	
        }

        // Если цена создания новой родительской ноды в данном случае меньше, чем если искать глубже, то
        // создаём новую родительскую ноду и прикрепляем наш лист к ней.
        if (newParentNodeCost < costLeft && newParentNodeCost < costRight) {			
			break;
		}

        // Иначе выбираем более дешевое направление для дальнейшего поиска
		if (costLeft < costRight) {
			treeNodeId = treeNode.leftNodeId;
		}
		else {
			treeNodeId = treeNode.rightNodeId;
		}
    }
    
    // Выделяем память под новую ноду заранее, так как если при добавлении может
    // увеличиться буфер для нод и ссылки на ноды перестанут быть корректными
	unsigned int newParentIndex = allocateNode();

    BVHTreeNode& createdleafNode = m_nodes[nodeId];
    unsigned int leafSiblingId = treeNodeId;
	BVHTreeNode& leafSibling = m_nodes[leafSiblingId];
    unsigned int oldParentIndex = leafSibling.parentNodeId;

	BVHTreeNode& newParent = m_nodes[newParentIndex];
    newParent.parentNodeId = oldParentIndex;
	newParent.bounds = createdleafNode.bounds.merge(leafSibling.bounds);
    newParent.leftNodeId = leafSiblingId;
	newParent.rightNodeId = nodeId;

	createdleafNode.parentNodeId = newParentIndex;
	leafSibling.parentNodeId = newParentIndex;

    if (oldParentIndex == BVH_TREE_NULL_ID) {
		// Если родитель для сдвигаемой ноды был корневым, то новая нода становится корневой
		m_rootNodeId = newParentIndex;
	}
    else {
        BVHTreeNode& oldParent = m_nodes[oldParentIndex];
		if (oldParent.leftNodeId == leafSiblingId) {
			oldParent.leftNodeId = newParentIndex;
		}
		else {
			oldParent.rightNodeId = newParentIndex;
		}
    }

    treeNodeId = createdleafNode.parentNodeId;
    fixUpwardsTree(treeNodeId);
}

void engine::BVHTree::removeLeaf(unsigned int nodeId) {
    if (nodeId == m_rootNodeId) {
		m_rootNodeId = BVH_TREE_NULL_ID;
		return;
	}

    BVHTreeNode& leafNode = m_nodes[nodeId];
	unsigned int parentNodeId = leafNode.parentNodeId;
	const BVHTreeNode& parentNode = m_nodes[parentNodeId];
    unsigned int grandParentNodeId = parentNode.parentNodeId;
	unsigned int siblingNodeId = parentNode.leftNodeId == nodeId ? parentNode.rightNodeId : parentNode.leftNodeId;
	assert(siblingNodeId != BVH_TREE_NULL_ID);
	BVHTreeNode& siblingNode = m_nodes[siblingNodeId];

    if (grandParentNodeId != BVH_TREE_NULL_ID) {
        // Если имеется прапредок, то удаляем предка и прикрепляем соседа к прапредку вместо него
		BVHTreeNode& grandParentNode = m_nodes[grandParentNodeId];
		if (grandParentNode.leftNodeId == parentNodeId) {
			grandParentNode.leftNodeId = siblingNodeId;
		}
		else {
			grandParentNode.rightNodeId = siblingNodeId;
		}
		siblingNode.parentNodeId = grandParentNodeId;
		deallocateNode(parentNodeId);

		fixUpwardsTree(grandParentNodeId);
    }
    else {
        // Если предок является корневой нодой, то соседняя для удаляемой нода становится корневой.
        m_rootNodeId = siblingNodeId;
        siblingNode.parentNodeId = BVH_TREE_NULL_ID;
		deallocateNode(parentNodeId);
    }

    leafNode.parentNodeId = BVH_TREE_NULL_ID;
}

void engine::BVHTree::updateLeaf(unsigned int nodeId, const SphereVolume &newVolume) {
    BVHTreeNode& node = m_nodes[nodeId];

    if (node.bounds.contains(newVolume)) return;

	removeLeaf(nodeId);
	node.bounds = newVolume;
	insertLeaf(nodeId);
}

void engine::BVHTree::fixUpwardsTree(unsigned int treeNodeId) {
    while (treeNodeId != BVH_TREE_NULL_ID) {
		BVHTreeNode& treeNode = m_nodes[treeNodeId];

		assert(treeNode.leftNodeId != BVH_TREE_NULL_ID && treeNode.rightNodeId != BVH_TREE_NULL_ID);

		const BVHTreeNode& leftNode = m_nodes[treeNode.leftNodeId];
		const BVHTreeNode& rightNode = m_nodes[treeNode.rightNodeId];
		treeNode.bounds = leftNode.bounds.merge(rightNode.bounds);

		treeNodeId = treeNode.parentNodeId;
	}
}
