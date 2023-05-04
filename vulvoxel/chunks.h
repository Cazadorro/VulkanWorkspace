//
// Created by Shae Bolt on 9/4/2022.
//

#ifndef VULKANWORKSPACE_CHUNKS_H
#define VULKANWORKSPACE_CHUNKS_H

#include <vector>
#include <array>
#include <span>
#include <cstdint>

#include<bit>
#include<uul/array.h>
#include<uul/bit.h>


namespace vul {
    namespace chunk_consts {
        static constexpr std::uint16_t chunk_size = 32u * 32u * 32u;
        static constexpr std::uint16_t chunk_layer_size = 32u * 32u;
        static constexpr std::uint8_t chunk_width = 32u;
        static constexpr std::uint8_t chunk_far_edge = 31;

        constexpr std::uint16_t
        to_linear_idx(std::uint8_t x, std::uint8_t y, std::uint8_t z) {
            return y * chunk_consts::chunk_width * chunk_consts::chunk_width +
                   z * chunk_consts::chunk_width + x;
        }

        struct ChunkIdx3D {
            std::uint8_t x;
            std::uint8_t y;
            std::uint8_t z;
        };

        constexpr ChunkIdx3D
        to_3d_idx(std::uint16_t idx) {
            std::uint8_t y = idx / (chunk_width * chunk_width);
            std::uint8_t z = (idx / (chunk_width)) % chunk_width;
            std::uint8_t x = idx % chunk_width;
            return {x, y, z};
        }

        constexpr std::uint8_t extract_x_idx(std::uint16_t idx) {
            return idx % chunk_width;
        }

        constexpr std::uint8_t extract_y_idx(std::uint16_t idx) {
            return idx / (chunk_width * chunk_width);
        }

        constexpr std::uint8_t extract_z_idx(std::uint16_t idx) {
            return (idx / (chunk_width)) % chunk_width;
        }
    }

    [[nodiscard]]
    std::span<std::uint32_t, chunk_consts::chunk_size>
    make_chunk_span(const std::span<std::uint32_t> &span);

    class ChunkSpan {
    public:
        explicit ChunkSpan(
                std::span<std::uint32_t, chunk_consts::chunk_size> ref_span);

        [[nodiscard]]
        std::uint32_t &
        operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z);

        [[nodiscard]]
        std::uint32_t &operator[](std::uint16_t idx);

        [[nodiscard]]
        std::uint32_t &at(std::uint8_t x, std::uint8_t y,
                          std::uint8_t z) noexcept(false);

        [[nodiscard]]
        std::uint32_t &at(std::uint16_t idx) noexcept(false);

        [[nodiscard]]
        std::uint32_t
        operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        std::uint32_t operator[](std::uint16_t idx) const;

        [[nodiscard]]
        std::uint32_t at(std::uint8_t x, std::uint8_t y,
                         std::uint8_t z) const noexcept(false);

        [[nodiscard]]
        std::uint32_t at(std::uint16_t idx) const noexcept(false);

        [[nodiscard]]
        bool is_surrounded(std::uint8_t x, std::uint8_t y, std::uint8_t z,
                           std::uint32_t empty_id = 0) const;

        [[nodiscard]]
        bool is_surrounded(std::uint16_t idx, std::uint32_t empty_id = 0) const;

        [[nodiscard]]
        std::span<const std::uint32_t, chunk_consts::chunk_size>
        data() const noexcept;

        [[nodiscard]]
        std::span<std::uint32_t, chunk_consts::chunk_size> data() noexcept;

        [[nodiscard]]
        std::span<std::uint32_t, chunk_consts::chunk_size>::iterator begin();

        [[nodiscard]]
        std::span<std::uint32_t, chunk_consts::chunk_size>::iterator end();

        [[nodiscard]]
        std::span<std::uint32_t, chunk_consts::chunk_size>::iterator
        begin() const;

        [[nodiscard]]
        std::span<std::uint32_t, chunk_consts::chunk_size>::iterator
        end() const;

