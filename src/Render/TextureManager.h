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
        ///        альфа канале находится канал второй, выбранный на основе маски. Путь к первой текстуре используется как название
        ///        для итоговой текстуры. Данный вариант функции добавлеет в менеджер уже загруженные
        ///        сторонним образом сырые данные текстур.
        /// @param alphaMask Маска для выбора копируемого канала. Желательно должен представлять вектор, где все значения
        ///                  кроме копируемого равны нулю. Пример выбора синего канала: vec4(0.f, 0.f, 1.f, 0.f);
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addMixedTexture_RGB_A(
                std::string pathRGB, glm::vec4 alphaMask,
                unsigned char* rgbImage, int rgbWidth, int rgbHeight, int rgbNrComponents,
                unsigned char* alphaImage, int alphaWidth, int alphaHeight, int alphaNrComponents);

        /// @brief Добавляет смешанную текстуру, где в RGB канале находятся RGB данные первой тестуры, а в
        ///        альфа канале находится канал второй, выбранный на основе маски. Путь к первой текстуре используется как название
        ///        для итоговой текстуры. Данный вариант функции загружает текстуры из указанных местоположений на диске.
        /// @param alphaMask Маска для выбора копируемого канала. Желательно должен представлять вектор, где все значения
        ///                  кроме копируемого равны нулю. Пример выбора синего канала: vec4(0.f, 0.f, 1.f, 0.f);
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addMixedTexture_RGB_A(std::string pathRGB, std::string pathAlpha, glm::vec4 alphaMask);

        /// @brief Добавляет смешанную текстуру, где в RGB канале находятся RGB данные первой тестуры, а в
        ///        альфа канале находится канал второй, выбранный на основе маски. Путь к первой текстуре используется как название
        ///        для итоговой текстуры. Данный вариант функции загружает текстуру из данных ASSIMP.
        /// @param alphaMask Маска для выбора копируемого канала. Желательно должен представлять вектор, где все значения
        ///                  кроме копируемого равны нулю. Пример выбора синего канала: vec4(0.f, 0.f, 1.f, 0.f);
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addMixedTexture_RGB_A(const aiTexture* dataRGB, const aiTexture* dataAlpha, std::string name, glm::vec4 alphaMask);

        /// @brief Загрузка только цветовых каналов текстуры. Альфа канал заливается нулями.
        ///        Данный вариант функции добавлеет в менеджер уже загруженные
        ///        сторонним образом сырые данные текстур.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addTexture_RGB(
            std::string path, unsigned char* image, int width, int height, int nrComponents);

        /// @brief Загрузка только цветовых каналов текстуры. Альфа канал заливается нулями.
        ///        Данный вариант функции загружает текстуру из указанного местоположения на диске.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addTexture_RGB(std::string path);

        /// @brief Загрузка только цветовых каналов текстуры. Альфа канал заливается нулями.
        ///        Данный вариант функции загружает текстуру из данных ASSIMP.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addTexture_RGB(const aiTexture* data, std::string name);

        /// @brief Загрузка текстуры из загруженных сторонним образом сырых данных. Путь к текстуре ипользуется как её название.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef addTexture(
            std::string path, unsigned char* image, int width, int height, int nrComponents);
        
        /// @brief Загрузка текстуры с диска по указанному пути. Путь к текстуре ипользуется как её название.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки). 
        static const TextureArrayRef addTexture(std::string path);

        /// @brief Загрузка текстуры из данных ASSIMP.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки). 
        static const TextureArrayRef addTexture(const aiTexture* data, std::string name);

        /// @brief Создание текстуры, залитой определённым цветом.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на добавленную текстуру. ЕСЛИ ТЕКСТУРУ 
        ///         ЗАГРУЗИТЬ НЕ УДАЛОСЬ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки). 
        static const TextureArrayRef addTexture(glm::vec4 color, int width, int height, std::string name);

        /// @brief Получить ссылку на текстуру с данным именем.
        /// @return Возвращает объект класса TextureArrayRef, предствляющего ссылку на текстуру. ЕСЛИ ТЕКСТУРЫ 
        ///         С ДАННЫЕМ ИМЕНЕМ НЕ СУЩЕСТВУЕТ, ТО ВОЗВРАЩАЕТ НЕВАЛИДНУЮ ССЫЛКУ (TextureArrayRef::isValidRef() для проверки).
        static const TextureArrayRef getTexture(std::string name);

        /// @brief Возвращает текстуру-плейсхолдер залитую чёрным цветом 
        static const TextureArrayRef getPlaceholderBlack();

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
        static Shader*                    g_shaderFill_RGB;
        static Mesh*                      g_fillRect;
        static TextureArrayRef            g_placeholderBlack;
        static std::unordered_map<std::string, TextureArrayRef> g_textureDict;
        static std::vector<TextureArrayList*>                   g_textureArrayList;
        
        static bool init(Shader* shaderMix_RGB_A, Shader* shaderFill_RGB, Mesh* fillRect);
        static void freeResources();
    };
}

#endif