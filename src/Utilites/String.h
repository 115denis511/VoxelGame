#ifndef __UTILITES__STRING_H__
#define __UTILITES__STRING_H__

#include "../stdafx.h"

namespace utilites {
    std::string cutStringAfterCharLast(std::string_view str, char cr);
    void replaceAll(std::string& str, std::string_view whatReplace, std::string_view replacer);
}

#endif