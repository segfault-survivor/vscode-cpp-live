#pragma once

#include <w/operators.hpp>

#include <algorithm>
#include <bit>
#include <chrono>
#include <execution>
#include <stdexcept>
#include <numbers>
#include <ranges>
#include <utility>
#include <vector>

#include <cmath>

#define USING_W_GLSW         \
    using namespace w::glsw; \
    using w::glsw::abs;      \
    using w::glsw::sin;      \
    using w::glsw::cos;      \
    using w::glsw::tanh;     \
    using w::glsw::atan;     \
    using w::glsw::exp;      \
    using w::glsw::floor;    \
    using w::glsw::log;      \
    using w::glsw::ceil;     \
    using w::glsw::min;      \
    using w::glsw::max;      \
    using w::glsw::pow;      \
    using w::glsw::round;    \
    using w::glsw::sqrt;     \
    /**/

#define W_GLSL_SHADER(name) std::pair{&name::mainImage, #name}

namespace w::glsw
{
    struct vec3;
    struct vec4;
    struct mat2;
    struct mat3;
    struct mat4;
    struct ivec3;
    struct bvec3;

    struct vec2
    {
        float x;
        float y;

        vec2() : x{}, y{}
        {
        }
        vec2(float i) : x{i}, y{i}
        {
        }
        vec2(float x, float y) : x{x}, y{y}
        {
        }
        explicit vec2(vec3);

        auto xyy() const -> vec3;
        auto xyx() const -> vec3;
        auto xxy() const -> vec3;
        auto yxy() const -> vec3;
        auto yyx() const -> vec3;
        auto yxx() const -> vec3;
        auto yyy() const -> vec3;
        auto xxx() const -> vec3;
        auto xxxx() const -> vec4;
        auto xx() const
        {
            return vec2{x, x};
        }
        auto yx() const
        {
            return vec2{y, x};
        }
        auto yy() const
        {
            return vec2{y, y};
        }

        auto operator-() const
        {
            return vec2{-x, -y};
        }
        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(vec2, x, y)

        auto operator*=(mat2 b) -> vec2 &;

        W_DEFINE_FRIEND_OPERATOR(vec2, +)
        W_DEFINE_FRIEND_OPERATOR(vec2, -)
        W_DEFINE_FRIEND_OPERATOR(vec2, *)
        W_DEFINE_FRIEND_OPERATOR(vec2, /)
    };

    struct ivec2
    {
        int x;
        int y;
        ivec2() : x{}, y{}
        {
        }
        ivec2(int a) : x{a}, y{a}
        {
        }
        ivec2(int x, int y) : x{x}, y{y}
        {
        }
        explicit ivec2(vec2 a) : x{int(a.x)}, y{int(a.y)}
        {
        }
        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(ivec2, x, y)
        auto operator<<(std::size_t a) const
        {
            return ivec2{x << a, y << a};
        }
        auto operator^(ivec2 a) const
        {
            return ivec2{x ^ a.x, y ^ a.y};
        }
        auto operator&(ivec2 a) const
        {
            return ivec2{x & a.x, y & a.y};
        }
        operator vec2() const
        {
            return {float(x), float(y)};
        }

        W_DEFINE_FRIEND_OPERATOR(ivec2, +)
        W_DEFINE_FRIEND_OPERATOR(ivec2, -)
        W_DEFINE_FRIEND_OPERATOR(ivec2, *)
        W_DEFINE_FRIEND_OPERATOR(ivec2, /)
    };

    using uint = std::uint32_t;

    struct uvec2
    {
        uint x;
        uint y;

        uvec2() : x{}, y{}
        {
        }

        uvec2(uint x, uint y) : x{x}, y{y}
        {
        }

        auto operator^(uvec2 right) const
        {
            return uvec2
            {
                x ^ right.x,
                y ^ right.y
            };
        }
        auto operator<<(uint right) const
        {
            return uvec2
            {
                x << right,
                y << right
            };
        }
        auto operator<<(uvec2 right) const
        {
            return uvec2
            {
                x << right.x,
                y << right.y
            };
        }
        auto operator>>(uvec2 right) const
        {
            return uvec2
            {
                x >> right.x,
                y >> right.y
            };
        }
    };

    struct uvec3
    {
        uint x;
        uint y;
        uint z;

        uvec3() : x{}, y{}, z{}
        {
        }

        uvec3(uint x, uint y, uint z) : x{x}, y{y}, z{z}
        {
        }
    };

    struct uvec4
    {
        uint x;
        uint y;
        uint z;
        uint w;

        uvec4() : x{}, y{}, z{}, w{}
        {
        }

        uvec4(uint x, uint y, uint z, uint w) : x{x}, y{y}, z{z}, w{w}
        {
        }

        auto xy() const
        {
            return uvec2{x, y};
        }
        auto xy(uvec2 a)
        {
            x = a.x;
            y = a.y;
        }
        auto xw() const
        {
            return uvec2{x, w};
        }
        auto xw(uvec2 a)
        {
            x = a.x;
            w = a.y;
        }
        auto yx() const
        {
            return uvec2{y, x};
        }
        auto yz() const
        {
            return uvec2{y, z};
        }
        auto yz(uvec2 a)
        {
            y = a.x;
            z = a.y;
        }
        auto zwxy() const
        {
            return uvec4{z, w, x, y};
        }
        auto wxyz() const
        {
            return uvec4{w, x, y, z};
        }
        auto yzwx() const
        {
            return uvec4{y, z, w, x};
        }

