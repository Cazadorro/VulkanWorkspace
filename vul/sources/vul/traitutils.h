//
// Created by Shae Bolt on 9/29/2021.
//

#ifndef VULKANWORKSPACE_TRAITUTILS_H
#define VULKANWORKSPACE_TRAITUTILS_H
#include <type_traits>


namespace vul{
    template <class, template <class, class...> class>
    struct is_instance : public std::false_type {};

    template <class...Ts, template <class, class...> class U>
    struct is_instance<U<Ts...>, U> : public std::true_type {};

    template<typename ...>
    using to_void = void; // maps everything to void, used in non-evaluated contexts

    template<typename T, typename = void>
    struct is_container : std::false_type
    {};

    template<typename T>
    struct is_container<T,
            to_void<decltype(std::declval<T>().begin()),
                    decltype(std::declval<T>().end()),
                    typename T::value_type
            >> : std::true_type // will  be enabled for iterable objects
    {};

    template<typename T, typename = void>
    struct is_contiguous : std::false_type
    {};
    template<typename T>
    struct is_contiguous<T,
            to_void<decltype(std::declval<T>().data()),
                    decltype(std::declval<T>().size()),
                    typename T::value_type
            >> : std::true_type // will  be enabled for iterable objects
    {};

}
#endif //VULKANWORKSPACE_TRAITUTILS_H
