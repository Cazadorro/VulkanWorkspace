//
// Created by Bolt on 5/4/2023.
//

#ifndef VULKANWORKSPACE_PUSHCONSTANTRANGE_H
#define VULKANWORKSPACE_PUSHCONSTANTRANGE_H


#include "vul/enumsfwd.h"
#include <uul/enumflags.h>
#include <vulkan/vulkan.h>
#include <cstdint>

namespace vul {

    //    struct Rect2D {
    //        VkOffset2D offset = {};
    //        VkExtent2D extent = {};
    //
    //        explicit Rect2D(const VkRect2D& rect);
    //        Rect2D(const VkOffset2D &offset,
    //               const VkExtent2D &extent);
    //
    //        explicit Rect2D(const VkExtent2D &extent);
    //
    //        operator VkRect2D() const;
    //        [[nodiscard]]
    //        VkRect2D &get();
    //        [[nodiscard]]
    //        const VkRect2D &get() const;
    //    };
    struct PushConstantRange {
        uul::EnumFlags<vul::ShaderStageFlagBits> stageFlags;
        std::uint32_t offset;
        std::uint32_t size;

        PushConstantRange() = default;


        explicit PushConstantRange(uul::EnumFlags<vul::ShaderStageFlagBits> stageFlags, std::uint32_t offset, std::uint32_t size);

        explicit PushConstantRange(VkPushConstantRange pushConstantRange);

        template<typename T>
        [[nodiscard]]
        static PushConstantRange create(uul::EnumFlags<vul::ShaderStageFlagBits> stageFlags, std::uint32_t offset = 0){
            return  PushConstantRange(stageFlags,
                                      offset,
                                      sizeof(T));
        }
        operator VkPushConstantRange() const;
        [[nodiscard]]
        VkPushConstantRange& get();
        [[nodiscard]]
        const VkPushConstantRange& get() const;
    };

}
#endif //VULKANWORKSPACE_PUSHCONSTANTRANGE_H
