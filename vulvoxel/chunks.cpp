//
// Created by Shae Bolt on 9/4/2022.
//

#include "chunks.h"
#include <uul/assert.h>
#include <uul/unreachable.h>
#include <fmt/core.h>
#include <range/v3/view/enumerate.hpp>
#include <stdexcept>


vul::ChunkSpan::ChunkSpan(
        std::span<std::uint32_t, chunk_consts::chunk_size> ref_span)
        : m_ref_span(ref_span) {

}

std::uint32_t &
vul::ChunkSpan::operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) {
    UUL_DEBUG_ASSERT(x < chunk_consts::chunk_width,
                     fmt::format("x {}, >= {}", x,
                                 chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(y < chunk_consts::chunk_width,
                     fmt::format("y {}, >= {}", y,
                                 chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(z < chunk_consts::chunk_width,
                     fmt::format("z {}, >= {}", z,
                                 chunk_consts::chunk_width).c_str());
    return m_ref_span[chunk_consts::to_linear_idx(x, y, z)];
}

std::uint32_t &vul::ChunkSpan::operator[](std::uint16_t idx) {
    UUL_DEBUG_ASSERT(idx < chunk_consts::chunk_size,
                     fmt::format("idx {}, >= {}", idx,
                                 chunk_consts::chunk_size).c_str());
    return m_ref_span[idx];
}

std::uint32_t &vul::ChunkSpan::at(std::uint8_t x, std::uint8_t y,
                                  std::uint8_t z) noexcept(false) {
    if (x >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("x {}, >= {}", x, chunk_consts::chunk_width));
    }
    if (y >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("y {}, >= {}", y, chunk_consts::chunk_width));
    }
    if (z >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("z {}, >= {}", z, chunk_consts::chunk_width));
    }
    return operator()(x, y, z);
}

std::uint32_t &vul::ChunkSpan::at(std::uint16_t idx) noexcept(false) {
    UUL_DEBUG_ASSERT(idx < chunk_consts::chunk_size,
                     fmt::format("idx {}, >= {}", idx,
                                 chunk_consts::chunk_size).c_str());
    return m_ref_span[idx];
}


std::uint32_t vul::ChunkSpan::operator()(std::uint8_t x, std::uint8_t y,
                                         std::uint8_t z) const {
    UUL_DEBUG_ASSERT(x < chunk_consts::chunk_width,
               fmt::format("x {}, >= {}", x,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(y < chunk_consts::chunk_width,
               fmt::format("y {}, >= {}", y,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(z < chunk_consts::chunk_width,
               fmt::format("z {}, >= {}", z,
                           chunk_consts::chunk_width).c_str());
    return m_ref_span[chunk_consts::to_linear_idx(x, y, z)];
}

std::uint32_t vul::ChunkSpan::operator[](std::uint16_t idx) const {
    UUL_DEBUG_ASSERT(idx < chunk_consts::chunk_size,
               fmt::format("idx {}, >= {}", idx,
                           chunk_consts::chunk_size).c_str());
    return m_ref_span[idx];
}

std::uint32_t vul::ChunkSpan::at(std::uint8_t x, std::uint8_t y,
                                 std::uint8_t z) const noexcept(false) {
    if (x >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("x {}, >= {}", x, chunk_consts::chunk_width));
    }
    if (y >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("y {}, >= {}", y, chunk_consts::chunk_width));
    }
    if (z >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("z {}, >= {}", z, chunk_consts::chunk_width));
    }
    return operator()(x, y, z);
}

std::uint32_t vul::ChunkSpan::at(std::uint16_t idx) const noexcept(false) {
    if (idx >= chunk_consts::chunk_size) {
        throw std::out_of_range(
                fmt::format("idx {}, >= {}", idx, chunk_consts::chunk_size));
    }
    return operator[](idx);
}

std::span<const std::uint32_t, vul::chunk_consts::chunk_size>
vul::ChunkSpan::data() const noexcept {
    return m_ref_span;
}

std::span<std::uint32_t, vul::chunk_consts::chunk_size>
vul::ChunkSpan::data() noexcept {
    return m_ref_span;
}

bool
vul::ChunkSpan::is_surrounded(std::uint8_t x, std::uint8_t y, std::uint8_t z,
                              std::uint32_t empty_id) const {
    if (x == 1 || x == chunk_consts::chunk_far_edge) {
        return false;
    }
    if (y == 1 || y == chunk_consts::chunk_far_edge) {
        return false;
    }
    if (z == 1 || z == chunk_consts::chunk_far_edge) {
        return false;
    }
    return
            operator()(x + 1, y, z) == empty_id && //left
            operator()(x - 1, y, z) == empty_id && //right
            operator()(x, y + 1, z) == empty_id && //up
            operator()(x, y - 1, z) == empty_id && //down
            operator()(x, y, z + 1) == empty_id && //front
            operator()(x, y, z - 1) == empty_id;   //back
}

bool
vul::ChunkSpan::is_surrounded(std::uint16_t idx, std::uint32_t empty_id) const {
    auto [x, y, z] = chunk_consts::to_3d_idx(idx);
    return is_surrounded(x, y, z, empty_id);
}

std::span<std::uint32_t, vul::chunk_consts::chunk_size>::iterator
vul::ChunkSpan::begin() {
    return m_ref_span.begin();
}

std::span<std::uint32_t, vul::chunk_consts::chunk_size>::iterator
vul::ChunkSpan::end() {
    return m_ref_span.end();
}

std::span<std::uint32_t, vul::chunk_consts::chunk_size>::iterator
vul::ChunkSpan::begin() const {
    return m_ref_span.begin();
}

std::span<std::uint32_t, vul::chunk_consts::chunk_size>::iterator
vul::ChunkSpan::end() const {
    return m_ref_span.end();
}


vul::ChunkRLE::ChunkRLE(std::vector<std::uint32_t> material_ids,
                        std::vector<std::uint16_t> offsets) : m_material_ids(
        std::move(material_ids)), m_offsets(std::move(offsets)) {

}

vul::ChunkRLE
vul::ChunkRLE::from_linear(
        const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk) {
    std::vector<std::uint16_t> offsets;
    std::vector<std::uint32_t> materials;
    std::uint16_t running_offset = 0;
    std::uint32_t running_material = chunk[0];
    for (const auto &current_material_type: chunk) {
        if (running_material != current_material_type) {
            offsets.push_back(running_offset);
            materials.push_back(running_material);
            running_material = current_material_type;
        }
        running_offset += 1;
    }
    UUL_DEBUG_ASSERT(materials.back() != running_material,
               "Last value shouldn't have actually been set...");
    UUL_DEBUG_ASSERT(running_offset == chunk.size());
    offsets.push_back(running_offset);
    materials.push_back(running_material);
    return {materials, offsets};
}

std::uint32_t
vul::ChunkRLE::operator()(std::uint8_t x, std::uint8_t y,
                          std::uint8_t z) const {
    UUL_DEBUG_ASSERT(x < chunk_consts::chunk_width,
               fmt::format("x {}, >= {}", x,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(y < chunk_consts::chunk_width,
               fmt::format("y {}, >= {}", y,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(z < chunk_consts::chunk_width,
               fmt::format("z {}, >= {}", z,
                           chunk_consts::chunk_width).c_str());
    return binary_search_rle_segment(chunk_consts::to_linear_idx(x, y, z));
}

std::uint32_t vul::ChunkRLE::operator[](std::uint16_t idx) const {
    UUL_DEBUG_ASSERT(idx < chunk_consts::chunk_size,
               fmt::format("idx {}, >= {}", idx,
                           chunk_consts::chunk_size).c_str());
    return binary_search_rle_segment(idx);
}

std::uint32_t
vul::ChunkRLE::at(std::uint8_t x, std::uint8_t y,
                  std::uint8_t z) const noexcept(false) {
    if (x >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("x {}, >= {}", x, chunk_consts::chunk_width));
    }
    if (y >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("y {}, >= {}", y, chunk_consts::chunk_width));
    }
    if (z >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("z {}, >= {}", z, chunk_consts::chunk_width));
    }
    return operator()(x, y, z);
}

std::uint32_t vul::ChunkRLE::at(std::uint16_t idx) const noexcept(false) {
    if (idx >= chunk_consts::chunk_size) {
        throw std::out_of_range(
                fmt::format("idx {}, >= {}", idx, chunk_consts::chunk_size));
    }
    return operator[](idx);
}


bool vul::ChunkRLE::is_empty() const noexcept {
    return m_material_ids.empty();
}

std::uint16_t vul::ChunkRLE::size() const noexcept {
    return m_material_ids.size();
}

std::span<const std::uint32_t>
vul::ChunkRLE::get_material_ids() const noexcept {
    return m_material_ids;
}

std::span<const std::uint16_t> vul::ChunkRLE::get_offsets() const noexcept {
    return m_offsets;
}

std::uint16_t vul::ChunkRLE::binary_search_rle_segment(std::uint16_t linear_idx,
                                                       std::uint16_t low_idx,
                                                       std::uint16_t high_idx) const {
    UUL_DEBUG_ASSERT(!is_empty(), "Can't search an empty array");
    std::uint16_t low = low_idx;
    std::uint16_t high = high_idx;
    while (low <= high) {
        std::uint16_t mid = (high + low) / 2;
        std::uint16_t before_arr = mid > 0 ? m_offsets[mid - 1] : 0;
        //If cell index is greater than last index in RLE, ignore lower left half
        //one offsets go past the end, so offset of 1, only includes very first element.
        if (m_offsets[mid] <= linear_idx) {
            low = mid + 1u;
        }
            //If linear_idx is smaller than RLE range, ignore upper right half
        else if (before_arr > linear_idx) {
            high = mid - 1u;
        }
            //means linear_idx is present at mid
            // (linear_idx GTE than previous offset, and LT next offset).
        else {
            return mid;
        }
    }
    uul::unreachable();
}


std::uint16_t
vul::ChunkRLE::binary_search_rle_segment(std::uint16_t linear_idx) const {
    std::uint16_t low = 0u;
    std::uint16_t high = size() - 1u;
    return binary_search_rle_segment(linear_idx, low, high);
}

vul::ChunkRLE vul::ChunkRLE::from_removing_hidden(
        const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk) {
    auto chunk_span = ChunkSpan(chunk);
    std::vector<std::uint16_t> offsets;
    std::vector<std::uint32_t> materials;
    std::uint16_t running_offset = 0;
    std::uint32_t running_material = chunk[0];
    for (const auto &[idx, current_material_type]: ranges::view::enumerate(
            chunk_span)) {
        if (running_material != current_material_type &&
            !chunk_span.is_surrounded(idx)) {
            offsets.push_back(running_offset);
            materials.push_back(running_material);
            running_material = current_material_type;
        }
        running_offset += 1;
    }
    UUL_DEBUG_ASSERT(materials.back() != running_material,
               "Last value shouldn't have actually been set...");
    UUL_DEBUG_ASSERT(running_offset == chunk.size());
    offsets.push_back(running_offset);
    materials.push_back(running_material);
    return {materials, offsets};
}


vul::ChunkRLEAssisted::ChunkRLEAssisted(vul::ChunkRLE chunk_rle) : ChunkRLE(
        std::move(chunk_rle)), m_index_assist(
        construct_index_assist(get_offsets())) {

}

vul::ChunkRLEAssisted vul::ChunkRLEAssisted::from_linear(
        const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk) {
    auto chunk_rle = ChunkRLE::from_linear(chunk);
    return ChunkRLEAssisted{chunk_rle};
}

vul::ChunkRLEAssisted vul::ChunkRLEAssisted::from_removing_hidden(
        const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk) {
    auto chunk_rle = ChunkRLE::from_removing_hidden(chunk);
    return ChunkRLEAssisted{chunk_rle};
}

std::uint32_t vul::ChunkRLEAssisted::operator()(std::uint8_t x, std::uint8_t y,
                                                std::uint8_t z) const {
    UUL_DEBUG_ASSERT(x < chunk_consts::chunk_width,
               fmt::format("x {}, >= {}", x,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(y < chunk_consts::chunk_width,
               fmt::format("y {}, >= {}", y,
                           chunk_consts::chunk_width).c_str());
    UUL_DEBUG_ASSERT(z < chunk_consts::chunk_width,
               fmt::format("z {}, >= {}", z,
                           chunk_consts::chunk_width).c_str());
    return binary_search_rle_segment_assisted(chunk_consts::to_linear_idx(x, y, z));
}

std::uint32_t vul::ChunkRLEAssisted::operator[](std::uint16_t idx) const {
    UUL_DEBUG_ASSERT(idx < chunk_consts::chunk_size,
               fmt::format("idx {}, >= {}", idx,
                           chunk_consts::chunk_size).c_str());
    return binary_search_rle_segment_assisted(idx);
}

std::uint32_t vul::ChunkRLEAssisted::at(std::uint8_t x, std::uint8_t y,
                                        std::uint8_t z) const noexcept(false) {
    if (x >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("x {}, >= {}", x, chunk_consts::chunk_width));
    }
    if (y >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("y {}, >= {}", y, chunk_consts::chunk_width));
    }
    if (z >= chunk_consts::chunk_width) {
        throw std::out_of_range(
                fmt::format("z {}, >= {}", z, chunk_consts::chunk_width));
    }
    return operator()(x, y, z);
}

std::uint32_t
vul::ChunkRLEAssisted::at(std::uint16_t idx) const noexcept(false) {
    if (idx >= chunk_consts::chunk_size) {
        throw std::out_of_range(
                fmt::format("idx {}, >= {}", idx, chunk_consts::chunk_size));
    }
    return operator[](idx);
}

std::span<const std::uint16_t, vul::chunk_consts::chunk_width>
vul::ChunkRLEAssisted::get_index_assist() const noexcept {
    return m_index_assist;
}

std::uint16_t vul::ChunkRLEAssisted::binary_search_rle_segment_assisted(
        std::uint16_t linear_idx) const {
    auto y = chunk_consts::extract_y_idx(linear_idx);
    auto min_idx = y == 0 ? 0 : m_index_assist[y-1];
    auto max_idx = m_index_assist[y];
    return ChunkRLE::binary_search_rle_segment(linear_idx, min_idx, max_idx);
}

std::array<std::uint16_t, vul::chunk_consts::chunk_width>
vul::ChunkRLEAssisted::construct_index_assist(
        const std::span<const std::uint16_t> &offsets) noexcept {
    //assists on each y level
    std::array<std::uint16_t, chunk_consts::chunk_width> y_assist = {};
    std::uint8_t y_idx = 0;
    for (const auto &[offset_idx, offset]: ranges::view::enumerate(offsets)) {
        if ((offset - (y_idx * chunk_consts::chunk_layer_size)) /
            chunk_consts::chunk_layer_size > y_idx) {
            y_assist[y_idx] = offset_idx;
            UUL_DEBUG_ASSERT(y_idx < chunk_consts::chunk_width,
                       fmt::format("y_idx {} is bigger than {}", y_idx,
                                   chunk_consts::chunk_width).c_str());
            y_idx += 1;
        }
    }
    return y_assist;
}


std::span<std::uint32_t, vul::chunk_consts::chunk_size>
vul::make_chunk_span(const std::span<std::uint32_t>& span) {
    return std::span<std::uint32_t, chunk_consts::chunk_size>(span);
}
