#pragma once

#include <w/single_arg.hpp>

#include <utility>
#include <concepts>

namespace w
{
    template<typename P, typename ... A>
    concept has_clone = requires(A && ... a) { P::clone(std::forward<A>(a)...); };

    template <typename P>
    class ptr 
    {
    public:
        using pointer_type = typename P::pointer_type;

        inline static struct clone_t {} clone;
        inline static struct borrow_t {} borrow;

        ptr() noexcept : value(P::null()) {}

        template <typename... A>
        requires has_clone<P, A...>
        explicit ptr(clone_t, A && ... a)
            : value(P::clone(std::forward<A>(a)...))
        {
        }

        explicit ptr(borrow_t, pointer_type p) noexcept
            : value(p)
        {
        }

        ~ptr()
        {
            if (P::is_null(value))
            {

            }
            else
            {
                P::destroy(value);
            }
        }

        ptr(ptr && other) noexcept
            : value(other.release())
        {
        }

        
        ptr(ptr const & other)
            requires has_clone<P, pointer_type>
            : value(P::clone(other.value))
        {
        }

        auto operator=(ptr const & right)
            requires has_clone<P, pointer_type>
        {
            ptr(right).swap(*this);
        }

        auto operator=(ptr && right) noexcept
        {
            ptr(std::move(right)).swap(*this);
        }

        void swap(ptr & other) noexcept
        {
            using std::swap;
            swap(value, other.value);
        }

        void reset(auto && ... a)
        {
            ptr(std::forward<decltype(a)>(a)...).swap(*this);
        }

        auto release()
        {
            return std::exchange(value, P::null());
        }

        pointer_type get() const noexcept
        {
            return value;
        }

    private:
        pointer_type value;
    };

    struct no_clone {};
    template<auto D, auto C = no_clone{}>
    struct ptr_default_policy
    {
        using pointer_type = single_arg<decltype(D)>;
        inline static constexpr auto has_clone = not std::is_same_v<decltype(C), no_clone>;
        static auto null()
        {
            return pointer_type{};
        }
        static auto clone(pointer_type a) requires has_clone
        {
            return C(a);
        }
        static auto is_null(pointer_type a)
        {
            return a == null();
        }
        static auto destroy(pointer_type a)
        {
            D(a);
        }
    };

}