#ifndef STACKUTILS_GLSL
#define STACKUTILS_GLSL


#define GEN_FIXED_STACK(TYPE, MAX_STACK_SIZE)                        \
    struct Stack_##TYPE##_##MAX_STACK_SIZE{                          \
        uint size;                                                   \
        uint array[MAX_STACK_SIZE];                                  \
    };                                                               \
                                                                     \
    TYPE top(const in Stack_##TYPE##_##MAX_STACK_SIZE stack){        \
        return stack.array[stack.size - 1];                          \
    }                                                                \
    void push(inout Stack_##TYPE##_##MAX_STACK_SIZE stack,           \
              TYPE value){                                           \
        stack.array[stack.size] = value;                             \
        stack.size += 1;                                             \
    }                                                                \
                                                                     \
    TYPE pop(inout Stack_##TYPE##_##MAX_STACK_SIZE stack){           \
        stack.size -= 1;                                             \
        return stack.array[stack.size];                              \
    }                                                                \
                                                                     \
    bool is_empty(const in Stack_##TYPE##_##MAX_STACK_SIZE stack){   \
        return stack.size == 0;                                      \
    }                                                                \
                                                                     \
    bool is_full(const in Stack_##TYPE##_##MAX_STACK_SIZE stack){    \
        return stack.size >= MAX_STACK_SIZE;                         \
    }                                                                \
                                                                     \
    Stack_##TYPE##_##MAX_STACK_SIZE init_Stack_##TYPE##_##MAX_STACK_SIZE(){ \
        Stack_##TYPE##_##MAX_STACK_SIZE stack;                              \
        stack.size = 0;                                                     \
        return stack;                                                       \
    }



#endif //STACKUTILS_GLSL
