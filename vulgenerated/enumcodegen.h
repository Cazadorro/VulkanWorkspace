//
// Created by Shae Bolt on 5/24/2021.
//

#ifndef VULKANPROJECT_ENUMCODEGEN_H
#define VULKANPROJECT_ENUMCODEGEN_H
#include "enuminfo.h"
#include "codegenutils.h"
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace vul{
    [[nodiscard]]
    CodeGen generate_to_string_vk_enum(const vul::EnumInfo &info,
                                       const std::string_view &namespace_str,
                                       const std::string_view &indent);
    [[nodiscard]]
    CodeGen generate_to_string_enum_class(const vul::EnumInfo &info,
                                          const std::string_view &namespace_str,
                                          const std::string_view &indent);
    [[nodiscard]]
    CodeGen generate_from_string_vk_enum(const vul::EnumInfo &info,
                                         const std::string_view &namespace_str,
                                         const std::string_view &indent);
    [[nodiscard]]
    CodeGen generate_from_string_enum_class(const vul::EnumInfo &info,
                                            const std::string_view &namespace_str,
                                            const std::string_view &indent);
    [[nodiscard]]
    CodeGen generate_enum_get(const vul::EnumInfo &info,
                              const std::string_view &namespace_str,
                              const std::string_view &indent);

    [[nodiscard]]
    std::string generate_enum_class(const vul::EnumInfo &info,
                                    const std::string_view &indent);

    [[nodiscard]]
    std::string generate_enum_operators(const vul::EnumInfo &info,
                                            const std::string_view &namespace_str,
                                    const std::string_view &indent);
    [[nodiscard]]
    std::string generate_forward_declaration(const vul::EnumInfo &info,
                                             const std::string_view &indent);
    [[nodiscard]]
    CodeGen generate_bitmask_wrapper(const vul::EnumInfo& info,
                                     const std::string_view &namespace_str,
                                     const std::string_view &indent);
    [[nodiscard]]
    std::string generate_bitmask_forward(const vul::EnumInfo& info, const std::string_view &indent);
}
#endif //VULKANPROJECT_ENUMCODEGEN_H