        auto operator^(uvec4 right) const
        {
            return uvec4
            {
                x ^ right.x,
                y ^ right.y,
                z ^ right.z,
                w ^ right.w
            };
        }
        auto operator^=(uvec4 right)
        {
            *this = *this ^ right;
        }
        auto operator>>(uvec4 right) const
        {
            return uvec4
            {
                x >> right.x,
                y >> right.y,
                z >> right.z,
                w >> right.w  
            };
        }

        auto operator*(uvec4 right) const
        {
            return uvec4
            {
                x * right.x,
                y * right.y,
                z * right.z,
                w * right.w
            };
        }
        auto operator*=(uvec4 right)
        {
            *this = *this * right;
        }
    };

    struct vec3
    {
        float x;
        float y;
        float z;

        vec3() : x{}, y{}, z{}
        {
        }
        vec3(float i) : x{i}, y{i}, z{i}
        {
        }
        vec3(float x, float y, float z) : x{x}, y{y}, z{z}
        {
        }
        vec3(vec2 a, float b) : x{a.x}, y{a.y}, z{b}
        {
        }
        vec3(float a, vec2 b) : x{a}, y{b.x}, z{b.y}
        {
        }
        vec3(vec2 a, vec3 b) : x{a.x}, y{a.y}, z{b.x}
        {
        }
        vec3(ivec3);
        vec3(bvec3);

        vec3 & operator++()
        {
            ++x;
            ++y;
            ++z;
            return *this;
        }
        
        auto xx() const
        {
            return vec2{x, x};
        }
        auto xy() const
        {
            return vec2{x, y};
        }
        auto xy(vec2 a)
        {
            x = a.x;
            y = a.y;
        }
        auto yx() const
        {
            return vec2{y, x};
        }
        auto yx(vec2 a)
        {
            y = a.x;
            x = a.y;
        }
        auto yz() const
        {
            return vec2{y, z};
        }
        auto yz(vec2 a)
        {
            y = a.x;
            z = a.y;
        }
        auto xz() const
        {
            return vec2{x, z};
        }
        auto xz(vec2 a)
        {
            x = a.x;
            z = a.y;
        }
        auto zy() const
        {
            return vec2{z, y};
        }
        auto zy(vec2 a)
        {
            z = a.x;
            y = a.y;
        }
        auto yy() const
        {
            return vec2{y, y};
        }
        auto xxx() const
        {
            return vec3{x, x, x};
        }
        auto xxy() const
        {
            return vec3{x, x, y};
        }
        auto xyx() const
        {
            return vec3{x, y, x};
        }
        auto xyy() const
        {
            return vec3{x, y, y};
        }
        auto xzy() const
        {
            return vec3{x, z, y};
        }
        auto xzz() const
        {
            return vec3{x, z, z};
        }
        auto xzy(vec3 a)
        {
            x = a.x;
            z = a.y;
            y = a.z;
        }
        auto yxx() const
        {
            return vec3{y, x, x};
        }
        auto yxy() const
        {
            return vec3{y, x, y};
        }
        auto yxz() const
        {
            return vec3{y, x, z};
        }
        auto yyx() const
        {
            return vec3{y, y, x};
        }
        auto yzx() const
        {
            return vec3{y, z, x};
        }
        auto zxy() const
        {
            return vec3{z, x, y};
        }
        auto zyx() const
        {
            return vec3{z, y, x};
        }
        auto zyz() const
        {
            return vec3{z, y, z};
        }
        auto zzz() const
        {
            return vec3{z, z, z};
        }
        auto yyxy() const -> vec4;
        auto zzzz() const -> vec4;

        auto operator-() const
        {
            return vec3{-x, -y, -z};
        }
        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(vec3, x, y, z)

        auto operator*=(mat3 b) -> vec3 &;

        auto operator==(vec3 const & right) const
        {
            return tie() == right.tie();
        }

        W_DEFINE_FRIEND_OPERATOR(vec3, +)
        W_DEFINE_FRIEND_OPERATOR(vec3, -)
        W_DEFINE_FRIEND_OPERATOR(vec3, *)
        W_DEFINE_FRIEND_OPERATOR(vec3, /)
    };

    inline vec2::vec2(vec3 a) : x{a.x}, y{a.y}
    {
    }

    struct ivec3
    {
        int x, y, z;

        ivec3() : x{}, y{}, z{}
        {
        }

        ivec3(int a) : x{a}, y{a}, z{a}
        {
        }

        ivec3(int x, int y, int z) : x{x}, y{y}, z{z}
        {
        }

        explicit ivec3(vec3 a) : x{int(a.x)}, y{int(a.y)}, z{int(a.z)}
        {
        }

        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(ivec3, x, y, z)

