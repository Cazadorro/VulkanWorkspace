//
// Created by Shae Bolt on 9/26/2021.
//

#ifndef VULKANWORKSPACE_CHUNKMANAGEMENT_H
#define VULKANWORKSPACE_CHUNKMANAGEMENT_H
#include <cstdint>
#include <array>
namespace vxl{
    template<std::size_t XDimT, std::size_t YDimT, std::size_t ZDimT>
    struct ChunkTable{
        static constexpr std::size_t SizeT = XDimT * YDimT * ZDimT;
        std::array<std::uint16_t, SizeT> m_sizes;
        std::array<std::uint32_t, SizeT> m_cumsum;
        std::array<std::uint64_t, SizeT> m_chunk_addresses;
        std::array<std::uint16_t, SizeT> m_allocation_index;
    };
}
#endif //VULKANWORKSPACE_CHUNKMANAGEMENT_H
