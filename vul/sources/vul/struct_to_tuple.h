//
// Created by Shae Bolt on 8/23/2021.
//

#ifndef VULKANWORKSPACE_STRUCT_TO_TUPLE_H
#define VULKANWORKSPACE_STRUCT_TO_TUPLE_H
#include "vul/repeat_macro.h"
#include <glm/glm.hpp>
#include <tuple>
#define STRUCT_TO_TUPLE_MACRO_FUNC(HUNDREDS,TENS,ONES) \
    else if constexpr(is_braces_constructible<type, VUL_COMMA_REP_PLUS(HUNDREDS,TENS,ONES,any_type)>{}) { \
      auto&& [VUL_COMMA_NUM_FILLER_REP_PLUS(HUNDREDS,TENS,ONES,p)] = object; \
      return std::make_tuple(VUL_COMMA_NUM_FILLER_REP_PLUS(HUNDREDS,TENS,ONES,p)); \
    }
namespace vul{

    template <class T, class... TArgs> decltype(void(T{std::declval<TArgs>()...}), std::true_type{}) test_is_braces_constructible(int);
    template <class, class...> std::false_type test_is_braces_constructible(...);
    template <class T, class... TArgs> using is_braces_constructible = decltype(test_is_braces_constructible<T, TArgs...>(0));

    //source https://www.reddit.com/r/cpp/comments/4yp7fv/c17_structured_bindings_convert_struct_to_a_tuple/
    struct any_type {
        template<class T>
        constexpr operator T(); // non explicit
    };

    //downside of approach is manual creation of each state, luckily effectively only have to care about 64 attributes
    //per binding, maxVertexInputAttributes is really 64 outside some of the really really big cards.
    template<class T>
    auto struct_to_tuple(T&& object) noexcept {
        using type = std::decay_t<T>;
        if constexpr(false){

        }
        VUL_REPEAT_WITH_NUM(0,6,4,STRUCT_TO_TUPLE_MACRO_FUNC)
        else {
            return std::make_tuple();
        }
    }

//    int to_tuple_test() {
//        {
//            struct s {
//                int p1;
//                double p2;
//            };
//
//            auto t = to_tuple(s{1, 2.0});
//            static_assert(std::is_same<std::tuple<int, double>, decltype(t)>{});
//            assert(1 == std::get<0>(t));
//            assert(2.0 == std::get<1>(t));
//        }
//
//        {
//            struct s {
//                struct nested { } p1;
//                int p2;
//                int p3;
//                s* p4;
//            };
//
//            auto t = to_tuple(s{s::nested{}, 42, 87, nullptr});
//            static_assert(std::is_same<std::tuple<s::nested, int, int, s*>, decltype(t)>{});
//            assert(42 == std::get<1>(t));
//            assert(87 == std::get<2>(t));
//            assert(nullptr == std::get<3>(t));
//        }
//    }
}
#endif //VULKANWORKSPACE_STRUCT_TO_TUPLE_H
