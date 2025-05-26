#pragma once
#include "BaseType.h"

namespace TinyRHI
{
    class UniqueHash
    {
    public:
        UniqueHash()
        {
            hashId = nextId++;
        }

        Uint32 Hash()
        {
            return hashId;
        }

    protected:

        static Uint32 nextId;
        Uint32 hashId;
    };
    inline Uint32 UniqueHash::nextId = 0;

} // namespace TinyRHI
