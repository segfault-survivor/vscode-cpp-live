#pragma once

namespace w
{
    enum class variant
    {
        debug,
        release
    };
    inline auto get_variant()
    {
        return variant::
#if defined(NDEBUG)
            release
#else
            debug
#endif
            ;
    }

    inline auto get_variant_str()
    {
        switch(get_variant())
        {
            case variant::release:
                return "Release";
            case variant::debug:
                return "Debug";
            default:
                return "Unknown";
        }
    }
}
