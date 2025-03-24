#pragma once

#include <w/fatal_error.hpp>
#include <w/now.hpp>
#include <w/ptr.hpp>
#include <w/rc_single.hpp>
#include <w/static_constructor.hpp>
#include <w/static_instance.hpp>
#include <w/windows/gdi.hpp>

#include <functional>
#include <stdexcept>

#include <Windows.h>
#include <Windowsx.h>

namespace w::windows
{
    template<typename T>
    struct atom
    {
        static auto get_class_name()
        {
            return typeid(T).name();
        }
        atom()
        {
            auto c = WNDCLASSA
            {
                .style         = T::info::style(),
                .lpfnWndProc   = &T::wnd_proc,
                .cbClsExtra    = T::info::cls_extra(),
                .cbWndExtra    = T::info::wnd_extra(),
                .hInstance     = T::info::instance(),
                .hIcon         = T::info::icon(),
                .hCursor       = T::info::cursor(),
                .hbrBackground = T::info::background(),
                .lpszMenuName  = T::info::menu_name(),
                .lpszClassName = get_class_name()
            };
            auto const ok = ::RegisterClassA(&c);
            if (ok)
            {
            }
            else
            {
                throw std::runtime_error("::RegisterClassA failed");
            }
        }
        atom(atom &&) = delete;
        atom & operator=(atom &&) = delete;
        ~atom()
        {
            auto const ok = ::UnregisterClassA(get_class_name(), ::GetModuleHandle(nullptr));
            if (ok)
            {
            }
            else
            {
                fatal_error();
            }
        }
    };

    inline auto message_box(HWND parent, auto text, auto title, auto style)
    {
        return ::MessageBoxA(parent, std::string{text}.c_str(), std::string{title}.c_str(), MB_ICONERROR | MB_OK);
    }

    class basic_window
    {
        struct info
        {
            static auto style() -> UINT
            {
                return 0;
            }
            static auto cls_extra() -> int
            {
                return 0;
            }
            static auto wnd_extra() -> int
            {
                return 0;
            }
            static auto instance() -> HINSTANCE
            {
                return ::GetModuleHandle(nullptr);
            }
            static auto icon() -> HICON
            {
                return nullptr;
            }
            static auto cursor() -> HCURSOR
            {
                return ::LoadCursor(nullptr, IDC_ARROW);
            }
            static auto background() -> HBRUSH
            {
                return (HBRUSH)(COLOR_GRAYTEXT + 1);
            }
            static auto menu_name() -> LPCSTR
            {
                return nullptr;
            }
        };

        using atom_type = atom<basic_window>;
        friend atom_type;
        rc_single<atom_type> atom; // static_constructor<static_instance<atom>> atom;

