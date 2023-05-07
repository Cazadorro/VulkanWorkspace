//
// Created by Bolt on 5/6/2023.
//

#ifndef VULKANWORKSPACE_CONCEPTS_H
#define VULKANWORKSPACE_CONCEPTS_H
namespace uul{
    template <typename T>
    concept ContiguousContainer = requires (T t) {
        t.data();
        t.size();
    };
    template<typename T>
    concept NestedContiguousContainer = ContiguousContainer<T> && ContiguousContainer<typename T::value_type>;
}
#endif //VULKANWORKSPACE_CONCEPTS_H
