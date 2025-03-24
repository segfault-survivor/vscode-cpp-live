#pragma once

#include <stdexcept>

#include <Windows.h>
#include <gdiplus.h>

namespace w::windows::gdi_plus
{
    struct init
    {
        using value_type = ULONG_PTR;
        value_type value;

        static auto create()
        {
            auto result = value_type{};
            auto input = Gdiplus::GdiplusStartupInput{};
            auto error = Gdiplus::GdiplusStartup(&result, &input, nullptr);
            return error == Gdiplus::Status::Ok ? result : throw std::runtime_error("Gdiplus::GdiplusStartup faled");
        }

        init() : value(create())
        {
            
        }
        ~init()
        {
            Gdiplus::GdiplusShutdown(value);
        }
        init(init &&) = delete;
        auto operator=(init &&) = delete;
    };
}
