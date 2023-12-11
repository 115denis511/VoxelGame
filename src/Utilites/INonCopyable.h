#ifndef __UTILITES__I_NONCOPYABLE_H__
#define __UTILITES__I_NONCOPYABLE_H__

namespace utilites {
    class INonCopyable {
    public:
        INonCopyable() = default;
                
        INonCopyable(INonCopyable&&) = default;
        INonCopyable& operator = (INonCopyable&&) = default;
                
        INonCopyable(const INonCopyable&) = delete;
        INonCopyable& operator = (const INonCopyable&) = delete;
    };
}

#endif