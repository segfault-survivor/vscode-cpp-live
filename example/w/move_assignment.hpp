#pragma once

#define W_DEFINE_MOVE_ASSIGNMENT(T) \
    auto operator=(T && right) noexcept \
    { \
        T{std::move(right)}.swap(*this); \
    } \
    /**/
