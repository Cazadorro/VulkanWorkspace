#ifndef REFTYPES_GLSL
#define REFTYPES_GLSL


#define GEN_GET_SET_REF_T(TYPE, ALIGNMENT)                                       \
        layout(buffer_reference,                                               \
               buffer_reference_align=ALIGNMENT,                               \
               scalar) buffer TYPE##_ref{                                      \
            TYPE##_t data;                                                     \
        };                                                                     \
        TYPE##_t get(TYPE##_ref ref, const in uint8_t i){                      \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint8_t i, const in TYPE##_t value){ \
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int8_t i){                       \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int8_t i, const in TYPE##_t value){  \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint16_t i){                     \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint16_t i, const in TYPE##_t value){\
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int16_t i){                      \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int16_t i, const in TYPE##_t value){ \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint32_t i){                     \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint32_t i, const in TYPE##_t value){\
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int32_t i){                      \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int32_t i, const in TYPE##_t value){ \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint64_t i){                     \
            return ref[(i)].data;                                              \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint64_t i, const in TYPE##_t value){\
            ref[(i)].data = value;                                             \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int64_t i){                      \
            return ref[(i)].data;                                              \
        }                                                                      \
        void set(TYPE##_ref ref, const in int64_t i, const in TYPE##_t value){ \
            ref[(i)].data = value;                                             \
        }


#define GEN_GET_SET_REF(TYPE, ALIGNMENT)                                       \
        layout(buffer_reference,                                               \
               buffer_reference_align=ALIGNMENT,                               \
               scalar) buffer TYPE##_ref{                                      \
            TYPE data;                                                     \
        };                                                                     \
        TYPE##_t get(TYPE##_ref ref, const in uint8_t i){                      \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint8_t i, const in TYPE value){ \
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int8_t i){                       \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int8_t i, const in TYPE value){  \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint16_t i){                     \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint16_t i, const in TYPE value){\
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int16_t i){                      \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int16_t i, const in TYPE value){ \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint32_t i){                     \
            return ref[uint(i)].data;                                          \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint32_t i, const in TYPE value){\
            ref[uint(i)].data = value;                                         \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int32_t i){                      \
            return ref[int(i)].data;                                           \
        }                                                                      \
        void set(TYPE##_ref ref, const in int32_t i, const in TYPE value){ \
            ref[int(i)].data = value;                                          \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in uint64_t i){                     \
            return ref[(i)].data;                                              \
        }                                                                      \
        void set(TYPE##_ref ref, const in uint64_t i, const in TYPE value){\
            ref[(i)].data = value;                                             \
        }                                                                      \
        TYPE##_t get(TYPE##_ref ref, const in int64_t i){                      \
            return ref[(i)].data;                                              \
        }                                                                      \
        void set(TYPE##_ref ref, const in int64_t i, const in TYPE value){ \
            ref[(i)].data = value;                                             \
        }


GEN_GET_SET_REF_T(float32, 4)
//GEN_GET_SET_REF(f32vec2, 8)
//GEN_GET_SET_REF(f32vec3, 4)
//GEN_GET_SET_REF(f32vec4, 16)
//GEN_GET_SET_REF_T(float64, 8)
//GEN_GET_SET_REF(f64vec2, 16)
//GEN_GET_SET_REF(f64vec3, 8)
//GEN_GET_SET_REF(f64vec4, 32)
//GEN_GET_SET_REF_T(uint8, 1)
//GEN_GET_SET_REF(u8vec2, 2)
//GEN_GET_SET_REF(u8vec3, 1)
//GEN_GET_SET_REF(u8vec4, 4)
//GEN_GET_SET_REF_T(uint16, 2)
//GEN_GET_SET_REF(u16vec2, 4)
//GEN_GET_SET_REF(u16vec3, 2)
//GEN_GET_SET_REF(u16vec4, 8)
//GEN_GET_SET_REF_T(uint32, 4)
//GEN_GET_SET_REF(u32vec2, 8)
//GEN_GET_SET_REF(u32vec3, 4)
//GEN_GET_SET_REF(u32vec4, 16)
//GEN_GET_SET_REF_T(uint64, 8)
//GEN_GET_SET_REF(u64vec2, 16)
//GEN_GET_SET_REF(u64vec3, 8)
//GEN_GET_SET_REF(i64vec4, 32)
//GEN_GET_SET_REF_T(int8, 1)
//GEN_GET_SET_REF(i8vec2, 2)
//GEN_GET_SET_REF(i8vec3, 1)
//GEN_GET_SET_REF(i8vec4, 4)
//GEN_GET_SET_REF_T(int16, 2)
//GEN_GET_SET_REF(i16vec2, 4)
//GEN_GET_SET_REF(i16vec3, 2)
//GEN_GET_SET_REF(i16vec4, 8)
//GEN_GET_SET_REF_T(int32, 4)
//GEN_GET_SET_REF(i32vec2, 8)
//GEN_GET_SET_REF(i32vec3, 4)
//GEN_GET_SET_REF(i32vec4, 16)
//GEN_GET_SET_REF_T(int64, 8)
//GEN_GET_SET_REF(i64vec2, 16)
//GEN_GET_SET_REF(i64vec3, 8)
//GEN_GET_SET_REF(i64vec4, 32)



#undef GEN_GET_SET_REF

#endif //REFTYPES_GLSL
