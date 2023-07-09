//
// Created by Bolt on 7/8/2023.
//

#ifndef VULKANWORKSPACE_FUNCTIONAL_H
#define VULKANWORKSPACE_FUNCTIONAL_H
#include <functional>
namespace uul{
    //use std::bind to compose!
    //https://stackoverflow.com/a/50035197/
//    template<class F>
//    F compose(F f, F g)
//    {
//        return [=](int x) { return f(g(x)); };
//    }

    template<class Root, class... Branches> auto link_functions(Root &&root, Branches &&... branches) {
        return [root, branches...](auto &&...args) {
            if(root != nullptr){
                root(std::forward<decltype(args)>(args)...);
            }
            ((void) branches(std::forward<decltype(args)>(args)...),...);
        };
    }
}
#endif //VULKANWORKSPACE_FUNCTIONAL_H
