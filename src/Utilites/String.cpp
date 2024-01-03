#include "String.h"

std::string utilites::cutStringAfterCharLast(std::string_view str, char cr) {
    int charsToCut = 0;
    for (int i = str.size() - 1; i >= 0; i--) {
        if (str[i] == cr)
            break;
        
        charsToCut++;
    }

    return std::string(str.substr(0, str.size() - charsToCut));
}

void utilites::replaceAll(std::string &str, std::string_view whatReplace, std::string_view replacer) {
    size_t index = 0;
    size_t indexOffset = replacer.size();
    while (true) {
        index = str.find(whatReplace, index);
        if (index == std::string::npos) break;

        str.replace(index, whatReplace.size(), replacer);

        index += indexOffset;
    }
}
