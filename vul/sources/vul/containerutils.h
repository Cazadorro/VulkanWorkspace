//
// Created by Shae Bolt on 8/22/2021.
//

#ifndef VULKANWORKSPACE_CONTAINERUTILS_H
#define VULKANWORKSPACE_CONTAINERUTILS_H
#include <type_traits>
#include <algorithm>
#include <functional>
namespace vul{
    template <typename T, typename = void>
    struct is_iterable : std::false_type {};
    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),
            decltype(std::declval<T>().end())>>
            : std::true_type {};

    template <typename T, typename = void>
    struct is_value_member_findable : std::false_type {};
    template <typename T>
    struct is_value_member_findable<T, std::void_t<decltype(std::declval<T>().find(std::declval<typename T::value_type>()))>>
            : std::true_type {};

    template <typename T, typename = void>
    struct is_key_member_findable : std::false_type {};
    template <typename T>
    struct is_key_member_findable<T, std::void_t<decltype(std::declval<T>().find(std::declval<typename T::key_type>()))>>
            : std::true_type {};

    template <typename T>
    using is_member_findable = std::enable_if_t<is_value_member_findable<T>::value || is_key_member_findable<T>::value>;


    template <typename T, typename = void>
    struct is_value_func_findable : std::false_type {};
    template <typename T>
    struct is_value_func_findable<T, std::void_t<decltype(std::find(std::declval<T>().begin(), std::declval<T>().end(), std::declval<typename T::value_type>()))>>
            : std::true_type {};

    template <typename T, typename = void>
    struct is_key_func_findable : std::false_type {};
    template <typename T>
    struct is_key_func_findable<T, std::void_t<decltype(std::find(std::declval<T>().begin(), std::declval<T>().end(), std::declval<typename T::key_type>()))>>
            : std::true_type {};

    template <typename T>
    using is_func_findable = std::enable_if_t<is_value_func_findable<T>::value || is_key_func_findable<T>::value>;

    template<typename Container_T>
    bool contains(const Container_T& container, const typename Container_T::key_type & item){
        if constexpr(is_key_member_findable<Container_T>::value){
            return container.find(item) != container.end();
        }else{
            return std::find(container.begin(), container.end(), item) != container.end();
        }
    }
    template<typename Container_T>
    bool contains(const Container_T& container, const typename Container_T::value_type & item){
        if constexpr(is_value_member_findable<Container_T>::value){
            return container.find(item) != container.end();
        }else{
            return std::find(container.begin(), container.end(), item) != container.end();
        }
    }

    template<typename Container_T>
    bool contains_any(const Container_T& container, const Container_T& rhs) {
        if constexpr(is_key_member_findable<Container_T>::value ||
                     is_value_member_findable<Container_T>::value) {
            for (const auto& item : rhs) {
                if (container.find(item) != container.end()) {
                    return true;
                }
            }
        } else {
            for (const auto& item : rhs) {
                if (std::find(container.begin(), container.end(), item) !=
                    container.end()) {
                    return true;
                }
            }
        }
        return false;
    }

    template<typename Container_T, class UnaryPredicate>
    bool contains_if(const Container_T& container, UnaryPredicate p){
        if constexpr(is_key_member_findable<Container_T>::value ||
                     is_value_member_findable<Container_T>::value) {
            return container.find_if(p) != container.end();
        } else {
            return std::find_if(container.begin(), container.end(), p) != container.end();
        }
    }

    template<typename Container_T, class BinaryPredicate>
    bool contains_any_if(const Container_T& container, const Container_T& rhs, BinaryPredicate p) {
        if constexpr(is_key_member_findable<Container_T>::value ||
                     is_value_member_findable<Container_T>::value) {
            for (const auto& item : rhs) {
                auto unary_predicate = [&item, &p](auto a){
                    return p(item, a);
                };
                if (container.find_if(item, unary_predicate) != container.end()) {
                    return true;
                }
            }
        } else {
            for (const auto& item : rhs) {
                auto unary_predicate = [&item, &p](auto a){
                    return p(item, a);
                };
                if (std::find_if(container.begin(), container.end(), unary_predicate) !=
                    container.end()) {
                    return true;
                }
            }
        }
        return false;
    }

    template<typename Container_T>
    void extend(Container_T& lhs, const Container_T& rhs){
        lhs.reserve(lhs.size() + rhs.size());
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename Container_T>
    void concat(const Container_T& lhs, const Container_T& rhs){
        Container_T container;
        container.reserve(lhs.size() + rhs.size());
        container.insert(container.end(), lhs.begin(), lhs.end());
        container.insert(container.end(), rhs.begin(), rhs.end());
    }

}
#endif //VULKANWORKSPACE_CONTAINERUTILS_H