    private:
        std::span<std::uint32_t, chunk_consts::chunk_size> m_ref_span;
    };

    class ChunkRLE {
    public:
        ChunkRLE() = default;

        ChunkRLE(std::vector<std::uint32_t> material_ids,
                 std::vector<std::uint16_t> offsets);

        [[nodiscard]]
        static ChunkRLE from_linear(
                const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk);

        static ChunkRLE from_removing_hidden(
                const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk);

        static ChunkRLE from_removing_id(const ChunkRLE& rle, std::uint32_t remove_id = 0u);

        [[nodiscard]]
        std::uint32_t
        operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        std::uint32_t operator[](std::uint16_t idx) const;

        [[nodiscard]]
        std::uint32_t at(std::uint8_t x, std::uint8_t y,
                         std::uint8_t z) const noexcept(false);

        [[nodiscard]]
        std::uint32_t at(std::uint16_t idx) const noexcept(false);

        [[nodiscard]]
        bool is_empty() const noexcept;

        [[nodiscard]]
        std::uint16_t size() const noexcept;

        [[nodiscard]]
        std::span<const std::uint32_t> get_material_ids() const noexcept;

        [[nodiscard]]
        std::span<const std::uint16_t> get_offsets() const noexcept;

    protected:
        [[nodiscard]]
        std::uint16_t binary_search_rle_segment(std::uint16_t linear_idx) const;

        [[nodiscard]]
        std::uint16_t binary_search_rle_segment(std::uint16_t linear_idx,
                                                std::uint16_t low_idx,
                                                std::uint16_t high_idx) const;

    private:
        //contains the index of the materials
        std::vector<std::uint32_t> m_material_ids;
        //contains the offsets to the end index in the real linear array
        std::vector<std::uint16_t> m_offsets;

    };

    class ChunkRLEAssisted : public ChunkRLE {
    public:
        ChunkRLEAssisted() = default;

        explicit ChunkRLEAssisted(ChunkRLE chunk_rle);

        [[nodiscard]]
        static ChunkRLEAssisted from_linear(
                const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk);

        static ChunkRLEAssisted from_removing_hidden(
                const std::span<std::uint32_t, chunk_consts::chunk_size> &chunk);

        [[nodiscard]]
        std::uint32_t
        operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        std::uint32_t operator[](std::uint16_t idx) const;

        [[nodiscard]]
        std::uint32_t at(std::uint8_t x, std::uint8_t y,
                         std::uint8_t z) const noexcept(false);

        [[nodiscard]]
        std::uint32_t at(std::uint16_t idx) const noexcept(false);

        [[nodiscard]]
        std::span<const std::uint16_t, chunk_consts::chunk_width>
        get_index_assist() const noexcept;

    private:
        //assists in search.
        std::array<std::uint16_t, chunk_consts::chunk_width> m_index_assist;

        [[nodiscard]]
        std::uint16_t
        binary_search_rle_segment_assisted(std::uint16_t linear_idx) const;

        [[nodiscard]]
        static std::array<std::uint16_t, chunk_consts::chunk_width>
        construct_index_assist(
                const std::span<const std::uint16_t> &offsets) noexcept;
    };

    class ChunkBitmask {
    public:
        static constexpr auto bits_per_word = (sizeof(std::uint32_t) * 8);
        static constexpr auto word_count =
                chunk_consts::chunk_size / bits_per_word;
        static constexpr auto byte_count = chunk_consts::chunk_size / 8;

        class BitReference {
        public:
            BitReference &operator=(bool value);

            operator bool() const;

        private:
            friend class ChunkBitmask;

            BitReference(ChunkBitmask *parent_ptr, std::uint16_t bit_idx);

            ChunkBitmask *m_parent_ptr;
            std::uint16_t m_bit_idx;
        };

        explicit ChunkBitmask(bool value = false);
        [[nodiscard]]
        static ChunkBitmask from_filled(const ChunkSpan& chunk_span, std::uint32_t empty_id = 0);

        [[nodiscard]]
        bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        bool operator[](std::uint16_t idx) const;

        [[nodiscard]]
        BitReference operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z);

        [[nodiscard]]
        BitReference operator[](std::uint16_t idx);

        [[nodiscard]]
        bool get(std::uint16_t idx) const;

        void set(std::uint16_t idx);

        void clear(std::uint16_t idx);

        void toggle(std::uint16_t idx);

        void assign(std::uint16_t idx, bool value);

        [[nodiscard]]
        const std::uint32_t * data() const;
        [[nodiscard]]
        std::uint16_t size() const;

        [[nodiscard]]
        std::span<const std::uint8_t, byte_count> byte_span() const;
    private:
        std::array<std::uint32_t, word_count> m_bits;
    };


    class ChunkBitmaskByteRLE {
    public:
        static constexpr auto bits_per_word = (sizeof(std::uint32_t) * 8);
        static constexpr auto word_count =
                chunk_consts::chunk_size / bits_per_word;
        ChunkBitmaskByteRLE() = default;
        ChunkBitmaskByteRLE(std::vector<std::uint8_t> empty_segments,
                            std::vector<std::uint8_t> multibyte_offset_pairs,
                            std::vector<std::uint8_t> multibyte_array);
        [[nodiscard]]
        static ChunkBitmaskByteRLE from_filled(const ChunkSpan& chunk_span, std::uint32_t empty_id = 0);

        [[nodiscard]]
        bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        bool operator[](std::uint16_t idx) const;

        [[nodiscard]]
        bool is_empty() const noexcept;

        [[nodiscard]]
        std::uint16_t size() const noexcept;

        [[nodiscard]]
        std::span<const std::uint8_t> get_byte_segments() const noexcept;

        [[nodiscard]]
        std::span<const std::uint16_t> m_heterogenous_segments() const noexcept;

    private:
        //if anything isn't empty it's assumed to be filled with something.
        //empty segments are a series of 3 byte pairs or two 12 bit values that correspond to
        //the start and stop of an empty segment.
        std::vector<std::uint8_t> m_empty_segments;
        //contains any multibyte segments to sets of bytes not just 0x00 or 0xFF.
        std::vector<std::uint8_t> m_multibyte_offset_pairs;
        //all the byte sequences which are not just 0x00 and 0xFF.
        std::vector<std::uint8_t> m_multibyte_array;
    };


    struct RLEPair{
        std::uint16_t start;
        std::uint16_t stop;
    };


    //TODO Changes when we remove single bit possibilities?
    class ChunkBitmaskBitRLE {
    public:
        ChunkBitmaskBitRLE() = default;
        ChunkBitmaskBitRLE(std::vector<RLEPair> empty_segments);
        [[nodiscard]]
        static ChunkBitmaskBitRLE from_filled(const ChunkSpan& chunk_span, std::uint32_t empty_id = 0);

        [[nodiscard]]
        bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        bool operator[](std::uint16_t idx) const;

        [[nodiscard]]
        bool is_empty() const noexcept;

        [[nodiscard]]
        std::uint16_t size() const noexcept;

        [[nodiscard]]
        std::span<const std::uint8_t> get_byte_segments() const noexcept;

        [[nodiscard]]
        std::span<const std::uint16_t> m_heterogenous_segments() const noexcept;
    private:
        std::vector<RLEPair> m_empty_segments;
    };
    struct RLEPair8{
        std::uint8_t start;
        std::uint8_t stop;
    };

    class ChunkBitmaskLayerTableBitRLE {
    public:
        static constexpr auto layer_size = 256;
        static constexpr auto table_size = chunk_consts::chunk_size / layer_size;
        ChunkBitmaskLayerTableBitRLE() = default;
        ChunkBitmaskLayerTableBitRLE(std::vector<RLEPair8> empty_segments, std::array<std::uint16_t, table_size> layer_table_offsets);
        [[nodiscard]]
        static ChunkBitmaskLayerTableBitRLE from_filled(const ChunkSpan& chunk_span, std::uint32_t empty_id = 0);

        [[nodiscard]]
        bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

        [[nodiscard]]
        bool operator[](std::uint16_t idx) const;

        [[nodiscard]]
        bool is_empty() const noexcept;

        [[nodiscard]]
        std::uint16_t size() const noexcept;

        [[nodiscard]]
        std::span<const std::uint8_t> get_byte_segments() const noexcept;

        [[nodiscard]]
        std::span<const std::uint16_t> m_heterogenous_segments() const noexcept;
    private:
        std::vector<RLEPair8> m_empty_segments;
        std::array<std::uint16_t, table_size> m_layer_table_offsets;
    };

    //chunk meta 8bit rle?



    class ChunkBitmaskGridLayer2 {
    public:
        struct GridLeaf{
            std::array<std::uint64_t,8> m_data = {};
            [[nodiscard]]
            bool operator[](std::uint16_t idx) const;
            [[nodiscard]]
            bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;

            void set(std::uint8_t x, std::uint8_t y, std::uint8_t z);

            void set(std::uint16_t idx);
        };
        ChunkBitmaskGridLayer2() = default;
        ChunkBitmaskGridLayer2(std::uint64_t top_bit_layer, std::uint64_t top_bit_layer_empty_flag, const std::vector<GridLeaf>& leaf_layer);
        [[nodiscard]]
        static ChunkBitmaskGridLayer2 from_filled(const ChunkSpan& chunk_span, std::uint32_t empty_id = 0);

        [[nodiscard]]
        bool operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;
        [[nodiscard]]
        bool get_top_layer(std::uint8_t x, std::uint8_t y, std::uint8_t z) const;
        [[nodiscard]]
        bool operator[](std::uint16_t idx) const;

        [[nodiscard]]
        bool is_empty() const noexcept;

        [[nodiscard]]
        std::uint16_t size() const noexcept;

        [[nodiscard]]
        std::uint64_t get_top_bit_layer() const noexcept;

        [[nodiscard]]
        std::span<const GridLeaf> get_leaf_layer() const noexcept;

        [[nodiscard]]
        std::uint8_t calc_top_layer_index(std::uint8_t x, std::uint8_t y, std::uint8_t z) const noexcept;
    private:
        [[nodiscard]]
        std::uint8_t calc_leaf_node_offset(std::uint8_t bit_index) const noexcept;
        std::uint64_t m_top_bit_layer;
        std::uint64_t m_top_bit_layer_empty_flag;
        static constexpr std::uint8_t top_layer_dim = 4u;
        static constexpr std::uint8_t leaf_layer_dim = 8u;
        std::vector<GridLeaf> m_leaf_layer;
    };

    //TODO need -1 size voxels removed
    //TODO need -2->4 size RLEs removed?
    //TODO need meta voxel info (shared in area?) pallete method?
}
#endif //VULKANWORKSPACE_CHUNKS_H
