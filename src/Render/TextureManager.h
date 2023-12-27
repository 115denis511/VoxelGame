#ifndef __RENDER__TEXTURE_MANAGER_H__
#define __RENDER__TEXTURE_MANAGER_H__

#include "../stdafx.h"
#include "../engine_properties.h"
#include "../WindowGLFW.h"
#include "TextureArray.h"
#include "Mesh.h"
#include "Shader.h"

namespace engine {
    class Render;

    class TextureManager {
        // Только Render может вызывать init и freeResources
        friend Render;

    public:
        /// @brief Добавляет смешанную текстуру, где в RGB канале находятся RGB данные первой тестуры, а в
        ///        альфа канале находится RED канал второй. Путь к первой текстуре используется как название
        ///        для итоговой текстуры. Данный вариант функции добавлеет в менеджер уже загруженные
        ///        сторонним образом сырые данные текстур.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addMixedTexture_RGB_A(
                std::string pathRGB,
                unsigned char* rgbImage, int rgbWidth, int rgbHeight, int rgbNrComponents,
                unsigned char* alphaImage, int alphaWidth, int alphaHeight, int alphaNrComponents);

        /// @brief Добавляет смешанную текстуру, где в RGB канале находятся RGB данные первой тестуры, а в
        ///        альфа канале находится RED канал второй. Путь к первой текстуре используется как название
        ///        для итоговой текстуры. Данный вариант функции загружает текстуры из указанных местоположений на диске.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha);

        /// @brief Загрузка текстуры из загруженных сторонним образом сырых данных. Путь к текстуре ипользуется как её название.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addTexture(
            std::string path, unsigned char* image, int width, int height, int nrComponents);
        
        /// @brief Загрузка текстуры с диска по указанному пути. Путь к текстуре ипользуется как её название.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки). 
        static const TextureArrayRef addTexture(std::string path);

        /// @brief Получить ссылку на текстуру с данным именем.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на текстуру. ЕСЛИ ТЕКСТУРЫ 
        ///         С ДАННЫЕМ ИМЕНЕМ НЕ СУЩЕСТВУЕТ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef getTexture(std::string name);

        /// @brief Обновить мипмапы и резидентность для текстур. ОБЯЗАТЕЛЬНО ДЕЛАТЬ ПОСЛЕ ДОБАВЛЕНИЯ НАБОРА ТЕКСТУР.
        static void updateMipmapsAndMakeResident();

    private:
        TextureManager() {}

        /// Технический класс для автоматизация добавления и удаления списков текстурных массивов
        /// разных размеров. Содержит все данные о массивах одинаковой размерности и текстурные объекты для вклейки
        /// новых текстур в список.
        class TextureArrayList : public utilites::INonCopyable {
        public:
            TextureArrayList(GLuint width, GLuint height, GLuint maxCapacity);
            ~TextureArrayList();

            bool addTextureArray();
            bool isEqualSize(int width, int height);
            void loadTextureFromRaw(GLuint textureUnit, unsigned char* rawImage, int components);

            GLuint                      m_width;
            GLuint                      m_height;
            GLuint                      m_maxCapacity;
            GLuint                      m_textureTempA;
            GLuint                      m_textureTempB;
            std::vector<TextureArray*>  m_textureArrays;
        };

        static GLuint                     g_fboTexCopy;
        static Shader*                    g_shaderMix_RGB_A;
        static Mesh*                      g_fillRect;
        static std::unordered_map<std::string, TextureArrayRef> g_textureDict;
        static std::vector<TextureArrayList*>                   g_textureArrayList;
        
        static bool init(Shader* shaderMix_RGB_A, Mesh* fillRect);
        static void freeResources();
    };
}

#endif