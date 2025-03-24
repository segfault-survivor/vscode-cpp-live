#pragma once

#include <w/instance_counter.hpp>

namespace w
{
    template<typename T>
    struct static_constructor : private instance_counter<static_constructor<T>>
    {
        using base = instance_counter<static_constructor<T>>;

        static auto should() noexcept
        {
            return base::value == 1;
        }

        static_constructor()
        {
            if (should())
            {
                T::construct();
            }
        }
        static_constructor(static_constructor &&) = delete; // TODO 
        ~static_constructor()
        {
            if (should())
            {
                T::destruct();
            }
        }
    };
}
