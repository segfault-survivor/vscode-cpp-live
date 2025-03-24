#pragma once

#include <cmath>
#include <utility>

namespace w::math
{
    inline auto lemniscate(auto t)
    {
        auto const s = std::sin(t);
        auto const c = std::cos(t);
        auto const p = std::pow(s, 2);
        auto const x = c / (1 + p);
        auto const y = s * c / (1 + p);
        return std::pair{x, y};
    }
}