        W_DEFINE_FRIEND_OPERATOR(ivec3, +)
        W_DEFINE_FRIEND_OPERATOR(ivec3, -)
        W_DEFINE_FRIEND_OPERATOR(ivec3, *)
        W_DEFINE_FRIEND_OPERATOR(ivec3, /)

        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(ivec3, &=)
        W_DEFINE_FRIEND_OPERATOR(ivec3, &)
        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(ivec3, ^=)
        W_DEFINE_FRIEND_OPERATOR(ivec3, ^)
    };

    inline vec3::vec3(ivec3 a) : x(a.x), y(a.y), z(a.z)
    {
    }

    struct bvec3
    {
        bool x;
        bool y;
        bool z;

        bvec3() : x{}, y{}, z{}
        {
        }

        bvec3(bool x, bool y, bool z) : x{x}, y{y}, z{z}
        {
        }
    };

    inline vec3::vec3(bvec3 a) : x(a.x), y(a.y), z(a.z)
    {
    }

    struct bvec4
    {
        bool x;
        bool y;
        bool z;
        bool w;

        bvec4() : x{}, y{}, z{}, w{}
        {
        }

        bvec4(bool x, bool y, bool z, bool w) : x{x}, y{y}, z{z}, w{w}
        {
        }
    };

    struct vec4
    {
        float x, y, z, w;

        vec4() : x{}, y{}, z{}, w{}
        {
        }
        vec4(float i) : x{i}, y{i}, z{i}, w{i}
        {
        }
        vec4(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w}
        {
        }
        vec4(float a, float b, vec2 c) : x{a}, y{b}, z{c.x}, w{c.y}
        {
        }
        vec4(float a, vec2 b, float c) : x{a}, y{b.x}, z{b.y}, w{c}
        {
        }
        vec4(vec2 a, float z, float w) : x{a.x}, y{a.y}, z{z}, w{w}
        {
        }
        vec4(vec2 a, vec2 b) : x{a.x}, y{a.y}, z{b.x}, w{b.y}
        {
        }
        vec4(vec3 a, float w) : x{a.x}, y{a.y}, z{a.z}, w{w}
        {
        }
        vec4(uvec4 a) : x(a.x), y(a.y), z(a.z), w(a.w)
        {
        }

        auto & operator--()
        {
            x--;
            y--;
            z--;
            w--;
            return *this;
        }
        auto operator--(int)
        {
            auto t = *this;
            --*this;
            return t;
        }
        auto xy() const
        {
            return vec2{x, y};
        }
        auto xy(vec2 a)
        {
            x = a.x;
            y = a.y;
            return vec2{x, y};
        }
        auto xw() const
        {
            return vec2{x, w};
        }
        auto yx() const
        {
            return vec2{y, x};
        }
        auto yx(vec2 a)
        {
            y = a.x;
            x = a.y;
        }
        auto yz() const
        {
            return vec2{y, z};
        }
        auto yw() const
        {
            return vec2{y, w};
        }
        auto xz() const
        {
            return vec2{x, z};
        }
        auto xz(vec2 a)
        {
            x = a.x;
            z = a.y;
        }
        auto ww() const
        {
            return vec2{w, w};
        }
        auto xyz() const
        {
            return vec3{x, y, z};
        }
        auto xyz(vec3 a)
        {
            x = a.x;
            y = a.y;
            z = a.z;
        }
        auto yz(vec2 a)
        {
            y = a.x;
            z = a.y;
        }
        auto zw()
        {
            return vec2{z, w};
        }
        auto grb(vec3 a)
        {
            y = a.x;
            x = a.y;
            z = a.z;
        }
        auto xxx() const
        {
            return vec3{x, x, x};
        }
        auto yyy() const
        {
            return vec3{y, y, y};
        }
        auto yzx() const
        {
            return vec3{y, z, x};
        }
        auto zzz() const
        {
            return vec3{z, z, z};
        }
        auto www() const
        {
            return vec3{w, w, w};
        }
        
        W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(vec4, x, y, z, w)

        auto operator*=(mat4 b) -> vec4 &;

        W_DEFINE_FRIEND_OPERATOR(vec4, +)
        W_DEFINE_FRIEND_OPERATOR(vec4, -)
        W_DEFINE_FRIEND_OPERATOR(vec4, *)
        W_DEFINE_FRIEND_OPERATOR(vec4, /)
    };

    inline auto vec2::xxx() const -> vec3
    {
        return {x, x, x};
    }

    inline auto vec2::xxxx() const -> vec4
    {
        return  {x, x, x, x};
    }

    inline auto vec3::yyxy() const -> vec4
    {
        return {y, y, x, y};
    }

    inline auto vec3::zzzz() const -> vec4
    {
        return {z, z, z, z};
    }

    struct mat2
    {
        float a, b;
        float c, d;
        mat2() : a{}, b{}, c{}, d{}
        {
        }
        explicit mat2(float a) : a{a}, b{a}, c{a}, d{a}
        {
        }
        explicit mat2(float a, float b, float c, float d) : a{a}, b{b}, c{c}, d{d}
        {
        }
        explicit mat2(vec2 a, float b, float c) : a{a.x}, b{a.y}, c{b}, d{c}
        {
        }
        explicit mat2(vec4 a) : a{a.x}, b{a.y}, c{a.z}, d{a.w}
        {
        }

        auto operator*(float m) const
        {
            return mat2{a * m, b * m, c * m, d * m};
        }

