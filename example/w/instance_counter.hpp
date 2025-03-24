#pragma once

#include <cstdlib>

namespace w
{
    template<typename T>
    struct instance_counter
    {
        inline static std::size_t value;

        instance_counter() noexcept
        {
            ++value;
        }
        instance_counter(instance_counter const &) noexcept
        {
            ++value;
        }
        instance_counter(instance_counter &&) noexcept
        {
            ++value;
        }
        ~instance_counter()
        {
            --value;
        }
    };
}
