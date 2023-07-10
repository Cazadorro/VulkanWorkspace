//
// Created by Bolt on 7/9/2023.
//

#ifndef VULKANWORKSPACE_CONTAINER_H
#define VULKANWORKSPACE_CONTAINER_H
namespace uul{
    template<typename Container_T, typename Container_U>
    void extend(Container_T& lhs, const Container_U& rhs){
        lhs.reserve(lhs.size() + rhs.size());
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename Container_T, typename Container_U, typename Container_V = Container_T>
    Container_T concat(const Container_T& lhs, const Container_U& rhs){
        Container_V container;
        container.reserve(lhs.size() + rhs.size());
        container.insert(container.end(), lhs.begin(), lhs.end());
        container.insert(container.end(), rhs.begin(), rhs.end());
        return container;
    }

    template<typename Container_T, typename Container_U>
    void append( Container_T& lhs, const Container_U& rhs){
        lhs.reserve(lhs.size() + rhs.size());
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    }
    //Returns the start of the new container?
    template<typename Container_T, typename Container_U>
    auto append_offset( Container_T& lhs, const Container_U& rhs){
        auto old_size = lhs.size();
        append(lhs, rhs);
        return lhs.begin() + old_size;
    }
}
#endif //VULKANWORKSPACE_CONTAINER_H
