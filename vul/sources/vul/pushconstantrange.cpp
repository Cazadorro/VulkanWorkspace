//
// Created by Bolt on 5/4/2023.
//

#include "pushconstantrange.h"

vul::PushConstantRange::PushConstantRange(vul::ShaderStageBitMask stageFlags, std::uint32_t offset,
                                          std::uint32_t size) : stageFlags(stageFlags), offset(offset), size(size) {

}

vul::PushConstantRange::PushConstantRange(VkPushConstantRange pushConstantRange) : PushConstantRange(
        static_cast<vul::ShaderStageBitMask>(pushConstantRange.stageFlags), pushConstantRange.offset,
        pushConstantRange.size) {

}

vul::PushConstantRange::operator VkPushConstantRange() const {
    return get();
}

VkPushConstantRange &vul::PushConstantRange::get() {
    return *reinterpret_cast<VkPushConstantRange *>(this);
}

const VkPushConstantRange &vul::PushConstantRange::get() const {
    return *reinterpret_cast<const VkPushConstantRange *>(this);
}