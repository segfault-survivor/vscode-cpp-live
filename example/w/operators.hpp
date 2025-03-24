#pragma once

#include <w/tie.hpp>
#include <tuple>
#include <utility>

namespace w::operators
{
    #define W_DEFINE_FRIEND_OPERATOR(T, op1) \
        friend auto operator op1(T a, T b)   \
        {                                    \
            a op1 ## = b;                    \
            return a;                        \
        }                                    \
        /**/

    namespace tuple
    {
        #define W_TUPLE_OP(o) \
            template<typename ... T> \
            auto & operator o(std::tuple<T...> & l, std::tuple<T...> const & r) \
            { \
                return []<std::size_t ... I>(auto & l, auto & r, std::index_sequence<I...>) -> auto & \
                { \
                    return ((std::get<I>(l) o std::get<I>(r)),...), l; \
                }(l, r, std::make_index_sequence<sizeof...(T)>{}); \
            } \
            /**/

        W_TUPLE_OP(+=);
        W_TUPLE_OP(-=);
        W_TUPLE_OP(*=);
        W_TUPLE_OP(/=);

        W_TUPLE_OP(&=);
        W_TUPLE_OP(^=);

        #undef W_TUPLE_OP

        #define W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(t, o) \
            auto operator o(t a) \
            { \
                using namespace w::operators::tuple; \
                auto self = tie(); \
                self o a.tie(); \
            } \
            /**/
        #define W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS(t, ...) \
            W_DEFINE_TIE(__VA_ARGS__)                            \
            W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(t, +=)  \
            W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(t, -=)  \
            W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(t, *=)  \
            W_DEFINE_ARITHMETIC_ASSIGNMENT_OPERATORS_ONE(t, /=)  \
            /**/
    }
}
