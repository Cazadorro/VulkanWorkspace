//
// Created by Shae Bolt on 9/6/2021.
//

#include "bitmask.h"

vul::bitmask::bitmask(std::size_t size_bits) : m_data(size_bits % m_bit_count == 0 ? size_bits/m_bit_count : (size_bits/m_bit_count) + 1, 0){

}
//bool get(const in uint32_array bitmask, i32vec2 size, i32vec2 index){
//    const int32_t bit_count = 32u;
//    int32_t linear_index = index.y * size.x + index.x;
//    int32_t u32_index = linear_index / bit_count;
//    int32_t bit_index = linear_index % bit_count;
//    return bitmask.data[u32_index] & (1 << bit_index) != 0;
//}
//
//void set(inout uint32_array bitmask, i32vec2 size, i32vec2 index){
//    const int32_t bit_count = 32u;
//    int32_t linear_index = index.y * size.x + index.x;
//    int32_t u32_index = linear_index / bit_count;
//    int32_t bit_index = linear_index % bit_count;
//    bitmask.data[u32_index] |= (1 << bit_index);
//}
//
//void clear(inout uint32_array bitmask, i32vec2 size, i32vec2 index){
//    const int32_t bit_count = 32u;
//    int32_t linear_index = index.y * size.x + index.x;
//    int32_t u32_index = linear_index / bit_count;
//    int32_t bit_index = linear_index % bit_count;
//    bitmask.data[u32_index] &= ~(1 << bit_index);
//}
//
//
//bool vul::bitmask::get(std::uint32_t x, std::uint32_t y) {
//    int32_t linear_index = index.y * size.x + index.x;
//    int32_t u32_index = linear_index / bit_count;
//    int32_t bit_index = linear_index % bit_count;
//    return bitmask.data[u32_index] & (1 << bit_index) != 0;
//}

bool vul::bitmask::get(glm::uvec3 pos, glm::uvec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    return get(linear_index);
}

bool vul::bitmask::get(glm::ivec3 pos, glm::ivec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    return get(linear_index);
}

void vul::bitmask::set(glm::uvec3 pos, glm::uvec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    set(linear_index);
}

void vul::bitmask::set(glm::ivec3 pos, glm::ivec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    set(linear_index);
}

void vul::bitmask::clear(glm::uvec3 pos, glm::uvec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    clear(linear_index);
}

void vul::bitmask::clear(glm::ivec3 pos, glm::ivec3 size) {
    auto linear_index = pos.z * size.x*size.y + pos.y * size.x + pos.x;
    clear(linear_index);
}

bool vul::bitmask::get(glm::uvec2 pos, glm::uvec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    return get(linear_index);
}

bool vul::bitmask::get(glm::ivec2 pos, glm::ivec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    return get(linear_index);
}

void vul::bitmask::set(glm::uvec2 pos, glm::uvec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    set(linear_index);
}

void vul::bitmask::set(glm::ivec2 pos, glm::ivec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    set(linear_index);
}

void vul::bitmask::clear(glm::uvec2 pos, glm::uvec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    clear(linear_index);
}

void vul::bitmask::clear(glm::ivec2 pos, glm::ivec2 size) {
    auto linear_index = pos.y * size.x + pos.x;
    clear(linear_index);
}

const std::uint32_t *vul::bitmask::data() const {
    return m_data.data();
}

std::size_t vul::bitmask::size() const {
    return m_data.size();
}

bool vul::bitmask::get(std::uint32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    return (m_data[u32_index] & (1 << bit_index)) != 0;
}

bool vul::bitmask::get(std::int32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    return (m_data[u32_index] & (1 << bit_index)) != 0;
}

void vul::bitmask::set(std::uint32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    m_data[u32_index] |= (1 << bit_index);
}

void vul::bitmask::set(std::int32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    m_data[u32_index] |= (1 << bit_index);
}


void vul::bitmask::clear(std::uint32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    m_data[u32_index] &= ~(1 << bit_index);
}

void vul::bitmask::clear(std::int32_t pos) {
    auto linear_index = pos;
    auto u32_index = linear_index / m_bit_count;
    auto bit_index = linear_index % m_bit_count;
    m_data[u32_index] &= ~(1 << bit_index);
}


