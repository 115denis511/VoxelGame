#ifndef __UTILITES__MULTIDIMENSIONAL_ARRAYS_H__
#define __UTILITES__MULTIDIMENSIONAL_ARRAYS_H__

namespace utilites {
    template<typename T, size_t sizeX, size_t sizeY>
    class Array2D {
    public:
        Array2D() 
        :   m_data(new T[sizeX * sizeY]) {}

        ~Array2D() {
            delete[] m_data;
        }

        T& operator () (size_t x, size_t y) {
            return m_data[y * sizeX + x];
        }

        const T& operator () (size_t x, size_t y) const {
            return m_data[y * sizeX + x];
        }

    private:
        T* m_data;
    };

    template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ>
    class Array3D {
    public:
        Array3D() 
        :   m_data(new T[sizeX * sizeY * sizeZ]) {}

        ~Array3D() {
            delete[] m_data;
        }

        T& operator () (size_t x, size_t y, size_t z) {
            return m_data[(z * sizeX * sizeY) + (y * sizeX + x)];
        }

        const T& operator () (size_t x, size_t y, size_t z) const {
            return m_data[(z * sizeX * sizeY) + (y * sizeX + x)];
        }

    private:
        T* m_data;
    };
}

#endif