//
// Created by Shae Bolt on 8/18/2021.
//

#ifndef VULKANWORKSPACE_VKASSERT_H
#define VULKANWORKSPACE_VKASSERT_H
namespace vul{
    namespace detail{
        void handle_assert(const char* message,
                           const char* condition,
                           const char * file_name,
                           const char * function_name,
                           unsigned long long line_number);
        inline void nothing(){}
    }

#if  defined(_MSC_VER) || defined(WIN32)
#define VUL_DEBUG_BREAK __debugbreak
#elif __has_builtin(__builtin_debugtrap)
    #define VUL_DEBUG_BREAK __builtin_debugtrap
#elif defined(SIGTRAP)
#define VUL_DEBUG_BREAK raise(SIGTRAP) detail::nothing
#else
#define VUL_DEBUG_BREAK detail::nothing
#endif



#define VUL_ASSERT_WITH_MESSAGE(condition, message) \
    do{                                                \
        if(!(condition)){                              \
            vul::detail::handle_assert(             \
                message,                               \
                #condition,                            \
                __FILE__,                              \
                __FUNCTION__,                          \
                __LINE__);                             \
            VUL_DEBUG_BREAK();                      \
            std::terminate();                          \
        }                                              \
    }while(0)

#define VUL_ASSERT_NO_MESSAGE(condition) \
    do{                                     \
        if(!(condition)){                   \
            vul::detail::handle_assert(  \
                "message",                    \
                #condition,                 \
                __FILE__,                   \
                __FUNCTION__,               \
                __LINE__);                  \
            VUL_DEBUG_BREAK();           \
            std::terminate();               \
        }                                   \
    }while(0)

#define VUL_WARNING_WITH_MESSAGE(condition, message) \
    do{                                                \
        if(!(condition)){                              \
            vul::detail::handle_assert(             \
                message,                               \
                #condition,                            \
                __FILE__,                              \
                __FUNCTION__,                          \
                __LINE__);                             \
        }                                              \
    }while(0)

#define VUL_WARNING_NO_MESSAGE(condition) \
    do{                                     \
        if(!(condition)){                   \
            vul::detail::handle_assert(  \
                "message",                    \
                #condition,                 \
                __FILE__,                   \
                __FUNCTION__,               \
                __LINE__);                  \
        }                                   \
    }while(0)



#define VUL_VOID_ARG1(x0) \
    do { (void)sizeof(x0); } while(0)

#define VUL_VOID_ARG2(x0, x1) \
    do { (void)sizeof(x0), (void)sizeof(x1); } while(0)

#define VUL_ARG_CHOOSER_2(arg0, arg1, arg2, ...) arg2
#define VUL_CHOOSE_ASSERT_MACRO(...) \
    VUL_ARG_CHOOSER_2(__VA_ARGS__,   \
        VUL_ASSERT_WITH_MESSAGE,     \
        VUL_ASSERT_NO_MESSAGE, )

#define VUL_CHOOSE_WARNING_MACRO(...) \
    VUL_ARG_CHOOSER_2(__VA_ARGS__,   \
        VUL_WARNING_WITH_MESSAGE,     \
        VUL_WARNING_NO_MESSAGE, )

#define VUL_CHOOSE_VOID_MACRO(...) \
    VUL_ARG_CHOOSER_2(__VA_ARGS__, \
        VUL_VOID_ARG2,             \
        VUL_VOID_ARG1, )
#define VUL_ASSERT(...)  VUL_CHOOSE_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__)
#define VUL_WARNING(...)  VUL_CHOOSE_WARNING_MACRO(__VA_ARGS__)(__VA_ARGS__)

#ifdef VUL_DEBUG
#define VUL_DEBUG_ASSERT(...) VUL_ASSERT(__VA_ARGS__)
#else
#define VUL_DEBUG_ASSERT(...) VUL_CHOOSE_VOID_MACRO(__VA_ARGS__)(__VA_ARGS__)
#endif

#ifdef VUL_DEBUG
#define VUL_DEBUG_WARNING(...) VUL_WARNING(__VA_ARGS__)
#else
#define VUL_DEBUG_WARNING(...) VUL_CHOOSE_VOID_MACRO(__VA_ARGS__)(__VA_ARGS__)
#endif

}
#endif //VULKANWORKSPACE_VKASSERT_H
