#pragma once

#include <optional>

namespace w
{
    template<typename T>
    struct static_instance
    {
        inline static std::optional<T> value;

        static auto construct()
        {
            value.emplace();
        }
        static auto destruct()
        {
            value.reset();
        }
    };
}
