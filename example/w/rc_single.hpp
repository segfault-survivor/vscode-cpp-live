#pragma once

#include <memory>
#include <mutex>

namespace w
{
    template<typename T>
    struct rc_single
    {
        inline static std::mutex m;
        inline static std::weak_ptr<T> w;

        std::shared_ptr<T> value;

        static auto create()
        {
            return (void)std::lock_guard{m}, (w.expired() ? (w = std::make_shared<T>()).lock() : w.lock());
        }

        rc_single() : value{create()}
        {
        }
        rc_single(rc_single const & other) = default;
        rc_single(rc_single && other) noexcept = default;
        rc_single & operator=(rc_single const & right) = default;
        rc_single & operator=(rc_single && right) noexcept = default;
        ~rc_single()
        {
            (void)std::lock_guard{m}, value.reset();
        }
    };
}
