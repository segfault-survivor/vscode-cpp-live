#pragma once

#define W_DEFINE_TIE(...)                              \
    auto tie() { return std::tie(__VA_ARGS__); }       \
    auto tie() const { return std::tie(__VA_ARGS__); } \
    /**/
