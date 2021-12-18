//
// Created by Shae Bolt on 9/6/2021.
//

#ifndef VULKANWORKSPACE_BITMASK_H
#define VULKANWORKSPACE_BITMASK_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <cstdint>

namespace vul {
    class bitmask {
    public:
        explicit bitmask(std::size_t size_bits);
        [[nodiscard]]
        bool get(std::uint32_t pos);
        [[nodiscard]]
        bool get(std::int32_t pos);
        void set(std::uint32_t pos);
        void set(std::int32_t pos);
        void clear(std::uint32_t pos);
        void clear(std::int32_t pos);
        [[nodiscard]]
        bool get(glm::uvec3 pos, glm::uvec3 size);
        [[nodiscard]]
        bool get(glm::ivec3 pos, glm::ivec3 size);
        void set(glm::uvec3 pos, glm::uvec3 size);
        void set(glm::ivec3 pos, glm::ivec3 size);
        void clear(glm::uvec3 pos, glm::uvec3 size);
        void clear(glm::ivec3 pos, glm::ivec3 size);
        [[nodiscard]]
        bool get(glm::uvec2 pos, glm::uvec2 size);
        [[nodiscard]]
        bool get(glm::ivec2 pos, glm::ivec2 size);
        void set(glm::uvec2 pos, glm::uvec2 size);
        void set(glm::ivec2 pos, glm::ivec2 size);
        void clear(glm::uvec2 pos, glm::uvec2 size);
        void clear(glm::ivec2 pos, glm::ivec2 size);

        [[nodiscard]]
        const std::uint32_t* data() const;
        [[nodiscard]]
        std::size_t size() const;
    private:
        static constexpr auto m_bit_count = 32;
        std::vector<std::uint32_t> m_data;
    };
}


#endif //VULKANWORKSPACE_BITMASK_H
