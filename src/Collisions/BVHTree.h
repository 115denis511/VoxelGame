#ifndef __COLLISIONS__BVH_TREE_H__
#define __COLLISIONS__BVH_TREE_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "../Log.h"
#include "../Utilites/ObjectPool.h"
#include "IMergeableVolume.h"

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

    template <typename Item, typename Volume>
    class BVHTree;
    template <typename Item, typename Volume>
    class IBVHTreeItem {
        friend BVHTree<Item, Volume>;

    private:
        unsigned int m_BVHNodeId{ 0xffffffff };
    };

    template <typename Item, typename Volume>
    struct BVHTreeNode {
        Volume bounds;
        Item* entityRef{ nullptr };
        unsigned int parentNodeId{ BVH_TREE_NULL_ID };
        unsigned int leftNodeId{ BVH_TREE_NULL_ID };
        unsigned int rightNodeId{ BVH_TREE_NULL_ID };
        unsigned int nextNodeId{ BVH_TREE_NULL_ID };

        bool isLeaf() const { return leftNodeId == BVH_TREE_NULL_ID; }
    };

    template <typename Item, typename Volume>
    class BVHTree {
    public:
        static_assert(
            std::is_base_of<IBVHTreeItem<Item, Volume>, Item>::value, 
            "Class <Item> must inherit from class IBVHTreeItem<Item, Volume>"
        );
        static_assert(
            std::is_base_of<IMergeableVolume<Volume>, Volume>::value, 
            "Class <Volume> must inherit from class IMergeableVolume<Volume>"
        );

        BVHTree() {
            m_nodes.resize(engine_properties::SCENE_MAX_ENTITIES);
            for (size_t i = 0; i < m_nodes.size() - 1; i++) {
                m_nodes[i].nextNodeId = i + 1;
            }
            m_nodes[m_nodes.size() - 1].nextNodeId = BVH_TREE_NULL_ID;
            m_nextFreeNodeId = 0;
        }

        void insertObject(Item* entity, const Volume& volume) {
            unsigned nodeId = allocateNode();
            BVHTreeNode<Item, Volume>& node = m_nodes[nodeId];

            node.bounds = volume;
            node.entityRef = entity;
            entity->m_BVHNodeId = nodeId;

            insertLeaf(nodeId);
        }

        void removeObject(Item* entity) {
            unsigned nodeId = entity->m_BVHNodeId;
            removeLeaf(nodeId);
            deallocateNode(nodeId);
        }

        void updateObject(const Item& object, const Volume& newVolume) {
            updateLeaf(object.m_BVHNodeId, newVolume);
        }

    protected:
        std::vector<BVHTreeNode<Item, Volume>> m_nodes;
        std::stack<unsigned int> m_nodesToProcess;
        unsigned int m_rootNodeId{ BVH_TREE_NULL_ID };
        unsigned int m_nextFreeNodeId{ 0 };

        unsigned int allocateNode() {
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
            BVHTreeNode<Item, Volume>& newNode = m_nodes[newNodeId];

            newNode.parentNodeId = BVH_TREE_NULL_ID;
            newNode.leftNodeId = BVH_TREE_NULL_ID;
            newNode.rightNodeId = BVH_TREE_NULL_ID;
            newNode.entityRef = nullptr;
            m_nextFreeNodeId = newNode.nextNodeId;

            return newNodeId;
        }

        void deallocateNode(unsigned int nodeId) {
            m_nodes[nodeId] = BVHTreeNode<Item, Volume>();
            m_nodes[nodeId].nextNodeId = m_nextFreeNodeId;
            m_nextFreeNodeId = nodeId;
        }

        void insertLeaf(unsigned int nodeId) {
            assert(m_nodes[nodeId].parentNodeId == BVH_TREE_NULL_ID);
            assert(m_nodes[nodeId].leftNodeId == BVH_TREE_NULL_ID);
            assert(m_nodes[nodeId].rightNodeId == BVH_TREE_NULL_ID);

            if (m_rootNodeId == BVH_TREE_NULL_ID) {
                m_rootNodeId = nodeId;
                return;
            }

            unsigned int treeNodeId = m_rootNodeId;
            BVHTreeNode<Item, Volume>& leafNode = m_nodes[nodeId];
            while (!m_nodes[treeNodeId].isLeaf()) {
                const BVHTreeNode<Item, Volume>& treeNode = m_nodes[treeNodeId];
                const BVHTreeNode<Item, Volume>& leftNode = m_nodes[treeNode.leftNodeId];
                const BVHTreeNode<Item, Volume>& rightNode = m_nodes[treeNode.rightNodeId];

                Volume combinedBounds = treeNode.bounds.merge(leafNode.bounds);

                float newParentNodeCost = 2.0f * combinedBounds.getArea();
                float minimumPushDownCost = 2.0f * (combinedBounds.getArea() - treeNode.bounds.getArea());

                float costLeft;
                float costRight;
                if (leftNode.isLeaf()) {
                    costLeft = leafNode.bounds.merge(leftNode.bounds).getArea() + minimumPushDownCost;
                }
                else {
                    Volume newLeftBounds = leafNode.bounds.merge(leftNode.bounds);
                    costLeft = (newLeftBounds.getArea() - leftNode.bounds.getRadius()) + minimumPushDownCost;	
                }

                if (rightNode.isLeaf()) {
                    costRight = leafNode.bounds.merge(rightNode.bounds).getArea() + minimumPushDownCost;
                }
                else {
                    Volume newRightBounds = leafNode.bounds.merge(rightNode.bounds);
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

            BVHTreeNode<Item, Volume>& createdleafNode = m_nodes[nodeId];
            unsigned int leafSiblingId = treeNodeId;
            BVHTreeNode<Item, Volume>& leafSibling = m_nodes[leafSiblingId];
            unsigned int oldParentIndex = leafSibling.parentNodeId;

            BVHTreeNode<Item, Volume>& newParent = m_nodes[newParentIndex];
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
                BVHTreeNode<Item, Volume>& oldParent = m_nodes[oldParentIndex];
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

        void removeLeaf(unsigned int nodeId) {
            if (nodeId == m_rootNodeId) {
                m_rootNodeId = BVH_TREE_NULL_ID;
                return;
            }

            BVHTreeNode<Item, Volume>& leafNode = m_nodes[nodeId];
            unsigned int parentNodeId = leafNode.parentNodeId;
            const BVHTreeNode<Item, Volume>& parentNode = m_nodes[parentNodeId];
            unsigned int grandParentNodeId = parentNode.parentNodeId;
            unsigned int siblingNodeId = parentNode.leftNodeId == nodeId ? parentNode.rightNodeId : parentNode.leftNodeId;
            assert(siblingNodeId != BVH_TREE_NULL_ID);
            BVHTreeNode<Item, Volume>& siblingNode = m_nodes[siblingNodeId];

            if (grandParentNodeId != BVH_TREE_NULL_ID) {
                // Если имеется прапредок, то удаляем предка и прикрепляем соседа к прапредку вместо него
                BVHTreeNode<Item, Volume>& grandParentNode = m_nodes[grandParentNodeId];
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

        void updateLeaf(unsigned int nodeId, const Volume& newVolume) {
            BVHTreeNode<Item, Volume>& node = m_nodes[nodeId];

            if (node.bounds.contains(newVolume)) return;

            removeLeaf(nodeId);
            node.bounds = newVolume;
            insertLeaf(nodeId);
        }

        void fixUpwardsTree(unsigned int treeNodeId) {
            while (treeNodeId != BVH_TREE_NULL_ID) {
                BVHTreeNode<Item, Volume>& treeNode = m_nodes[treeNodeId];

                assert(treeNode.leftNodeId != BVH_TREE_NULL_ID && treeNode.rightNodeId != BVH_TREE_NULL_ID);

                const BVHTreeNode<Item, Volume>& leftNode = m_nodes[treeNode.leftNodeId];
                const BVHTreeNode<Item, Volume>& rightNode = m_nodes[treeNode.rightNodeId];
                treeNode.bounds = leftNode.bounds.merge(rightNode.bounds);

                treeNodeId = treeNode.parentNodeId;
            }
        }

    };
}

#endif