//
// Created by Shae Bolt on 1/13/2022.
//

#ifndef VULKANWORKSPACE_UNREACHABLE_H
#define VULKANWORKSPACE_UNREACHABLE_H
//https://stackoverflow.com/a/65258501/
#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible with GCC (-std=c++0x or above)
[[noreturn]] inline __attribute__((always_inline)) void unreachable() {__builtin_unreachable();}
#elif defined(_MSC_VER) // MSVC
[[noreturn]] __forceinline void unreachable() {__assume(false);}
#else // ???
inline void unreachable() {}
#endif
#endif //VULKANWORKSPACE_UNREACHABLE_H
