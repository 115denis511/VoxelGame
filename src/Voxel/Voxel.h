#ifndef __VOXEL__VOXEL_H__
#define __VOXEL__VOXEL_H__

#include "../stdafx.h"

namespace engine {
    class Voxel {
    public:
        Voxel() :
            m_solidId(NO_VOXEL),
            m_solidSize(0), 
            m_liquidId(NO_VOXEL),
            m_liquidSize(0)
        {}

        void setSolidId(uint8_t id) { m_solidId = id; }
        void setSolidSize(uint8_t size) { m_solidSize = (m_solidSize & (~SIZE_MASK)) | (size & SIZE_MASK); }
        void setSolidVoxel(uint8_t id, uint8_t size) {
            setSolidId(id);
            setSolidSize(size);
        }
        bool isHaveSolid() { return m_solidId != NO_VOXEL; };
        uint8_t getSolidId() { return m_solidId; }
        uint8_t getSolidSize() { return m_solidSize & SIZE_MASK; }
        void deleteSolid() { m_solidId = NO_VOXEL; }

        void setLiquidId(uint8_t id) { m_liquidId = id; }
        void setLiquidSize(uint8_t size) { m_liquidSize = (m_liquidSize & (~SIZE_MASK)) | (size & SIZE_MASK); }
        void setLiquidVoxel(uint8_t id, uint8_t size) {
            setLiquidId(id);
            setLiquidSize(size);
        }
        bool isHaveLiquid() { return m_liquidId != NO_VOXEL; };
        uint8_t getLiquidId() { return m_liquidId; }
        uint8_t getLiquidSize() { return m_liquidSize & SIZE_MASK; }
        void deleteLiquid() { m_liquidId = NO_VOXEL; }

        enum class Type {
            SOLID = 0,
            LIQUID = 2
        };
        void setId(Type type, uint8_t id) { m_fieldsArray[static_cast<int>(type)] = id; }
        void setSize(Type type, uint8_t size) {
            int field = static_cast<int>(type) + 1;
            m_fieldsArray[field] = (m_fieldsArray[field] & (~SIZE_MASK)) | (size & SIZE_MASK);
        }
        void setVoxel(Type type, uint8_t id, uint8_t size) {
            setId(type, id);
            setSize(type, size);
        }
        uint8_t getId(Type field) { return m_fieldsArray[static_cast<int>(field)]; }
        uint8_t getSize(Type field) { return m_fieldsArray[static_cast<int>(field) + 1] & SIZE_MASK; }

        uint32_t dbg_getRaw() { return m_raw; }
        uint8_t dbg_getSolidIdFromRaw() { return m_raw & 0xFF; }
        uint8_t dbg_getSolidSizeFromRaw() { return (m_raw >> 8) & 0b111; }
        uint8_t dbg_getLiquidIdFromRaw() { return (m_raw >> 16) & 0xFF; }
        uint8_t dbg_getLiquidSizeFromRaw() { return (m_raw >> 24) & 0b111; }

    private:
        static constexpr uint8_t NO_VOXEL = 0xFF;
        static constexpr uint8_t SIZE_MASK = 0b111;
        
        union {
            struct {
                uint8_t m_solidId;
                uint8_t m_solidSize;
                uint8_t m_liquidId;
                uint8_t m_liquidSize;
            };
            uint8_t m_fieldsArray[4];
            uint32_t m_raw;
        };
    };
}

#endif
