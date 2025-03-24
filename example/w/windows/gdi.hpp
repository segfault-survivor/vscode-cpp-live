#pragma once

#include <w/fatal_error.hpp>
#include <w/move_assignment.hpp>
#include <w/ptr.hpp>
#include <w/swap.hpp>

#include <Windows.h>

#include <mdspan>
#include <tuple>
#include <utility>

namespace w::windows::gdi
{
    struct scoped_dc
    {
        using raw_ptr = std::pair<HWND, HDC>;
        using value_type = ptr
        <
            ptr_default_policy
            <
                [](raw_ptr a)
                {
                    auto [w, dc] = a;
                    if (dc && ::IsWindow(w))
                    {
                        auto const one = ::ReleaseDC(w, dc);
                        if (one == 1)
                        {
                        }
                        else
                        {
                            fatal_error();
                        }
                    }
                }
            >
        >;

        value_type value;

        static auto create(HWND w)
        {
            auto const result = ::GetDC(w);
            return result ? raw_ptr{w, result} : throw std::runtime_error("::GetDC failed");
        }
        explicit scoped_dc(HWND w) : value{value_type::borrow, create(w)}
        {
        }

        auto get() const
        {
            return value.get().second;
        }
    };

    struct scoped_paint
    {
        using value_type = std::pair<PAINTSTRUCT, HDC>;
        HWND w;
        value_type value;
        static auto create(HWND w)
        {
            auto ps = PAINTSTRUCT{};
            auto const dc = ::BeginPaint(w, &ps);
            return value_type{ps, dc};
        }
        explicit scoped_paint(HWND w) : w{w}, value{create(w)}
        {

        }
        ~scoped_paint()
        {
            auto const ok = ::EndPaint(w, &value.first);
            if (ok)
            {

            }
            else
            {
                fatal_error();
            }
        }
        scoped_paint(scoped_paint &&) = delete;
        auto operator=(scoped_paint &&) = delete;

        auto dc() const
        {
            return value.second;
        }
    };

    struct scoped_select_object
    {
        using raw_ptr = std::pair<HDC, HGDIOBJ>;

        static auto create(HDC dc, HGDIOBJ n) -> raw_ptr
        {
            auto const result = ::SelectObject(dc, n);
            // TODO: regions
            // TODO: error handling
            return result ? raw_ptr{dc, result} : throw std::runtime_error("::SelectObject failed");
        }

        using value_type = ptr
        <
            ptr_default_policy
            <
                [](raw_ptr a)
                {
                    auto [dc, obj] = a;
                    if (dc)
                    {
                        create(dc, obj);
                    }
                }
            >
        >;

        value_type value;

        scoped_select_object(HDC dc, HGDIOBJ n) : value{value_type::borrow, create(dc, n)}
        {
        }
    
        // W_DEFINE_SWAP(scoped_select_object, value)
    };

    template<typename T>
    struct scoped_gdi_object
    {
        using raw_ptr = T;
        using value_type = ptr
        <
            ptr_default_policy
            <
                [](raw_ptr a)
                {
                    if (a)
                    {
                        auto const ok = ::DeleteObject(a);
                        if (ok)
                        {
                        }
                        else
                        {
                            fatal_error();
                        }
                    }
                }
            >
        >;

        value_type value;

        explicit scoped_gdi_object(raw_ptr value) : value{value_type::borrow, value}
        {
        }

        // W_DEFINE_SWAP(scoped_gdi_object, value)

        auto get() const noexcept
        {
            return value.get();
        }
    };

    struct scoped_ddb
    {
        using value_type = scoped_gdi_object<HBITMAP>;
        value_type value;

        static auto create(HDC dc, int w, int h)
        {
            auto const result = ::CreateCompatibleBitmap(dc, w, h);
            // TODO: error handling
            return result ? result : throw std::runtime_error("::CreateCompatibleBitmap failed");
        }

        scoped_ddb(HDC dc, int w, int h) : value{create(dc, w, h)}
        {
        }
    };

    struct scoped_compatible_dc
    {
        using raw_ptr = HDC;
        using value_type = ptr
        <
            ptr_default_policy
            <
                [](raw_ptr a)
                {
                    if (a)
                    {
                        auto const ok = ::DeleteDC(a);
                        if (ok)
                        {

                        }
                        else
                        {
                            fatal_error();
                        }
                    }
                }
            >
        >;

