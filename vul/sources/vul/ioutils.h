//
// Created by Shae Bolt on 9/2/2021.
//

#ifndef VULKANWORKSPACE_IOUTILS_H
#define VULKANWORKSPACE_IOUTILS_H
#include <vector>
#include <cstdint>
#include <string>
namespace vul{
    std::vector<std::uint32_t> readSPIRV(const std::string& filepath);
}
#endif //VULKANWORKSPACE_IOUTILS_H
