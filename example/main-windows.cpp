#if __has_include("shader_more.hpp")
    #include "shader_more.hpp"
#endif

#include "shader.hpp"

#include <w/glsw.hpp>
#include <w/now.hpp> 
#include <w/variant.hpp>
#include <w/windows/basic_window.hpp>
#include <w/windows/gdi.hpp>
#include <w/windows/gdi_plus.hpp>
#include <w/windows/wic.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <numeric>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <utility>

#include <cmath>
#include <cstdlib>

constexpr auto application_name = std::string_view{"C++Live example"};

namespace demo
{
    USING_W_GLSW

    inline auto mainImage(vec2 fragCoord)
    {
        auto const uv = (fragCoord - 0.5 * iResolution.xy()) / iResolution.y * 3;
        float a = atan2(uv.x, uv.y);
        float p = smoothstep(1, 0.5, length(uv) + sin(10 * a + iTime * 3) / 3 * sin(iTime * 2));
        return vec4{p};
    }
}

auto const my_shaders = std::array
{
    std::pair{&demo::mainImage, "Demo"},
    // std::pair{&wood_shader_toy::mainImage, "Toy"},
    std::pair{&heat_dissipation::mainImage, "Heat"},
    std::pair{&flying_spiral::mainImage, "Spiral"},
    std::pair{&smooth_sine::mainImage, "Sine"},
    std::pair{&what_is_ray_marching::mainImage, "Raymarching"}
#if defined(CPP_LIVE_HAVE_MORE_SHADERS)
    CPP_LIVE_HAVE_MORE_SHADERS
#endif
};

auto main() -> int
try
{
    ::CoInitialize(0);
    // for (;;)
    {
        auto started_at = w::now();

        auto gdi_plus = w::windows::gdi_plus::init{};

        auto mouse = w::glsw::POINT{};

        auto window = []
        {
            auto const W = 300;
            auto const H = 200;
            auto const X = ::GetSystemMetrics(SM_CXSCREEN) - W;
            auto const Y = ::GetSystemMetrics(SM_CYSCREEN) - H;

            auto const v = w::get_variant_str();

            return w::windows::basic_window
            {
                std::string{application_name} + " [" + v + "]",
                {X, Y, X + W, Y + H},
                WS_EX_TOPMOST | WS_EX_NOACTIVATE,
                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
            };
        }();
        
        auto loader = w::windows::wic::factory{};

        auto load_to_ichannel = [](w::windows::wic::factory & l, w::glsw::sampler2D & s, std::filesystem::path const & p, w::windows::gdi::scoped_dc const & sdc)
        {
            auto const picture = l.load(p, sdc);
            auto [width, height] = picture.size();
            auto const src = picture.dib.ptr();
            auto const cnt = width * height;
            s.data.resize(cnt);
            std::copy(src, src + cnt, s.data.data());
            s.width = width;
            s.height = height;
        };
        
        {
            auto sdc = window.dc();
            // load_to_ichannel(loader, w::glsw::iChannel0, R"path(THE-IMAGE.png)path", sdc);
        }

        window.on_paint = [&](w::windows::gdi::image & output)
        {
            auto graphics = Gdiplus::Graphics{output.dc()};

            auto const [width, height] = window.client_size();

            auto brush = Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 0, 255));
            graphics.FillRectangle(&brush, 0, 0, width, height);

            static auto first_frame = true;
            auto duration = std::vector<std::pair<std::string, std::chrono::nanoseconds>>{};

            auto const w = 100;
            auto const n = width / w;
    #if 0
            auto const v = my_shaders | std::views::drop(0) | std::views::take(100); // This little maneuver's gonna cost us 51 years
    #else
            auto const & v = my_shaders;
    #endif
            auto const whole_window = size(v) == 1;

            unsigned i = -1;
            for (auto f : v)
            {
                ++i;
                auto const x = LONG(i % n * w);
                auto const y = whole_window ? 0 : height - LONG((i / n + 1) * w);

                if (x + w > width || y < 0)
                {
                    continue;
                }

                auto const s = whole_window ? w::glsw::SIZE{width, height} : w::glsw::SIZE{w, w};

                auto const b = w::now();
                auto const time = std::chrono::duration_cast<std::chrono::nanoseconds>(b - started_at).count() / 1e9f;
                w::glsw::render(time, {x, y}, s, f.first, output, mouse);
                auto const e = w::now();

                if (first_frame)
                {
                    duration.emplace_back(f.second, e - b);
                }
            }

            if (first_frame)
            {
                auto const total_time = std::accumulate(begin(duration), end(duration), std::chrono::nanoseconds{}, [](auto a, auto b) { return a + b.second; });
    #if 0
                unsigned i = 1;
                for (auto [n, d] : duration)
                {
                    auto const m = total_time.count() ? 100. * d.count() / total_time.count() : 100;
                    std::println("{:>3}. {:<30} {:>15} {:>10.1f}%", i, n, d, m);
                    ++i;
                }
                std::fflush(stdout);
    #endif
                // output.dib.send_to_clipboard();

                first_frame = false;
            }
        };
        window.on_mouse_move = [&](auto x, auto y)
        {
            auto const [width, height] = window.client_size();
            mouse = {x, height - y};
        };

        while(window)
        {
            window.run_one();
            window.invalidate();
        }
    }

    return EXIT_SUCCESS;
}
catch(std::exception const & e)
{
    w::windows::message_box(nullptr, e.what(), application_name, MB_ICONERROR | MB_OK);

    return EXIT_FAILURE;
}
