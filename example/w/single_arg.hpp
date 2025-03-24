#pragma once

namespace w
{
    template <typename> struct single_arg_t;
    template <typename R, typename A> struct single_arg_t<R (*)(A)> { using type = A; };
    template<typename F> using single_arg = single_arg_t<decltype(+F{})>::type;
}
