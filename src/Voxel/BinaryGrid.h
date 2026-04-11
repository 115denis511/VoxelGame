#ifndef __VOXEL__BINARY_GRID_H__
#define __VOXEL__BINARY_GRID_H__

#include "../stdafx.h"

namespace engine {
    template<typename T>
    concept BinaryField = requires(T t) {
        requires std::is_same_v<T, uint8_t> ||
            std::is_same_v<T, uint16_t> ||
            std::is_same_v<T, uint32_t> ||
            std::is_same_v<T, uint64_t>;
    };

    template<BinaryField T>
    class BinaryGrid {
    public:
        static constexpr int SIZE = sizeof(T) * 8;
        static constexpr T LEFT_BIT = 1u << (SIZE - 1);
        static constexpr T RIGHT_BIT = 1u;

        void set(int x, int y, int z, bool val) {
            T mask = val << (SIZE - 1);
            m_grid[y][z] = (m_grid[y][z] & ~(LEFT_BIT >> x)) | (mask >> x);
        }

        void set(int x, int y, int z) {
            T val = LEFT_BIT >> x;
            m_grid[y][z] |= val;
        }

        void unset(int x, int y, int z) {
            T val = ~(LEFT_BIT >> x);
            m_grid[y][z] &= val;
        }

        bool get(int x, int y, int z) const {
            return (bool)(m_grid[y][z] & (LEFT_BIT >> x));
        }

        void setRow(int y, int z, T row) {
            m_grid[y][z] = row;
        }

        T getRow(int y, int z) const {
            return m_grid[y][z];
        }

        void fillAllRows(T row) {
            for (int y = 0; y < SIZE; y++) {
                for (int z = 0; z < SIZE; z++) {
                    m_grid[y][z] = row;
                }
            }
        }

    private:
        T m_grid[SIZE][SIZE]{ 0 };
    };
}

#endif