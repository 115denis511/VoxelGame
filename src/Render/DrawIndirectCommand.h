#ifndef __RENDER__DRAW_INDIRECT_COMMAND_H__
#define __RENDER__DRAW_INDIRECT_COMMAND_H__

namespace engine {
    struct DrawElementsIndirectCommand {
        unsigned int  count{ 0 };
        unsigned int  instanceCount{ 0 };
        unsigned int  firstIndex{ 0 };
        int           baseVertex{ 0 };
        unsigned int  baseInstance{ 0 };
    };

    struct DrawArraysIndirectCommand {
        unsigned int  count{ 0 };
        unsigned int  instanceCount{ 0 };
        unsigned int  first{ 0 };
        unsigned int  baseInstance{ 0 };
    };
}

#endif