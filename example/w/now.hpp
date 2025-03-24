#pragma once

#include <chrono>

namespace w
{
    inline auto now()
    {
        return std::chrono::high_resolution_clock::now();
    }
}