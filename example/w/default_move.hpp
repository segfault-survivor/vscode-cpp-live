#pragma once

#include <w/swap.hpp>
#include <w/move_assignment.hpp>

#include <utility>

namespace w
{
    template<typename T>
    struct default_move
    {
        using value_type = T;
        value_type value;

        default_move() : value{} {}
        default_move(value_type value) : value{value} {}
        default_move(default_move const & other) : value{other.value} {}
        default_move(default_move && other) noexcept : value(std::exchange(other.value, T{})) {}
        auto operator=(default_move const & right) { default_move{right}.swap(*this); }
        W_DEFINE_MOVE_ASSIGNMENT(default_move)
        W_DEFINE_SWAP(default_move, value)
        operator value_type() const { return value; }
    };
}
