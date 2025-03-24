#pragma once

#include <cstdlib>

namespace w
{
    [[noreturn]] inline auto fatal_error()
    {
        std::abort();
    }
}