//
// Created by Bolt on 5/31/2023.
//

#ifndef VULKANWORKSPACE_CONSTEXPRMAP_H
#define VULKANWORKSPACE_CONSTEXPRMAP_H
#include <stdexcept>
#include <optional>
#include <algorithm>
#include <utility>
#include <cstdint>
namespace uul{
    template<typename Key, typename Value, std::size_t Size>
    struct ConstexprMap {
        static constexpr auto max_size = Size;
        std::array<std::pair<Key, Value>, Size> data;

        [[nodiscard]]
        constexpr Value at(const Key &key) const {
            const auto itr = std::find_if(begin(data), end(data),
                                          [&key](const auto &v) {
                                              return v.first == key;
                                          });
            if (itr != end(data)) {
                return itr->second;
            } else {
                throw std::range_error("Not Found");
            }
        }

        [[nodiscard]]
        constexpr std::optional<Value> get(const Key &key) const {
            const auto itr = std::find_if(begin(data), end(data),
                                          [&key](const auto &v) {
                                              return v.first == key;
                                          });
            if (itr != end(data)) {
                return itr->second;
            } else {
                return std::nullopt;
            }
        }

    };
}
#endif //VULKANWORKSPACE_CONSTEXPRMAP_H