        using raw_ptr = HWND;
        using value_type = ptr
        <
            ptr_default_policy
            <
                [](raw_ptr a)
                {
                    if (::IsWindow(a))
                    {
                        auto const ok = ::DestroyWindow(a);
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
        std::exception_ptr current_exception;
        gdi::image backbuffer;
        unsigned fps = 0;
        decltype(now()) last_fps_time;

        static auto get_this(HWND wnd, UINT m, LPARAM l)
        {
            if (m == WM_NCCREATE)
            {
                auto const s = reinterpret_cast<CREATESTRUCT *>(l);
                auto const result = reinterpret_cast<basic_window*>(s->lpCreateParams);
                ::SetWindowLongPtr(wnd, GWLP_USERDATA, (DWORD_PTR)result);
                return result;
            }
            else
            {
                return reinterpret_cast<basic_window*>(::GetWindowLongPtr(wnd, GWLP_USERDATA));
            }
        }

        static auto get_wnd_proc(HWND wnd)
        {
            return (WNDPROC)::GetWindowLongPtr(wnd, GWLP_WNDPROC);
        }

        constexpr inline static auto has_exception = LRESULT{0xf00df00d};
        static LRESULT CALLBACK wnd_proc(HWND wnd, UINT m, WPARAM w, LPARAM l)
        try
        {
            auto const self = get_this(wnd, m, l);
            // std::println("wnd_proc {} {} {}", m, w, l);
            switch (m)
            {
                case WM_MOUSEMOVE:
                {
                    if (self->on_mouse_move)
                    {
                        auto const x = GET_X_LPARAM(l); 
                        auto const y = GET_Y_LPARAM(l); 
                        self->on_mouse_move(x, y);
                    }
                }
                break;
                case WM_PAINT:
                {
                    if (self->on_paint)
                    {
                        auto const b = now();

                        auto sp = gdi::scoped_paint{self->value.get()};
                        auto & back = self->get_backbuffer();
                        self->on_paint(back);
                        auto const s = std::to_string(self->fps);
                        ::TextOutA(back.dc(), 0, 0, s.data(), s.size());
                        auto size = back.size();
                        ::BitBlt(sp.dc(), 0, 0, size.cx, size.cy, back.dc(), 0, 0, SRCCOPY);

                        auto const e = now();
                        auto const d = std::chrono::duration_cast<std::chrono::nanoseconds>(e - b).count();
                        auto const f = 1'000'000'000 / d;

                        using namespace std::literals;
                        if ((e - self->last_fps_time) > (1s / 3.))
                        {
                            self->last_fps_time = e;
                            self->fps = f;
                        }
                    }
                }
                break;
                case WM_ERASEBKGND:
                {
                    return TRUE;
                }
                break;
            }
            return ::DefWindowProc(wnd, m, w, l);
        }
        catch(...)
        {
            get_this(wnd, m, l)->current_exception = std::current_exception();
            return has_exception;
        }

        static auto is_basic_window(HWND wnd)
        {
            return get_wnd_proc(wnd) == &basic_window::wnd_proc;
        }

        auto rethrow()
        {
            if (current_exception)
            {
                std::rethrow_exception(std::exchange(current_exception, nullptr));
            }
        }

        static auto create(basic_window * self, std::string const & name, RECT r, DWORD ex_style, DWORD style)
        {
            ::AdjustWindowRectEx(&r, style, FALSE, ex_style);

            auto result = ::CreateWindowExA
            (
                ex_style, 
                atom_type::get_class_name(), 
                name.c_str(), 
                style, 
                std::min(r.left, r.right), 
                std::min(r.top, r.bottom), 
                std::abs(r.right - r.left), 
                std::abs(r.bottom - r.top),
                nullptr, // parent
                nullptr, // menu
                nullptr, // hinstance
                self
            );
            return result ? result : throw std::runtime_error("::CreateWindowExA failed") ;
        }

    public:
        std::function<void(gdi::image &)> on_paint;
        std::function<void(int, int)> on_mouse_move;

        explicit basic_window(std::string const & name, RECT r, DWORD ex_style = 0, DWORD style = WS_OVERLAPPEDWINDOW, int cmd_show = SW_SHOW) : 
            value{value_type::borrow, create(this, name, r, ex_style, style)},
            backbuffer(dc(), client_size())
        {
            ::ShowWindow(value.get(), cmd_show); 
            ::UpdateWindow(value.get());
        }

        explicit operator bool() const
        {
            return ::IsWindow(value.get());
        }

        auto invalidate() const
        {
            if (*this)
            {
                auto const ok = ::InvalidateRect(value.get(), nullptr, FALSE);
                if (ok)
                {
                }
                else
                {
                    throw std::runtime_error("::InvalidateRect failed");
                }
            }
        }

        auto dc() const -> gdi::scoped_dc
        {
            return gdi::scoped_dc(value.get());
        }

        auto client_size() const -> SIZE
        {
            auto r = RECT{};
            auto const ok = ::GetClientRect(value.get(), &r);
            auto const [left, top, right, bottom] = r;
            // TODO: GetLastError
            return ok ? SIZE{right, bottom} : throw std::runtime_error("::GetClientRect");
        }

        auto get_backbuffer() -> gdi::image &
        {
            auto const n = client_size();
            auto const o = backbuffer.size();
            if (n.cx == o.cx && n.cy == o.cy)
            {
            }
            else
            {
                backbuffer = gdi::image(dc(), n);
            }
            return backbuffer;
        }

        static auto run_one()
        {
            auto msg = MSG{};
            auto const result = ::GetMessage(&msg, nullptr, 0, 0);

            // std::println("hwnd = {}", std::bit_cast<std::size_t>(msg.hwnd));

            if (result == -1) // 
            {
                // handle the error and possibly exit
            }
            else
            {
                [[maybe_unused]] auto const translate_ok = ::TranslateMessage(&msg); 
                auto const dispatch_result = ::DispatchMessage(&msg);

                auto const wnd = msg.hwnd;
                if (has_exception == dispatch_result && is_basic_window(wnd))
                {
                    get_this(wnd, 0, 0)->rethrow();
                }
            }
        }

        auto message_box(auto text, auto caption, auto style)
        {
            return windows::message_box(value.get(), text, caption, style);
        }
    };
}
