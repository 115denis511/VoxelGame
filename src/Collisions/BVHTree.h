#ifndef __COLLISIONS__BVH_TREE_H__
#define __COLLISIONS__BVH_TREE_H__

#include "../stdafx.h"
#include "SphereVolume.h"
#include "Frustum.h"
#include "../engine_properties.h"
#include "../Log.h"
#include "../Scene/EntityReferences.h"
#include "../Scene/RenderComponent.h"
#include "../Utilites/ObjectPool.h"

// Сделано на основе следующих материалов
// https://www.azurefromthetrenches.com/introductory-guide-to-aabb-tree-collision-detection/
// https://github.com/JamesRandall/SimpleVoxelEngine/blob/master/voxelEngine/src/AABBTree.h
// https://github.com/JamesRandall/SimpleVoxelEngine/blob/master/voxelEngine/src/AABBTree.cpp
// В отличии от оригинала объёмы AABB заменены на объёмы сфер.
// Исправлен баг допущенный оригинальным автором - во время вставки нового листа автор злоупотребляет
// ссылками на ноды внутри списка для удобства. При добавлении новой ноды через allocateNode() возможно расширение
// списка и потеря валидных ссылок на ноды. Подобное поведение приводило к отрубанию от дерева нод, существовавших 
// до расширения списка.

namespace engine {
    constexpr unsigned int BVH_TREE_NULL_ID = 0xffffffff;

    struct BVHTreeNode {
        SphereVolume bounds;
        EntityReferences* entityRef{ nullptr };
        unsigned int parentNodeId{ BVH_TREE_NULL_ID };
        unsigned int leftNodeId{ BVH_TREE_NULL_ID };
        unsigned int rightNodeId{ BVH_TREE_NULL_ID };
        unsigned int nextNodeId{ BVH_TREE_NULL_ID };

        bool isLeaf() const { return leftNodeId == BVH_TREE_NULL_ID; }
    };

    class BVHTree {
    public:
        BVHTree();

        void insertObject(EntityReferences* entity, const SphereVolume& volume);
        void removeObject(EntityReferences* entity);
        void updateObject(const EntityReferences& object, const SphereVolume& newVolume);
        std::vector<int> getOverlapsedRenderComponents(Frustum frustum);

    private:
        std::vector<BVHTreeNode> m_nodes;
        std::stack<unsigned int> m_nodesToProcess;
        std::mutex m_stackMutex;
        std::mutex m_overlapsMutex;
        std::atomic<bool> m_interruptThreads{ false };
        unsigned int m_rootNodeId{ BVH_TREE_NULL_ID };
        unsigned int m_nextFreeNodeId{ 0 };

        unsigned int allocateNode();
        void deallocateNode(unsigned int nodeId);
        void insertLeaf(unsigned int nodeId);
        void removeLeaf(unsigned int nodeId);
        void updateLeaf(unsigned int nodeId, const SphereVolume& newVolume);
        void fixUpwardsTree(unsigned int treeNodeId);

    };
}

#endif