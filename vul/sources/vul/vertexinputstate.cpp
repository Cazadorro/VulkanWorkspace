//
// Created by Shae Bolt on 8/22/2021.
//

#include "vertexinputstate.h"
#include "vul/enums.h"
template<>
vul::Format vul::getAttributeFormat<std::uint8_t>() {
    return Format::R8Uint;
}

template<>
vul::Format vul::getAttributeFormat<std::int8_t>() {
    return Format::R8Sint;
}

template<>
vul::Format vul::getAttributeFormat<std::uint16_t>() {
    return Format::R16Uint;
}

template<>
vul::Format vul::getAttributeFormat<std::int16_t>() {
    return Format::R16Sint;
}

template<>
vul::Format vul::getAttributeFormat<std::uint32_t>() {
    return Format::R32Uint;
}

template<>
vul::Format vul::getAttributeFormat<std::int32_t>() {
    return Format::R32Sint;
}

template<>
vul::Format vul::getAttributeFormat<std::uint64_t>() {
    return Format::R64Uint;
}

template<>
vul::Format vul::getAttributeFormat<std::int64_t>() {
    return Format::R64Sint;
}

template<>
vul::Format vul::getAttributeFormat<float>() {
    return Format::R32Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<double>() {
    return Format::R64Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::u8vec2>() {
    return Format::R8g8Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i8vec2>() {
    return Format::R8g8Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u16vec2>() {
    return Format::R16g16Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i16vec2>() {
    return Format::R16g16Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u32vec2>() {
    return Format::R32g32Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i32vec2>() {
    return Format::R32g32Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u64vec2>() {
    return Format::R64g64Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i64vec2>() {
    return Format::R64g64Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::f32vec2>() {
    return Format::R32g32Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::f64vec2>() {
    return Format::R64g64Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::u8vec3>() {
    return Format::R8g8b8Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i8vec3>() {
    return Format::R8g8b8Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u16vec3>() {
    return Format::R16g16b16Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i16vec3>() {
    return Format::R16g16b16Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u32vec3>() {
    return Format::R32g32b32Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i32vec3>() {
    return Format::R32g32b32Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u64vec3>() {
    return Format::R64g64b64Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i64vec3>() {
    return Format::R64g64b64Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::f32vec3>() {
    return Format::R32g32b32Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::f64vec3>() {
    return Format::R64g64b64Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::u8vec4>() {
    return Format::R8g8b8a8Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i8vec4>() {
    return Format::R8g8b8a8Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u16vec4>() {
    return Format::R16g16b16a16Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i16vec4>() {
    return Format::R16g16b16a16Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u32vec4>() {
    return Format::R32g32b32a32Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i32vec4>() {
    return Format::R32g32b32a32Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::u64vec4>() {
    return Format::R64g64b64a64Uint;
}

template<>
vul::Format vul::getAttributeFormat<glm::i64vec4>() {
    return Format::R64g64b64a64Sint;
}

template<>
vul::Format vul::getAttributeFormat<glm::f32vec4>() {
    return Format::R32g32b32a32Sfloat;
}

template<>
vul::Format vul::getAttributeFormat<glm::f64vec4>() {
    return Format::R64g64b64a64Sfloat;
}

