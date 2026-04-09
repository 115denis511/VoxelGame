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

        size_t getSize() { return sizeX * sizeY; }
        T* getDataPtr() { return m_data; }

    private:
        T* m_data;
    };

    template<typename T, size_t sizeX, size_t sizeY, size_t sizeZ, size_t additional = 0, size_t align = 0>
    class Array3D {
    public:
        Array3D() {
            if (align == 0) m_data = new T[sizeX * sizeY * sizeZ + additional];
            else m_data = new (std::align_val_t(align)) T[sizeX * sizeY * sizeZ + additional];
        }

        ~Array3D() {
            if (align == 0) delete[] m_data;
            else ::operator delete[](m_data, std::align_val_t(align));
        }

        T& operator () (size_t x, size_t y, size_t z) {
            return m_data[(z * sizeX * sizeY) + (y * sizeX + x)];
        }

        const T& operator () (size_t x, size_t y, size_t z) const {
            return m_data[(z * sizeX * sizeY) + (y * sizeX + x)];
        }

        size_t getSize() { return sizeX * sizeY * sizeZ; }
        T* getDataPtr() { return m_data; }

    private:
        T* m_data;
    };
}

#endif