//
// Created by Shae Bolt on 9/4/2022.
//

#ifndef VULKANWORKSPACE_CHUNKS_H
#define VULKANWORKSPACE_CHUNKS_H

#include <vector>
#include <array>
#include <span>
#include <cstdint>


namespace vul {
    namespace chunk_consts {
        static constexpr std::uint16_t chunk_size = 32u * 32u * 32u;
        static constexpr std::uint16_t chunk_layer_size = 32u*32u;
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

        constexpr std::uint8_t extract_x_idx(std::uint16_t idx){
            return idx % chunk_width;
        }

        constexpr std::uint8_t extract_y_idx(std::uint16_t idx){
            return idx / (chunk_width * chunk_width);
        }

        constexpr std::uint8_t extract_z_idx(std::uint16_t idx){
            return (idx / (chunk_width)) % chunk_width;
        }
    }

    [[nodiscard]]
    std::span<std::uint32_t, chunk_consts::chunk_size> make_chunk_span(const std::span<std::uint32_t>& span);
    class ChunkSpan {
    public:
        explicit ChunkSpan(std::span<std::uint32_t, chunk_consts::chunk_size> ref_span);

        [[nodiscard]]
        std::uint32_t&
        operator()(std::uint8_t x, std::uint8_t y, std::uint8_t z);

        [[nodiscard]]
        std::uint32_t& operator[](std::uint16_t idx);

        [[nodiscard]]
        std::uint32_t& at(std::uint8_t x, std::uint8_t y,
                         std::uint8_t z) noexcept(false);

        [[nodiscard]]
        std::uint32_t& at(std::uint16_t idx) noexcept(false);

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
        std::uint16_t binary_search_rle_segment_assisted(std::uint16_t linear_idx) const;
        [[nodiscard]]
        static std::array<std::uint16_t, chunk_consts::chunk_width> construct_index_assist(const std::span<const std::uint16_t>&  offsets) noexcept;
    };
    //TODO need -1 size voxels removed
    //TODO need -2->4 size RLEs removed?
    //TODO need meta voxel info (shared in area?) pallete method?
}
#endif //VULKANWORKSPACE_CHUNKS_H
