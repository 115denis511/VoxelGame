#ifndef __UTILITES__OBJECT_POOL_H__
#define __UTILITES__OBJECT_POOL_H__

#include <cmath>

namespace utilites {
    template<typename T, size_t size>
    class ObjectPool {
    public:
        class Object {
            friend ObjectPool;
        public:
            Object() { m_next = -1; }

            T& getObject() { return m_object; }
            bool isInUse() { return m_isInUse; }

        private:
            bool m_isInUse{ false };

            union {
                T m_object;
                int m_next;
            };
        };

        ObjectPool() {
            m_firstAvailable = 0;

            for (size_t i = 0; i < size - 1; i++) {
                m_list[i].m_next = i + 1;
            }

            m_list[size - 1].m_next = -1;
        }

        bool add(const T& object) {
            if (m_firstAvailable == -1) {
                return false;
            }

            Object* newObject = &m_list[m_firstAvailable];
            m_biggestUsedId = std::max(m_biggestUsedId, m_firstAvailable);
            m_firstAvailable = newObject->m_next;

            newObject->m_isInUse = true;
            newObject->m_object = object;

            return true;
        }

        bool add(const T& object, int& returnId) {
            returnId = m_firstAvailable;
            return add(object);
        }

        void remove(int id) {
            if (!m_list[id].m_isInUse) {
                return;
            }

            m_list[id].m_isInUse = false;
            m_list[id].m_next = m_firstAvailable;
            m_firstAvailable = id;
        }

        bool isHaveSpace() {
            return m_firstAvailable != -1;
        }

        Object& get(int id) {
            return m_list[id];
        }

        int getBiggestUsedId() {
            return m_biggestUsedId;
        }

    private:
        int m_firstAvailable{ -1 };
        int m_biggestUsedId{ -1 };
        Object m_list[size];
    };
}

#endif