#if __has_include("shader_more.hpp")
#    include "shader_more.hpp"
#endif

#include "shader.hpp"

#include <w/now.hpp>
#include <w/variant.hpp>

#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <gtkmm/application.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>
#include <gtkmm.h>

#include <vector>

#include <cmath>
#include <cstdint>

namespace demo
{
    USING_W_GLSW

    inline auto mainImage(vec2 fragCoord)
    {
        auto const uv = (fragCoord - 0.5 * iResolution.xy()) / iResolution.y * 3;
        float      a  = atan2(uv.x, uv.y);
        float      p  = smoothstep(1, 0.5, length(uv) + sin(10 * a + iTime * 3) / 3 * sin(iTime * 2));
        return vec4{p};
    }
} // namespace demo

auto const my_shaders = std::array        //
    {                                     //
     std::pair{&demo::mainImage, "Demo"}, //
     // std::pair{&wood_shader_toy::mainImage, "Toy"},
     std::pair{&heat_dissipation::mainImage, "Heat"},
     std::pair{&flying_spiral::mainImage, "Spiral"},
     std::pair{&smooth_sine::mainImage, "Sine"},
     std::pair{&what_is_ray_marching::mainImage, "Raymarching"}
#if defined(CPP_LIVE_HAVE_MORE_SHADERS)
     CPP_LIVE_HAVE_MORE_SHADERS
#endif
    };

class drawing_area : public Gtk::DrawingArea
{
    decltype(w::now()) started_at;
    decltype(w::now()) last_fps_time;
    unsigned           frame_count;
    using fps_callback_type = std::function<void(unsigned)>;
    fps_callback_type  fps_callback;
    std::size_t        current_shader;

public:
    drawing_area() : //
        started_at(w::now()), last_fps_time(started_at), frame_count{}, fps_callback{}, current_shader{}
    {
        Glib::signal_idle().connect(sigc::mem_fun(*this, &drawing_area::on_idle));
        add_events(Gdk::SCROLL_MASK | Gdk::BUTTON_PRESS_MASK);
    }

    void set_fps_callback(fps_callback_type callback)
    {
        fps_callback = std::move(callback);
    }

    auto get_current_shader_index() const
    {
        return current_shader;
    }

protected:
    auto update_current_shader_index(bool forward)
    {
        auto const S   = std::size(my_shaders);
        current_shader = (current_shader + S + forward * 2 - 1) % S;
    }

    bool on_scroll_event(GdkEventScroll * e) override
    {
        update_current_shader_index(e->direction == 1);
        return true;
    }

    bool on_button_press_event(GdkEventButton * e) override
    {
        update_current_shader_index(e->button == 1);
        return true;
    }

    void render_current_shader(auto & s)
    {
        auto const w = s->get_width();
        auto const h = s->get_height();

        if (!w || !h) return;

        struct output_type
        {
            unsigned char * p;
            int             s;
            int             h;

            struct color_convert
            {
                unsigned int * value;

                auto operator=(std::uint32_t c) const
                {
                    *value = 0xff000000 | c;
                }
            };

            color_convert operator[](std::size_t y, std::size_t x)
            {
                y = h - 1 - y;
                auto const o = reinterpret_cast<unsigned int *>(p + y * s);
                return color_convert{o + x};
            }
        } output(s->get_data(), s->get_stride(), h);

        auto const time = std::chrono::duration_cast<std::chrono::nanoseconds>(w::now() - started_at).count() / 1e9f;
        w::glsw::render(time, {0, 0}, {w, h}, my_shaders[current_shader].first, output, {0, 0});
    }

    bool on_draw(Cairo::RefPtr<Cairo::Context> const & c) override
    {
        auto const a = get_allocation();
        auto s = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, a.get_width(), a.get_height());

        render_current_shader(s);

        s->mark_dirty();

        c->set_source(s, 0, 0);
        c->paint();

        update_fps();

        return true;
    }

    void update_fps()
    {
        frame_count++;

        auto const now = w::now();
        auto since_last_fps = std::chrono::duration<double>{now - last_fps_time};
        if (since_last_fps.count() >= .3)
        {
            auto fps = frame_count / since_last_fps.count();
            frame_count = 0;
            last_fps_time = now;
            if (fps_callback)
            {
                fps_callback(fps);
            }
        }
    }

    bool on_idle()
    {
        queue_draw();
        return true;
    }
};

class MainWindow : public Gtk::Window
{
    drawing_area drawing_area;

public:
    MainWindow()
    {
        //signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::on_delete_event));

        // set_title("");
        // set_decorated(false);

        set_keep_above(true);

        // set_accept_focus(false);

        // set_skip_taskbar_hint(true);

        set_default_size(300, 200);

        //signal_map().connect(sigc::mem_fun(*this, &MainWindow::on_map));

        add(drawing_area);
        drawing_area.show();

        drawing_area.set_fps_callback
        (
            [this](auto fps)
            {
                auto const v = w::get_variant_str();
                auto const i = drawing_area.get_current_shader_index();
                auto oss = std::ostringstream{};
                oss 
                    << 
                    (
                        w::get_variant() == w::variant::debug ?
                        std::string{'['} + v + "] " :
                        ""
                    )
                    << my_shaders[i].second << " "
                    << "(" << (i + 1) << "/" << std::size(my_shaders) << ") "
                    << "FPS: " << static_cast<int>(fps);

                this->set_title(oss.str());
            }
        );
    }

protected:
    void on_map() override
    {
        Gtk::Window::on_map();
        position_window();
    }

    bool on_delete_event(GdkEventAny* event) override
    {
        std::exit(0);
        //Gtk::Main::quit();
        return false;
    }

    auto size() const
    {
        auto w  = int{};
        auto h = int{};
        get_size(w, h);
        return std::pair{w, h};
    }

    void position_window()
    {
        auto const a = get_screen()->get_monitor_workarea();
        auto const [w, h] = size();

        auto const x = a.get_x() + a.get_width() - w;
        auto const y = a.get_y() + a.get_height() - h;

        move(x, y);
    }
};

auto on_exception(char const * e)
{
    fprintf(stderr, "C++Live Example: error: %s\n", e);
    return EXIT_FAILURE;
}

auto main(int argc, char * argv[]) /*noexcept*/ -> int
try
{
    auto const app    = Gtk::Application::create(argc, argv, "org.gtkmm.example.cpp-live");
    auto       window = MainWindow{};
    return app->run(window);
}
catch(std::exception const & e)
{
    return on_exception(e.what());
}
catch(...)
{
    return on_exception("unknown");
}