        auto operator*(vec2 v) const
        {
            return vec2
            {
                a * v.x + c * v.y,
                b * v.x + d * v.y
            };
        }

        auto operator*(mat2 v) const
        {
            return mat2
            {
                a * v.a + c * v.b, 
                b * v.a + d * v.b,

                a * v.c + c * v.d,
                b * v.c + d * v.d
            };
        }

        auto operator*=(mat2 v) // TODO: vice versa
        {
            *this = *this * v;
        }
    };

    inline auto operator*(vec2 a, mat2 b)
    {
        a *= b;
        return a;
    }

    inline auto vec2::operator*=(mat2 b) -> vec2 &
    {
        *this = vec2
        {
            x * b.a + y * b.b, 
            x * b.c + y * b.d
        };
        return *this;
    }

    struct mat3
    {
        float a,  b,  c; 
        float d,  e,  f;
        float g,  h,  i;

        explicit mat3(float z) : 
        a{z}, b{z}, c{z},
        d{z}, e{z}, f{z},
        g{z}, h{z}, i{z}
        {
        }

        mat3
        (
            float a, float b, float c, 
            float d, float e, float f,
            float g, float h, float i
        ) : 
            a{a}, b{b}, c{c},
            d{d}, e{e}, f{f},
            g{g}, h{h}, i{i}
        {
        }

        mat3
        (
            vec3 a, 
            vec3 b,
            vec3 c
        ) : 
            a{a.x}, b{a.y}, c{a.z},
            d{b.x}, e{b.y}, f{b.z},
            g{c.x}, h{c.y}, i{c.z}
        {
        }

        auto operator-(mat3 const & right) const
        {
            return mat3
            {
                a - right.a, b - right.b, c - right.c,
                d - right.d, e - right.e, f - right.f,
                g - right.g, h - right.h, i - right.i
            };
        }

        auto operator*(vec3 v) const
        {
            return vec3
            {
                a * v.x + d * v.y + g * v.z,
                b * v.x + e * v.y + h * v.z,
                c * v.x + f * v.y + i * v.z
            };
        }

        auto operator*(mat3 v) const
        {
            return mat3
            {
                a * v.a + d * v.b + g * v.c, 
                b * v.a + e * v.b + h * v.c,
                c * v.a + f * v.b + i * v.c,

                a * v.d + d * v.e + g * v.f, 
                b * v.d + e * v.e + h * v.f,
                c * v.d + f * v.e + i * v.f,

                a * v.g + d * v.h + g * v.i, 
                b * v.g + e * v.h + h * v.i,
                c * v.g + f * v.h + i * v.i
            };
        }

        auto operator[](std::size_t z)
        {
            switch (z)
            {
                case 0:
                    return vec3{a, b, c};
                case 1:
                    return vec3{d, e, f};
                case 2:
                    return vec3{g, h, i};
                default:
                    throw std::out_of_range(__FUNCTION__);
            }
        }
    };

    inline auto operator*(vec3 a, mat3 b)
    {
        a *= b;
        return a;
    }

    inline auto vec3::operator*=(mat3 b) -> vec3 &
    {
        *this = vec3
        {
            x * b.a + y * b.b + z * b.c, 
            x * b.d + y * b.e + z * b.f,
            x * b.g + y * b.h + z * b.i
        };
        return *this;
    }

    struct mat4
    {
        float a, b, c, d; 
        float e, f, g, h;
        float i, j, k, l;
        float m, n, o, p;

        explicit mat4(float z) : 
            a{z}, b{z}, c{z}, d{z}, 
            e{z}, f{z}, g{z}, h{z}, 
            i{z}, j{z}, k{z}, l{z},
            m{z}, n{z}, o{z}, p{z}
        {
        }

        mat4
        (
            float a, float b, float c, float d, 
            float e, float f, float g, float h, 
            float i, float j, float k, float l,
            float m, float n, float o, float p
        ) : 
            a{a}, b{b}, c{c}, d{d}, 
            e{e}, f{f}, g{g}, h{h}, 
            i{i}, j{j}, k{k}, l{l},
            m{m}, n{n}, o{o}, p{p}
        {
        }

        auto operator[](std::size_t z)
        {
            switch (z)
            {
                case 0:
                    return vec4{a, b, c, d};
                case 1:
                    return vec4{e, f, g, h};
                case 2:
                    return vec4{i, j, k, l};
                case 3:
                    return vec4{m, n, o, p};
                default:
                    throw std::out_of_range(__FUNCTION__);
            }
        }
    };

    inline auto operator*(vec4 a, mat4 b)
    {
        a *= b;
        return a;
    }

    inline auto vec4::operator*=(mat4 b) -> vec4 &
    {
        *this = vec4
        {
            x * b.a + y * b.b + z * b.c + w * b.d, 
            x * b.e + y * b.f + z * b.g + w * b.h,
            x * b.i + y * b.j + z * b.k + w * b.l,
            x * b.m + y * b.n + z * b.o + w * b.p
        };

        return *this;
    }

    inline auto vec2::xyy() const -> vec3
    {
        return {x, y, y};
    }

    inline auto vec2::xyx() const -> vec3
    {
        return  {x, y, x};
    }

