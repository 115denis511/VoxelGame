#ifndef __COLLISIONS__I_MERGEABLE_VOLUME__
#define __COLLISIONS__I_MERGEABLE_VOLUME__

namespace engine {
    template<typename T>
    class IMergeableVolume {
    public:
        virtual ~IMergeableVolume() = default;
        virtual T merge(const T& other) const = 0;
    };
}

#endif