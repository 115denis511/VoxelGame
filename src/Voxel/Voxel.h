#ifndef __VOXEL__VOXEL_H__
#define __VOXEL__VOXEL_H__

#include "../stdafx.h"

namespace engine {
    class Voxel {
    public:
        Voxel() :
            m_solidId(NO_VOXEL),
            m_solidSize(0), 
            m_waterId(NO_VOXEL),
            m_waterSize(0)
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

        void setWaterId(uint8_t id) { m_waterId = id; }
        void setWaterSize(uint8_t size) { m_waterSize = (m_waterSize & (~SIZE_MASK)) | (size & SIZE_MASK); }
        void setWaterVoxel(uint8_t id, uint8_t size) {
            setWaterId(id);
            setWaterSize(size);
        }
        bool isHaveWater() { return m_waterId != NO_VOXEL; };
        uint8_t getWaterId() { return m_waterId; }
        uint8_t getWaterSize() { return m_waterSize & SIZE_MASK; }
        void deleteWater() { m_waterId = NO_VOXEL; }

        uint32_t dbg_getRaw() { return m_raw; }
        uint8_t dbg_getSolidIdFromRaw() { return m_raw & 0xFF; }
        uint8_t dbg_getSolidSizeFromRaw() { return (m_raw >> 8) & 0b111; }
        uint8_t dbg_getWaterIdFromRaw() { return (m_raw >> 16) & 0xFF; }
        uint8_t dbg_getWaterSizeFromRaw() { return (m_raw >> 24) & 0b111; }

    private:
        static constexpr uint8_t NO_VOXEL = 0xFF;
        static constexpr uint8_t SIZE_MASK = 0b111;
        
        union {
            struct {
                uint8_t m_solidId;
                uint8_t m_solidSize;
                uint8_t m_waterId;
                uint8_t m_waterSize;
            };
            uint32_t m_raw;
        };
    };
}

#endif