    inline auto vec2::xxy() const -> vec3
    {
        return  {x, x, y};
    }

    inline auto vec2::yxy() const -> vec3
    {
        return {y, x, y};
    }

    inline auto vec2::yyx() const -> vec3
    {
        return  {y, y, x};
    }

    inline auto vec2::yxx() const -> vec3
    {
        return  {y, x, x};
    }

    inline auto vec2::yyy() const -> vec3
    {
        return  {y, y, y};
    }

    struct rgb
    {
        float r;
        float g;
        float b;

        operator vec3() const
        {
            return vec3{r, g, b};
        }

        auto rrr() const
        {
            return vec3{r, r, r};
        }
    };

    struct sampler2D
    {
        std::size_t width;
        std::size_t height;
        std::vector<std::uint32_t> data;

        sampler2D() = default;
        sampler2D(sampler2D const &) = delete;
        auto operator=(sampler2D const &) = delete;
    };

    inline auto iTime = 0.0f;
    inline auto iDate = vec4{};
    inline auto iFrame = 0;
    inline auto iMouse = vec3{};
    inline auto iResolution = vec3{100, 100, 1};
    inline auto iChannel0 = sampler2D{};
    inline auto iChannel1 = sampler2D{};
    inline auto iChannel2 = sampler2D{};
    inline auto iChannel3 = sampler2D{};
    // inline auto iChannelResolution = std::array<vec2, 4>{};

    inline auto length(vec2 a)
    {
        return std::hypot(a.x, a.y);
    }
    inline auto length(vec3 a)
    {
        return std::hypot(a.x, a.y, a.z);
    }

