//
// Created by Shae Bolt on 9/4/2022.
//

#include "chunks.h"
#include <uul/assert.h>
#include <uul/unreachable.h>
#include <fmt/core.h>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/slice.hpp>
#include <range/v3/view/reverse.hpp>
#include <stdexcept>
#include <optional>


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
    UUL_DEBUG_ASSERT(!is_empty(), "Can't search an is_empty array");
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
    for (const auto &[idx, current_material_type]: ranges::views::enumerate(
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
    return binary_search_rle_segment_assisted(
            chunk_consts::to_linear_idx(x, y, z));
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
    auto min_idx = y == 0 ? 0 : m_index_assist[y - 1];
    auto max_idx = m_index_assist[y];
    return ChunkRLE::binary_search_rle_segment(linear_idx, min_idx, max_idx);
}

std::array<std::uint16_t, vul::chunk_consts::chunk_width>
vul::ChunkRLEAssisted::construct_index_assist(
        const std::span<const std::uint16_t> &offsets) noexcept {
    //assists on each y level
    std::array<std::uint16_t, chunk_consts::chunk_width> y_assist = {};
    std::uint8_t y_idx = 0;
    for (const auto &[offset_idx, offset]: ranges::views::enumerate(offsets)) {
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
vul::make_chunk_span(const std::span<std::uint32_t> &span) {
    return std::span<std::uint32_t, chunk_consts::chunk_size>(span);
}


vul::ChunkBitmask::ChunkBitmask(bool value) : m_bits(
        uul::make_array<std::uint32_t, word_count>(
                value ? 0xFFFFFFFF : 0)) {

}

bool vul::ChunkBitmask::operator()(std::uint8_t x, std::uint8_t y,
                                   std::uint8_t z) const {
    return get(chunk_consts::to_linear_idx(x, y, z));
}

bool vul::ChunkBitmask::operator[](std::uint16_t idx) const {
    return get(idx);
}

vul::ChunkBitmask::BitReference
vul::ChunkBitmask::operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) {
    return {this, chunk_consts::to_linear_idx(x, y, z)};
}

vul::ChunkBitmask::BitReference
vul::ChunkBitmask::operator[](std::uint16_t idx) {
    return {this, idx};
}

bool vul::ChunkBitmask::get(std::uint16_t idx) const {
    auto u32_index = idx / bits_per_word;
    auto bit_index = idx % bits_per_word;
    return uul::bit_get(m_bits[u32_index], bit_index);
}

void vul::ChunkBitmask::set(std::uint16_t idx) {
    auto u32_index = idx / bits_per_word;
    auto bit_index = idx % bits_per_word;
    m_bits[u32_index] = uul::bit_set(m_bits[u32_index], bit_index);
}

void vul::ChunkBitmask::clear(std::uint16_t idx) {
    auto u32_index = idx / bits_per_word;
    auto bit_index = idx % bits_per_word;
    m_bits[u32_index] = uul::bit_clear(m_bits[u32_index], bit_index);
}

void vul::ChunkBitmask::toggle(std::uint16_t idx) {
    auto u32_index = idx / bits_per_word;
    auto bit_index = idx % bits_per_word;
    m_bits[u32_index] = uul::bit_toggle(m_bits[u32_index], bit_index);
}

void vul::ChunkBitmask::assign(std::uint16_t idx, bool value) {
    auto u32_index = idx / bits_per_word;
    auto bit_index = idx % bits_per_word;
    m_bits[u32_index] = uul::bit_assign(m_bits[u32_index], bit_index, value);
}

vul::ChunkBitmask
vul::ChunkBitmask::from_filled(const vul::ChunkSpan &chunk_span,
                               std::uint32_t empty_id) {
    ChunkBitmask chunk_bitmask(false);
    for (auto [idx, value]: ranges::views::enumerate(chunk_span)) {
        if (value != empty_id) {
            chunk_bitmask[idx] = true;
        }
    }
    return chunk_bitmask;
}

const std::uint32_t *vul::ChunkBitmask::data() const {
    return m_bits.data();
}

std::uint16_t vul::ChunkBitmask::size() const {
    return m_bits.size();
}

std::span<const std::uint8_t, vul::ChunkBitmask::byte_count>
vul::ChunkBitmask::byte_span() const {
    auto byte_ptr = reinterpret_cast<const std::uint8_t*>(m_bits.data());
    return std::span<const std::uint8_t, vul::ChunkBitmask::byte_count>{byte_ptr, vul::ChunkBitmask::byte_count};
}


vul::ChunkBitmask::BitReference &
vul::ChunkBitmask::BitReference::operator=(bool value) {
    m_parent_ptr->assign(m_bit_idx, value);
    return *this;
}

vul::ChunkBitmask::BitReference::operator bool() const {
    return m_parent_ptr->get(m_bit_idx);
}

vul::ChunkBitmask::BitReference::BitReference(vul::ChunkBitmask *parent_ptr,
                                              std::uint16_t bit_idx)
        : m_parent_ptr(parent_ptr), m_bit_idx(bit_idx) {

}

vul::ChunkBitmaskByteRLE::ChunkBitmaskByteRLE(
        std::vector<std::uint8_t> empty_segments,
        std::vector<std::uint8_t> multibyte_offset_pairs,
        std::vector<std::uint8_t> multibyte_array) :
        m_empty_segments(std::move(empty_segments)),
        m_multibyte_offset_pairs(std::move(multibyte_offset_pairs)),
        m_multibyte_array(std::move(multibyte_array))

{

}

std::array<std::uint8_t, 3> merge_12bit(std::uint16_t first, std::uint16_t second) {
    std::array<std::uint8_t, 3> ret = {};
    std::uint32_t merged = first;
    merged |= std::uint32_t(second) << 12;
    ret[0] = merged;
    ret[1] = merged >> 8;
    ret[2] = merged >> 16;
    return ret;
}

enum class AccumulationType {
    None,
    Empty,
    Filled,
    Multi
};

struct RLEStartStop {
    std::uint16_t start;
    std::uint16_t stop;
};

enum class RLEPairByteType {
    Empty,
    Filled,
    Multi
};

struct RLEByteTypePair {
    RLEPairByteType type;
    RLEStartStop pair;
};

vul::ChunkBitmaskByteRLE
vul::ChunkBitmaskByteRLE::from_filled(const vul::ChunkSpan &chunk_span,
                                      std::uint32_t empty_id) {
    using namespace ranges;

    std::uint8_t empty_byte = 0x00;
    std::uint8_t full_byte = 0xFF;
    std::uint16_t running_start_idx = 0;


    std::vector<std::uint8_t> temp_bytes;
    AccumulationType current_accumulation_type = AccumulationType::None;
    auto chunk_bitmask = ChunkBitmask::from_filled(chunk_span, empty_id);

    std::vector<RLEByteTypePair> pairs;

    auto chunk_bitmask_byte_span = chunk_bitmask.byte_span();
    for( auto [idx, current_byte] : views::enumerate(chunk_bitmask_byte_span) ) {
        switch(current_accumulation_type) {
            using enum AccumulationType;
            case None: {
                if (current_byte == empty_byte) {
                    current_accumulation_type = AccumulationType::Empty;
                } else if (current_byte == full_byte) {
                    current_accumulation_type = AccumulationType::Filled;
                } else {//(current_byte != full_byte)
                    current_accumulation_type = AccumulationType::Multi;
                }
                running_start_idx = idx;
                break;
            }
            case Empty: {
                if (current_byte == empty_byte) {
                    //nothing
                } else {
                    pairs.emplace_back(RLEPairByteType::Empty, RLEStartStop{running_start_idx, static_cast<std::uint16_t>(idx - 1u)});
                    running_start_idx = idx;
                    if (current_byte == full_byte) {
                        current_accumulation_type = AccumulationType::Filled;
                    } else {//(current_byte != full_byte)
                        current_accumulation_type = AccumulationType::Multi;
                    }
                }
                break;
            }
            case Filled: {
                if (current_byte == full_byte) {
                    //nothing
                } else {
                    pairs.emplace_back(RLEPairByteType::Filled, RLEStartStop{running_start_idx, static_cast<std::uint16_t>(idx - 1u)});
                    running_start_idx = idx;
                    if (current_byte == empty_byte) {
                        current_accumulation_type = AccumulationType::Empty;
                    } else {//(current_byte != full_byte)
                        current_accumulation_type = AccumulationType::Multi;
                    }
                }
                break;
            }
            case Multi: {
                if (current_byte == full_byte || current_byte == empty_byte ) {
                    pairs.emplace_back(RLEPairByteType::Multi, RLEStartStop{running_start_idx, static_cast<std::uint16_t>(idx - 1u)});
                    running_start_idx = idx;
                    if (current_byte == empty_byte) {
                        current_accumulation_type = AccumulationType::Empty;
                    } else {//(current_byte != full_byte)
                        current_accumulation_type = AccumulationType::Filled;
                    }
                } else { //multi
                    //nothing!
                }
                break;
            }
        }
    }

    std::vector<RLEByteTypePair> new_pairs;
    auto last_pair = pairs.back();
    bool checking_merge_of_maybe_empty = false;
    bool checking_merge_of_multi = false;
    if(last_pair.type == RLEPairByteType::Empty) {
        checking_merge_of_maybe_empty = true;
    }
    //checking for previous empty_merges.

    auto empty_byte_cost = 3;
    auto filled_byte_cost = 0;
    auto multi_offset_cost = 3;

    std::size_t previous_multi_index = 0xFFFF;
    RLEByteTypePair previous_multi_pair ={};
    auto end_index = pairs.size() - 1;
    for(auto [idx, current_pair] : pairs | views::enumerate) {
        if(current_pair.type == RLEPairByteType::Multi) {
            previous_multi_index = idx;
            previous_multi_pair = current_pair;
            if(idx == end_index){
                new_pairs.push_back(current_pair);
            }
            break;
        }else {
            new_pairs.push_back(current_pair);
        }
    }



    std::uint16_t empty_count = 0;

    //forward check for merging more optimal.
    //for(std::uint16_t i = previous_multi_index + 1; i < pairs.size(); ++i)
    //inner loop for(std::uint16_t look_back_idx = previous_multi_index + 1; look_back_idx <= index; ++look_back_idx)
    for(auto [idx, current_pair] : pairs | views::enumerate | views::slice(previous_multi_index + 1, pairs.size()) ){
        if(current_pair.type == RLEPairByteType::Empty) {
            empty_count += 1;
            auto between_byte_distance = (current_pair.pair.stop+1) - previous_multi_pair.pair.stop;
            auto merge_threshold = (empty_byte_cost * empty_count); // 3 bytes per empty
            if(between_byte_distance < merge_threshold) {
                previous_multi_pair.pair.stop = current_pair.pair.stop;
                previous_multi_index = idx;
                if(idx == end_index) {
                    new_pairs.push_back(previous_multi_pair);
                }
                empty_count = 0;
            }else if(idx == end_index){
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                for(auto look_back_pair : pairs | views::slice(previous_multi_index + 1, idx + 1)){
                    new_pairs.push_back(look_back_pair);
                }
            }

        }else if(current_pair.type == RLEPairByteType::Multi) {
            auto between_byte_distance = (current_pair.pair.start+1) - previous_multi_pair.pair.stop;
            auto merge_threshold = multi_offset_cost + (empty_byte_cost * empty_count); //3 bytes per empty plus the extra multi pair
            if(between_byte_distance < merge_threshold) {
                previous_multi_pair.pair.stop = current_pair.pair.stop;
            }else {
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                for(auto look_back_pair : pairs | views::slice(previous_multi_index + 1, idx)){
                    new_pairs.push_back(look_back_pair);
                }
                previous_multi_pair = current_pair;

            }
            if(idx == end_index) {
                new_pairs.push_back(previous_multi_pair);
            }
            previous_multi_index = idx;
            empty_count = 0;
        }else {//filled
            if(idx == end_index){
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                for(auto look_back_pair : pairs | views::slice(previous_multi_index + 1, idx + 1)){
                    new_pairs.push_back(look_back_pair);
                }
            }
        }
    }


    //TODO assert all values for pairs "connect" (1,4),(5,19),(20,23) etc...
    pairs = new_pairs;
    new_pairs.clear();


    previous_multi_index = 0xFFFF;
    previous_multi_pair ={};
    end_index = 0;
    for(auto [idx, current_pair] : pairs | views::enumerate | views::reverse) {
        if(current_pair.type == RLEPairByteType::Multi) {
            previous_multi_index = idx;
            previous_multi_pair = current_pair;
            if(idx == end_index){
                new_pairs.push_back(current_pair);
            }
            break;
        }else {
            new_pairs.push_back(current_pair);
        }
    }

    for(auto [idx, current_pair] : pairs | views::enumerate | views::slice(std::size_t(0), previous_multi_index) | views::reverse){
        if(current_pair.type == RLEPairByteType::Empty) {
            empty_count += 1;
            auto between_byte_distance = (previous_multi_pair.pair.start+1) - current_pair.pair.start;
            auto merge_threshold = (empty_byte_cost * empty_count); // 3 bytes per empty
            if(between_byte_distance < merge_threshold) {
                previous_multi_pair.pair.stop = current_pair.pair.stop;
                previous_multi_index = idx;
                if(idx == end_index) {
                    new_pairs.push_back(previous_multi_pair);
                }
                empty_count = 0;
            }else if(idx == end_index){
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                for(auto look_back_pair : pairs | views::slice(idx, previous_multi_index) | views::reverse){
                    new_pairs.push_back(look_back_pair);
                }
            }

        }else if(current_pair.type == RLEPairByteType::Multi) {

            auto between_byte_distance = (previous_multi_pair.pair.start+1) - current_pair.pair.stop;
            auto merge_threshold = multi_offset_cost + (empty_byte_cost * empty_count); //3 bytes per empty plus the extra multi pair
            //TODO if part shouldn't need to be here because should have been handled in other loop
            if(between_byte_distance < merge_threshold) {
                previous_multi_pair.pair.start = current_pair.pair.start;
            }else {
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                UUL_DEBUG_ASSERT((idx+1) < previous_multi_index);
                for(auto look_back_pair : pairs | views::slice(idx+1, previous_multi_index) | views::reverse){
                    new_pairs.push_back(look_back_pair);
                }
                previous_multi_pair = current_pair;
            }
            if(idx == end_index) {
                new_pairs.push_back(previous_multi_pair);
            }
            previous_multi_index = idx;
            empty_count = 0;
        }else {//filled
            if(idx == end_index){
                new_pairs.push_back(previous_multi_pair);
                //fill in skipped pairs that were potential merges.
                for(auto look_back_pair : pairs | views::slice(idx, previous_multi_index) | views::reverse){
                    new_pairs.push_back(look_back_pair);
                }
            }
        }
    }
    //TODO, reverse traverse *first* then forward traverse to save on having to reverse again
    pairs = new_pairs | views::reverse | to<std::vector>();

    std::vector<std::uint8_t> empty_segments;
    std::vector<std::uint8_t> multibyte_offset_pairs;
    std::vector<std::uint8_t> multibyte_array;
    for(const auto value : pairs){
        if(value.type == RLEPairByteType::Empty){
            auto merged_bytes = merge_12bit(value.pair.start, value.pair.stop);
            for(const auto byte : merged_bytes){
                empty_segments.push_back(byte);
            }
        } else if (value.type == RLEPairByteType::Multi){
            auto byte_distance = (value.pair.stop + 1) - value.pair.start;
            auto merged_bytes = merge_12bit(value.pair.stop, multibyte_array.size() + byte_distance - 1);
            for(const auto byte : merged_bytes){
                multibyte_offset_pairs.push_back(byte);
            }
            for(const auto multi_byte : chunk_bitmask_byte_span | views::slice(value.pair.start, static_cast<std::uint16_t>(value.pair.stop + 1))){
                multibyte_array.push_back(multi_byte);
            }
        }
    }


    return {empty_segments, multibyte_offset_pairs, multibyte_array};
}

vul::ChunkBitmaskBitRLE::ChunkBitmaskBitRLE(
        std::vector<RLEPair> empty_segments) : m_empty_segments(std::move(empty_segments)) {

}

vul::ChunkBitmaskBitRLE
vul::ChunkBitmaskBitRLE::from_filled(const vul::ChunkSpan &chunk_span,
                                     std::uint32_t empty_id) {
    auto chunk_bitmask = ChunkBitmask::from_filled(chunk_span, empty_id);
    std::vector<RLEPair> empty_segments;
    std::uint16_t start_idx = 0xFFFF;
    for(std::uint16_t i = 0; i < chunk_consts::chunk_size; ++i){
        if(chunk_bitmask[i]){
            if(start_idx == 0xFFFF){
                continue;
            }else{
                empty_segments.emplace_back(start_idx, i);
                start_idx = 0xFFFF;
            }
        }else{
            if(start_idx == 0xFFFF){
                start_idx = i;
            }
        }
    }
    if(start_idx != 0xFFFF){
        empty_segments.emplace_back(start_idx, chunk_consts::chunk_size);
    }
    return {empty_segments};
}

vul::ChunkBitmaskLayerTableBitRLE::ChunkBitmaskLayerTableBitRLE(
        std::vector<RLEPair8> empty_segments, std::array<std::uint16_t, table_size> layer_table_offsets) : m_empty_segments(std::move(empty_segments)), m_layer_table_offsets((layer_table_offsets)) {

}

vul::ChunkBitmaskLayerTableBitRLE
vul::ChunkBitmaskLayerTableBitRLE::from_filled(const vul::ChunkSpan &chunk_span,
                                               std::uint32_t empty_id) {

    auto chunk_bitmask = ChunkBitmask::from_filled(chunk_span, empty_id);
    std::vector<RLEPair8> empty_segments;
    std::array<std::uint16_t, table_size> layer_table_offsets = {};
    for(std::uint8_t table_layer = 0; table_layer < table_size; ++table_layer) {
        auto layer_start = table_layer * layer_size;
        auto layer_stop = (table_layer + 1) * layer_size;
        std::uint16_t start_idx = 0xFFFF;
        for (std::uint16_t i = layer_start; i < layer_stop; ++i) {
            if (chunk_bitmask[i]) {
                if (start_idx == 0xFFFF) {
                    continue;
                } else {
                    //TODO inclusive end right now...?
                    empty_segments.emplace_back(start_idx % layer_size, (i-1) % layer_size);
                    start_idx = 0xFFFF;
                }
            } else {
                if (start_idx == 0xFFFF) {
                    start_idx = i;
                }
            }
        }
        if (start_idx != 0xFFFF && start_idx % layer_size != 0) {
                empty_segments.emplace_back(start_idx % layer_size, 255);
        }
        if(start_idx % layer_size == 0){
            //if completely empty, we mark it here.  If completely full, will just be the same endpoint as the last one.
            layer_table_offsets[table_layer] = static_cast<std::uint16_t>(empty_segments.size()) | 0b1000'0000'0000'0000;
        }else{
            layer_table_offsets[table_layer] = empty_segments.size();
        }

    }
    //TODO could remove 2 bytes by ignoring last 2 bytes in table?
    //TODO could save bytes by only storing 8bit offsets (total saved 128bytes), instead of cumulative offsets, but requires more work in the shader (basically on average 64 bytes need to be touched per iteration?))
    //TODO could use table index itself to indicate empty start-stop list? save 2 bytes per start stop that's empty (in half filled, it's 128 bytes?)
    //TODO could use both RLE non table and this version on GPU, as this would only change the code a bit. Guessing won't matter though if it's mostly the same threads per block?
    return {empty_segments, layer_table_offsets};
}