        value_type value;

        static auto create(HDC dc)
        {
            auto const result = ::CreateCompatibleDC(dc);
            return result ? result : throw std::runtime_error("::CreateCompatibleDC failed");
        }

        explicit scoped_compatible_dc(HDC dc) : value{value_type::borrow, create(dc)}
        {
        }
       
        // W_DEFINE_SWAP(scoped_compatible_dc, value)
    };

    struct scoped_dib
    {
        using bits_type = std::uint32_t;
        using bits_type_ptr = bits_type *;
        using value_type = std::pair<scoped_gdi_object<HBITMAP>, bits_type_ptr>;
        value_type value;

        static auto create_bitmap_info(SIZE s)
        {
            return BITMAPINFO
            {
                .bmiHeader = 
                {
                    .biSize = sizeof(BITMAPINFOHEADER),
                    .biWidth = s.cx,
                    .biHeight = s.cy,
                    .biPlanes = 1,
                    .biBitCount = 32,
                    .biCompression = BI_RGB,
                    .biSizeImage = 0,
                    .biXPelsPerMeter = 0,
                    .biYPelsPerMeter = 0,
                    .biClrUsed = 0,
                    .biClrImportant = 0
                }
            };
        }

        static auto create(HDC dc, SIZE s)
        {
            auto const info = create_bitmap_info(s);
            auto bits = bits_type_ptr{};
            auto const bitmap = ::CreateDIBSection
            (
                dc,
                &info,
                DIB_RGB_COLORS,
                reinterpret_cast<VOID **>(&bits),
                nullptr,
                0
              );
              
            // TODO: error handling, use GetLastError
            return bitmap ? std::make_pair(bitmap, bits) : throw std::runtime_error("::CreateCompatibleBitmap failed");
        }

        scoped_dib(HDC dc, SIZE s) : value{create(dc, s)}
        {
        }

        auto ptr() const
        {
            return value.second;
        }

        auto info() const
        {
            auto result = DIBSECTION{};
            ::GetObject(value.first.get(), sizeof(result), &result); // TODO: error handling
            return result;
        }

        auto size() const
        {
            auto const i = info();
            return SIZE{i.dsBmih.biWidth, std::abs(i.dsBmih.biHeight)};
        }

        auto send_to_clipboard() const
        {
            auto const i = info();

            auto const h = i.dsBmih;

            auto const s = h.biSizeImage;

            auto const b = i.dsBm.bmBits;

            auto global = std::unique_ptr<std::remove_pointer_t<HGLOBAL>, decltype(&::GlobalFree)>
            {
                ::GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(BITMAPINFOHEADER) + s),
                &::GlobalFree
            };

            auto dst = std::unique_ptr<void, decltype(&::GlobalUnlock)>
            {
                ::GlobalLock(global.get()),
                &::GlobalUnlock
            };

            memcpy(dst.get(), &h, sizeof(h));
            memcpy((BITMAPINFOHEADER*)dst.get() + 1, b, s);

            if(!::OpenClipboard(nullptr))
            {
                throw std::runtime_error("::OpenClipboard failed");
            }

            ::EmptyClipboard();
            ::SetClipboardData(CF_DIB, global.release());
            ::CloseClipboard();
        }
    };

    struct image
    {
        scoped_compatible_dc cdc;
        scoped_dib dib;
        scoped_select_object sel;
        decltype(std::mdspan(dib.ptr(), 0, 0)) data;

        image(scoped_dc const & sdc, SIZE s) : 
            cdc{sdc.get()}, 
            dib{sdc.get(), s}, 
            sel{dc(), dib.value.first.get()}, 
            data{dib.ptr(), s.cy, s.cx}
        {
        }
        image(image && other) noexcept = default;
        W_DEFINE_MOVE_ASSIGNMENT(image)
        W_DEFINE_SWAP(image, cdc, dib, sel, data)

        decltype(auto) operator[](this auto && self, std::size_t x, std::size_t y)
        {
            return self.data[x, y];
        }

        auto size() const
        {
            return dib.size();
        }

        auto dc() const noexcept -> HDC
        {
            return cdc.value.get();
        }
    };
}