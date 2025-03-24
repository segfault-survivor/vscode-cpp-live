#pragma once

#include <exception>
#include <format>

#include <comdef.h>

namespace w::windows
{
    struct hresult : std::runtime_error
    {
        HRESULT value;

        explicit hresult(HRESULT value) : 
            std::runtime_error{_com_error{value}.ErrorMessage()}, 
            value{value}
        {
        }
    };

    inline auto throw_if_failed(HRESULT hr, char const * msg)
    {
        if (FAILED(hr))
        {
            throw hresult{hr};
        }
    }
}
