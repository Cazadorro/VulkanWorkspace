//
// Created by Bolt on 5/6/2023.
//

#ifndef VULKANWORKSPACE_TYPENAME_H
#define VULKANWORKSPACE_TYPENAME_H
#include <string_view>
namespace uul {
    template<typename T>
    constexpr auto type_name() {
        std::string_view name, prefix, suffix;
#ifdef __clang__
        name = __PRETTY_FUNCTION__;
      prefix = "auto type_name() [T = ";
      suffix = "]";
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
      prefix = "constexpr auto type_name() [with T = ";
      suffix = "]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "auto __cdecl type_name<";
        suffix = ">(void)";
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
    template<typename T>
    constexpr auto type_name(T t){
        return type_name<T>();
    }

}
#endif //VULKANWORKSPACE_TYPENAME_H
