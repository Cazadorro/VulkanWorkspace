//
// Created by Shae Bolt on 8/29/2021.
//

#ifndef VULKANWORKSPACE_VKUTILS_H
#define VULKANWORKSPACE_VKUTILS_H

#include <gsl/span>
#include <vector>

namespace vul {


    template<typename T, typename = void>
    struct is_vk_getable : std::false_type {
    };
    template<typename T>
    struct is_vk_getable<T, std::void_t<decltype(std::declval<T>().get())>>
            : std::true_type {
    };

    template<typename T, is_vk_getable<T> * = nullptr>
    auto get(const T &value) {
        return value.get();
    }

    template<typename T, is_vk_getable<T> * = nullptr>
    auto get_vector(const gsl::span<const std::reference_wrapper<T>> &values) {
        using get_type = decltype(std::declval<T>().get());
        std::vector<get_type> get_vector;
        get_vector.reserve(values.size());
        for (const auto &value : values) {
            get_vector.push_back(get(value.get()));
        }
        return get_vector;
    }

    template<typename T, is_vk_getable<T> * = nullptr>
    auto get_vector(const gsl::span<const T> &values) {
        using get_type = decltype(std::declval<T>().get());
        std::vector<get_type> get_vector;
        get_vector.reserve(values.size());
        for (const auto &value : values) {
            get_vector.push_back(get(value));
        }
        return get_vector;
    }




    template<typename T, is_vk_getable<T> * = nullptr>
    auto get_vector(const std::initializer_list<const T> &values) {
        using get_type = decltype(std::declval<T>().get());
        std::vector<get_type> get_vector;
        get_vector.reserve(values.size());
        for (const auto &value : values) {
            get_vector.push_back(get(value));
        }
        return get_vector;
    }

}
#endif //VULKANWORKSPACE_VKUTILS_H
