#pragma once

#include <w/tie.hpp>
#include <print>
#include <typeinfo>

#define W_DEFINE_SWAP(C, ...)                              \
    W_DEFINE_TIE(__VA_ARGS__)                              \
    auto swap(C & other) noexcept -> void                  \
    {                                                      \
        tie().swap(other.tie());                           \
    }                                                      \
    friend auto swap(C & left, C & right) noexcept -> void \
    {                                                      \
        return left.swap(right);                           \
    }                                                      \
    /**/