    inline auto distance(vec2 a, vec2 b)
    {
        return length(a - b);
    }
    inline auto distance(vec3 a, vec3 b)
    {
        return length(a - b);
    }
    inline auto sqrt(float a)
    {
        return a < 0 ? NAN : std::sqrt(a);
    }
    inline auto sqrt(vec2 a)
    {
        return vec2{sqrt(a.x), sqrt(a.y)};
    }
    inline auto sqrt(vec3 a)
    {
        return vec3{sqrt(a.x), sqrt(a.y), sqrt(a.z)};
    }
    inline auto sqrt(vec4 a)
    {
        return vec4{sqrt(a.x), sqrt(a.y), sqrt(a.z), sqrt(a.w)};
    }
    inline auto normalize(vec2 a)
    {
        return a / length(a);
    }
    inline auto normalize(vec3 a)
    {
        return a / length(a);
    }
    inline auto dot(vec2 a, vec2 b)
    {
        return a.x * b.x + a.y * b.y;
    }
    inline auto dot(vec3 a, vec3 b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    inline auto radians(float a)
    {
        return a / 180 * std::numbers::pi_v<float>;
    }
    inline auto radians(vec3 a)
    {
        return vec3{radians(a.x), radians(a.y), radians(a.z)};
    }
    inline auto sin(float a)
    {
        return std::sin(a);
    }
    inline auto sin(vec2 a)
    {
        return vec2(sin(a.x), sin(a.y));
    }
    inline auto sin(vec3 a)
    {
        return vec3(sin(a.x), sin(a.y), sin(a.z));
    }
    inline auto sin(vec4 a)
    {
        return vec4(sin(a.x), sin(a.y), sin(a.z), sin(a.w));
    }
    inline auto cos(float a)
    {
        return std::cos(a);
    }
    inline auto cos(vec2 a)
    {
        return vec2{cos(a.x), cos(a.y)};
    }
    inline auto cos(vec3 a)
    {
        return vec3{cos(a.x), cos(a.y), cos(a.z)};
    }
    inline auto cos(vec4 a)
    {
        return vec4{cos(a.x), cos(a.y), cos(a.z), cos(a.w)};
    }
    inline auto tanh(float a)
    {
        return std::tanh(a);
    }
    inline auto tanh(vec3 a)
    {
        return vec3{tanh(a.x), tanh(a.y), tanh(a.z)};
    }
    inline auto atan(float a, float b)
    {
        return std::atan2(a, b);
    }
    inline auto atan(float a)
    {
        return std::atan(a);
    }
    inline auto min(float a, float b)
    {
        return std::min(a, b);
    }
    inline auto min(double a, double b) // TODO: remove
    {
        return std::min(float(a), float(b));
    }
    inline auto min(float a, double b) // TODO: remove
    {
        return std::min(a, float(b));
    }
    inline auto min(double a, float b) // TODO: remove
    {
        return std::min(float(a), b);
    }
    inline auto min(int a, int b)
    {
        return std::min(a, b);
    }
    inline auto min(vec2 a, vec2 b)
    {
        return vec2(min(a.x, b.x), min(a.y, b.y));
    }
    inline auto min(vec3 a, vec3 b)
    {
        return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
    }
    inline auto min(ivec3 a, ivec3 b)
    {
        return ivec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
    }
    inline auto min(vec3 a, float b)
    {
        return vec3{min(a.x, b), min(a.y, b), min(a.z, b)};
    }
    inline auto max(float a, float b)
    {
        return std::max(a, b);
    }
    inline auto max(double a, double b) // TODO: remove
    {
        return std::max(float(a), float(b));
    }
    inline auto max(double a, float b) // TODO: remove
    {
        return std::max(float(a), b);
    }
    inline auto max(float a, double b) // TODO: remove
    {
        return std::max(a, float(b));
    }
    inline auto max(int a, int b)
    {
        return std::max(a, b);
    }
    inline auto max(vec2 a, float b)
    {
        return vec2{max(a.x, b), max(a.y, b)};
    }
    inline auto max(vec3 a, float b)
    {
        return vec3{max(a.x, b), max(a.y, b), max(a.z, b)};
    }
    inline auto max(vec4 a, float b)
    {
        return vec4{max(a.x, b), max(a.y, b), max(a.z, b), max(a.w, b)};
    }
    inline auto max(float a, vec3 b)
    {
        return vec3{max(a, b.x), max(a, b.y), max(a, b.z)};
    }
    inline auto max(vec2 a, vec2 b)
    {
        return vec2{max(a.x, b.x), max(a.y, b.y)};
    }
    inline auto max(vec3 a, vec3 b)
    {
        return vec3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
    }
    inline auto max(ivec3 a, ivec3 b)
    {
        return ivec3{max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)};
    }
    inline auto abs(float a)
    {
        return std::abs(a);
    }
    inline auto abs(int a)
    {
        return std::abs(a);
    }
    inline auto abs(vec2 a)
    {
        return vec2{abs(a.x), abs(a.y)};
    }
    inline auto abs(vec3 a)
    {
        return vec3{abs(a.x), abs(a.y), abs(a.z)};
    }
    inline auto abs(ivec3 a)
    {
        return ivec3{abs(a.x), abs(a.y), abs(a.z)};
    }
    inline auto abs(vec4 a)
    {
        return vec4{abs(a.x), abs(a.y), abs(a.z), abs(a.w)};
    }
    inline auto log(float a)
    {
        return std::log(a);
    }
    inline auto log(vec3 a)
    {
        return vec3{log(a.x), log(a.y), log(a.z)};
    }
    inline auto exp(float a)
    {
        return std::exp(a);
    }
    inline auto exp(vec3 a)
    {
        return vec3{exp(a.x), exp(a.y), exp(a.z)};
    }
    inline auto ceil(float a)
    {
        return std::ceil(a);
    }
    inline auto ceil(vec2 a)
    {
        return vec2{ceil(a.x), ceil(a.y)};
    }
    inline auto ceil(vec3 a)
    {
        return vec3{ceil(a.x), ceil(a.y), ceil(a.z)};
    }
    inline auto clamp(float a, float b, float c)
    {
        return std::clamp(a, b, c);
    }
    inline auto clamp(vec2 a, float b, float c)
    {
        return vec2(clamp(a.x, b, c), clamp(a.y, b, c));
    }
    inline auto clamp(vec3 a, float b, float c)
    {
        return vec3(clamp(a.x, b, c), clamp(a.y, b, c), clamp(a.z, b, c));
    }
    inline auto clamp(vec2 a, vec2 b, vec2 c)
    {
        return vec2(clamp(a.x, b.x, c.x), clamp(a.y, b.y, c.y));
    }
    inline auto clamp(vec3 a, vec3 b, vec3 c)
    {
        return vec3(clamp(a.x, b.x, c.x), clamp(a.y, b.y, c.y), clamp(a.z, b.z, c.z));
    }
    inline auto clamp(vec4 a, vec4 b, vec4 c)
    {
        return vec4(clamp(a.x, b.x, c.x), clamp(a.y, b.y, c.y), clamp(a.z, b.z, c.z), clamp(a.w, b.w, c.w));
    }
    inline auto step(float a, float b)
    {
        return b < a ? 0.f : 1.f;
    }
    inline auto step(float a, vec2 b)
    {
        return vec2{step(a, b.x), step(a, b.y)};
    }
    inline auto step(float a, vec3 b)
    {
        return vec3{step(a, b.x), step(a, b.y), step(a, b.z)};
    }
    inline auto step(float a, vec4 b)
    {
        return vec4{step(a, b.x), step(a, b.y), step(a, b.z), step(a, b.w)};
    }
    inline auto step(vec3 a, vec3 b)
    {
        return vec3{step(a.x, b.x), step(a.y, b.y), step(a.z, b.z)};
    }
    inline auto step(vec4 a, vec4 b)
    {
        return vec4{step(a.x, b.x), step(a.y, b.y), step(a.z, b.z), step(a.w, b.w)};
    }
    inline auto mix(float edge0, float edge1, float x)
    {
        return edge0 * (1 - x) + edge1 * x;
    }
    inline auto mix(vec2 edge0, vec2 edge1, float x)
    {
        return edge0 * (1 - x) + edge1 * x;
    }
    inline auto mix(vec3 edge0, vec3 edge1, float x)
    {
        return edge0 * (1 - x) + edge1 * x;
    }
    inline auto mix(vec4 edge0, vec4 edge1, float x)
    {
        return edge0 * (1 - x) + edge1 * x;
    }
    inline auto mix(vec3 edge0, vec3 edge1, vec3 x)
    {
        return vec3{mix(edge0.x, edge1.x, x.x), mix(edge0.y, edge1.y, x.y), mix(edge0.z, edge1.z, x.z)};
    }
    inline auto unmix(float edge0, float edge1, float x)
    {
        auto const p = edge1 - edge0;
        return p ? (x - edge0) / p : x < edge0 ? 1.f : 0.f;
    }
    inline auto smoothstep(float edge0, float edge1, float x)
    {
        auto const r = clamp(unmix(edge0, edge1, x), 0.f, 1.f);
        return r * r * (3 - 2 * r);
    }
    inline auto smoothstep(float edge0, float edge1, vec2 a)
    {
        return vec2{smoothstep(edge0, edge1, a.x), smoothstep(edge0, edge1, a.y)};
    }
    inline auto smoothstep(float edge0, float edge1, vec3 a)
    {
        return vec3{smoothstep(edge0, edge1, a.x), smoothstep(edge0, edge1, a.y), smoothstep(edge0, edge1, a.z)};
    }
    inline auto cross(vec3 a, vec3 b)
    {
        return vec3
        {
            a.y * b.z - b.y * a.z,
            a.z * b.x - b.z * a.x,
            a.x * b.y - b.x * a.y
        };
    }
    inline auto floor(float a)
    {
        return std::floor(a);
    }
    inline auto floor(vec2 a)
    {
        return vec2{floor(a.x), floor(a.y)};
    }
    inline auto floor(vec3 a)
    {
        return vec3{floor(a.x), floor(a.y), floor(a.z)};
    }
    inline auto floor(vec4 a)
    {
        return vec4{floor(a.x), floor(a.y), floor(a.z), floor(a.w)};
    }
    inline auto fract(float a)
    {
        return a - floor(a);
    }
    inline auto fract(vec2 a)
    {
        return a - floor(a);
    }
    inline auto fract(vec3 a)
    {
        return a - floor(a);
    }
    inline auto fract(vec4 a)
    {
        return a - floor(a);
    }
    inline auto round(float a)
    {
        return std::round(a);
    }
    inline auto round(vec2 a)
    {
        return vec2{round(a.x), round(a.y)};
    }
    inline auto mod(float a, float b)
    {
        return a - b * floor(a / b);
    }
    inline auto mod(vec2 a, float b)
    {
        return vec2{mod(a.x, b), mod(a.y, b)};
    }
    inline auto mod(vec3 a, float b)
    {
        return vec3{mod(a.x, b), mod(a.y, b), mod(a.z, b)};
    }
    inline auto mod(vec4 a, float b)
    {
        return vec4{mod(a.x, b), mod(a.y, b), mod(a.z, b), mod(a.w, b)};
    }
    inline auto mod(vec2 a, vec2 b)
    {
        return vec2{mod(a.x, b.x), mod(a.y, b.y)};
    }
    inline auto mod(vec3 a, vec3 b)
    {
        return vec3{mod(a.x, b.x), mod(a.y, b.y), mod(a.z, b.z)};
    }
    inline auto sign(float a)
    {
        return a < 0 ? -1.f : a > 0 ? 1.f : 0.f;
    }
    inline auto sign(int a)
    {
        return a < 0 ? -1 : a > 0 ? 1 : 0;
    }
    inline auto sign(vec2 a)
    {
        return vec2{sign(a.x), sign(a.y)};
    }
    inline auto sign(vec3 a)
    {
        return vec3{sign(a.x), sign(a.y), sign(a.z)};
    }
    inline auto sign(ivec3 a)
    {
        return ivec3{sign(a.x), sign(a.y), sign(a.z)};
    }
    inline auto refract(vec3 a, vec3 b, float c)
    {
        auto const k = 1.0f - c * c * (1.0f - dot(b, a) * dot(b, a));
        if (k < 0.0f)
            return vec3(0.0); // TODO: or genDType(0.0)
        else
            return c * a - (c * dot(b, a) + sqrt(k)) * b;
    }
    inline auto reflect(vec3 a, vec3 b)
    {
        return a - 2.0f * dot(b, a) * b;
    }
    inline vec3 faceforward(vec3 a, vec3 b, vec3 c)
    {
        return dot(c, b) < 0 ? a : -a;
    }
    inline auto pow(float a, float b)
    {
        return std::pow(a, b);
    }
    inline auto pow(vec3 a, vec3 b)
    {
        return vec3{pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z)};
    }
    inline auto pow(vec4 a, vec4 b)
    {
        return vec4{pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z), pow(a.w, b.w)};
    }
    inline auto floatBitsToUint(float a)
    {
        return std::bit_cast<uint>(a);
    }
    inline auto floatBitsToUint(vec4 a)
    {
        return uvec4
        {
            floatBitsToUint(a.x),
            floatBitsToUint(a.y),
            floatBitsToUint(a.z),
            floatBitsToUint(a.w)
        };
    }
    inline auto all(uvec4 a)
    {
        return a.x && a.y && a.z && a.w;
    }
    inline auto equal(int a, int b)
    {
        return a == b;
    }
    inline auto equal(uint a, uint b)
    {
        return a == b;
    }
    inline auto equal(ivec3 a, ivec3 b)
    {
        return bvec3
        {
            equal(a.x, b.x),
            equal(a.y, b.y),
            equal(a.z, b.z)
        };
    }
    inline auto equal(uvec4 a, uvec4 b)
    {
        return bvec4
        {
            equal(a.x, b.x),
            equal(a.y, b.y),
            equal(a.z, b.z),
            equal(a.w, b.w)
        };
    }
    inline auto notEqual(int a, int b)
    {
        return a != b;
    }
    inline auto notEqual(ivec3 a, ivec3 b)
    {
        return bvec3
        {
            notEqual(a.x, b.x),
            notEqual(a.y, b.y),
            notEqual(a.z, b.z),
        };
    }
    
    inline vec4 textureLod(sampler2D const & b, vec2 p, float l)
    {
        if (!std::isnormal(p.x) || !std::isnormal(p.y))
        {
            return vec4{};
        }

        auto const width = b.width;
        auto const height = b.height;

        auto u = p.x;
        auto v = p.y;

        u = fract(u);
        v = fract(v);

        float x = u * (width - 1);
        float y = v * (height - 1);

        int x0 = static_cast<int>(std::floor(x));
        int x1 = std::min(x0 + 1, int(width) - 1);
        int y0 = static_cast<int>(std::floor(y));
        int y1 = std::min(y0 + 1, int(height) - 1);

        float tx = x - x0;
        float ty = y - y0;

        auto get_texel = [&](int xi, int yi)
        {
            yi = height - yi - 1;
            //return vec3((xi + yi) % 2 == 1);

            if (xi < 0 || xi >= width || yi < 0 || yi >= height)
            {
                // std::println("over{}-{}", xi, yi);
                // DebugBreak(); 
                // abort();
                return vec3{};
            }

            auto const i = b.data[xi + yi * width];
            auto const r = (i >> 16) & 0xFF;
            auto const g = (i >> 8) & 0xFF;
            auto const b = (i >> 0) & 0xFF;
            return vec3
            {
                float(r) / 255.0f, 
                float(g) / 255.0f, 
                float(b) / 255.0f
            };
        };

        auto const c00 = get_texel(x0, y0);
        auto const c10 = get_texel(x1, y0);
        auto const c01 = get_texel(x0, y1);
        auto const c11 = get_texel(x1, y1);

        auto const top = mix(c00, c10, tx);
        auto const bottom = mix(c01, c11, tx);

        auto const result = mix(top, bottom, ty);

        return vec4(result, 0.f);
    }
    inline auto texture(sampler2D const & b, vec2 p, float bias = 0.f)
    {
        auto const result = textureLod(b, p, 0.f);
        return rgb{result.x, result.y, result.z};
    }

    inline auto texture(sampler2D const & b, vec3 p, float bias = 0.f) // TODO
    {
        auto const result = textureLod(b, p.xy(), 0.f);
        return rgb{result.x, result.y, result.z};
    }

    struct POINT
    {
        long x;
        long y;
    };
    struct SIZE
    {
        long cx;
        long cy;
    };
    auto render(float time, POINT p, SIZE s, auto f, auto & o, POINT mouse, bool parallel = true)
    {
        static_assert(std::is_same_v<decltype(f({float{}, float{}})), w::glsw::vec4>);

        w::glsw::iTime = time;

        auto seconds_since_midnight = []
        {
            // Thanks, Howard
            using namespace std;
            using namespace std::chrono;

            auto now = current_zone()->to_local(system_clock::now());
            auto today = floor<days>(now);
            auto tod = duration_cast<nanoseconds>(now - today);
            return tod.count() / 1e9f;
        };

        w::glsw::iDate = {1970, 1, 1, seconds_since_midnight()};
        w::glsw::iResolution = w::glsw::vec3{float(s.cx), float(s.cy), 1.0};
        {
            auto const rel = POINT{mouse.x - p.x, mouse.y - p.y};
            w::glsw::iMouse = {float(rel.x), float(rel.y), 0.f};
        }

        auto const write = [&](auto i, auto j)
        {
            auto c = f({j + 0.5f, i + 0.5f});

            /* if (std::isnan(c.x) || std::isnan(c.y) || std::isnan(c.z) || std::isnan(c.w))
            {
                c = {1.f};
                std::cout << std::stacktrace::current() << "x = " << j << " y = " << i << "\n";
                std::abort();
            } */

            auto get = [](auto v)
            {
                auto const c = std::clamp(v, 0.f, 1.f);
                auto const m = c * 255.f;
                auto const r = std::uint32_t(std::round(m));
                return r;
            };
            auto const rgba = get(c.z) + (get(c.y) << 8) + (get(c.x) << 16) + (get(c.w) << 24);
            o[i + p.y, j + p.x] = rgba;
        };

        if (parallel)
        {
            auto const v = std::views::iota(long{}, s.cy * s.cx);
            std::for_each
            (
                std::execution::par_unseq,
                v.begin(), v.end(),
                [&](auto v)
                {
                    auto const i = v / s.cx;
                    auto const j = v % s.cx;

                    write(i, j);
                }
            );
        }
        else
        {
            for (auto i = 0; i != s.cy; ++i)
            {
                for (auto j = 0; j != s.cx; ++j)
                {
                    write(i, j);
                }
            }
        }
    }
}
