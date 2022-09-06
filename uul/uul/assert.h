//
// Created by Shae Bolt on 8/18/2021.
//

#ifndef VULKANWORKSPACE_VKASSERT_H
#define VULKANWORKSPACE_VKASSERT_H
namespace uul{
    namespace detail{
        void handle_assert(const char* message,
                           const char* condition,
                           const char * file_name,
                           const char * function_name,
                           unsigned long long line_number);
        inline void nothing(){}
    }

#if  defined(_MSC_VER) || defined(WIN32)
#define UUL_DEBUG_BREAK __debugbreak
#elif __has_builtin(__builtin_debugtrap)
    #define UUL_DEBUG_BREAK __builtin_debugtrap
#elif defined(SIGTRAP)
#define UUL_DEBUG_BREAK raise(SIGTRAP) detail::nothing
#else
#define UUL_DEBUG_BREAK detail::nothing
#endif



#define UUL_ASSERT_WITH_MESSAGE(condition, message) \
    do{                                                \
        if(!(condition)){                              \
            uul::detail::handle_assert(             \
                message,                               \
                #condition,                            \
                __FILE__,                              \
                __FUNCTION__,                          \
                __LINE__);                             \
            UUL_DEBUG_BREAK();                      \
            std::terminate();                          \
        }                                              \
    }while(0)

#define UUL_ASSERT_NO_MESSAGE(condition) \
    do{                                     \
        if(!(condition)){                   \
            uul::detail::handle_assert(  \
                "message",                    \
                #condition,                 \
                __FILE__,                   \
                __FUNCTION__,               \
                __LINE__);                  \
            UUL_DEBUG_BREAK();           \
            std::terminate();               \
        }                                   \
    }while(0)

#define UUL_WARNING_WITH_MESSAGE(condition, message) \
    do{                                                \
        if(!(condition)){                              \
            uul::detail::handle_assert(             \
                message,                               \
                #condition,                            \
                __FILE__,                              \
                __FUNCTION__,                          \
                __LINE__);                             \
        }                                              \
    }while(0)

#define UUL_WARNING_NO_MESSAGE(condition) \
    do{                                     \
        if(!(condition)){                   \
            uul::detail::handle_assert(  \
                "message",                    \
                #condition,                 \
                __FILE__,                   \
                __FUNCTION__,               \
                __LINE__);                  \
        }                                   \
    }while(0)



#define UUL_VOID_ARG1(x0) \
    do { (void)sizeof(x0); } while(0)

#define UUL_VOID_ARG2(x0, x1) \
    do { (void)sizeof(x0), (void)sizeof(x1); } while(0)

//  idea from  https://stackoverflow.com/a/28074198/
#define UUL_ARG_CHOOSER_2(arg0, arg1, arg2, ...) arg2
//needs extra set of parenthesis when used.
#define UUL_MSVC_FUNC_RECOMPOSER_WORK_AROUND_2(args_with_parenthesis) UUL_ARG_CHOOSER_2 args_with_parenthesis
#define UUL_CHOOSE_ASSERT_MACRO(...) \
    UUL_MSVC_FUNC_RECOMPOSER_WORK_AROUND_2((__VA_ARGS__,   \
        UUL_ASSERT_WITH_MESSAGE,     \
        UUL_ASSERT_NO_MESSAGE, ))

#define UUL_CHOOSE_WARNING_MACRO(...) \
    UUL_MSVC_FUNC_RECOMPOSER_WORK_AROUND_2((__VA_ARGS__,   \
        UUL_WARNING_WITH_MESSAGE,     \
        UUL_WARNING_NO_MESSAGE, ))

#define UUL_CHOOSE_VOID_MACRO(...) \
    UUL_MSVC_FUNC_RECOMPOSER_WORK_AROUND_2((__VA_ARGS__, \
        UUL_VOID_ARG2,             \
        UUL_VOID_ARG1, ))
#define UUL_ASSERT(...)  UUL_CHOOSE_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__)
#define UUL_WARNING(...)  UUL_CHOOSE_WARNING_MACRO(__VA_ARGS__)(__VA_ARGS__)

#ifdef UUL_DEBUG
#define UUL_DEBUG_ASSERT(...) UUL_ASSERT(__VA_ARGS__)
#else
#define UUL_DEBUG_ASSERT(...) UUL_CHOOSE_VOID_MACRO(__VA_ARGS__)(__VA_ARGS__)
#endif

#ifdef UUL_DEBUG
#define UUL_DEBUG_WARNING(...) UUL_WARNING(__VA_ARGS__)
#else
#define UUL_DEBUG_WARNING(...) UUL_CHOOSE_VOID_MACRO(__VA_ARGS__)(__VA_ARGS__)
#endif

}
#endif //VULKANWORKSPACE_VKASSERT_H
