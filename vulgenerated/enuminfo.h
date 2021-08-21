//
// Created by Shae Bolt on 5/23/2021.
//

#ifndef VULKANPROJECT_ENUMINFO_H
#define VULKANPROJECT_ENUMINFO_H
#include "vknameutils.h"
#include "casestrings.h"
#include <string>
#include <string_view>
#include <unordered_set>
namespace pugi{
    class xml_node;
}
namespace vul{

    struct EnumValueNameInfo{
        vul::UpperSnakeCaseStr vk_name;
        vul::UpperCamelCaseStr name;
    };



    enum class EnumType{
        Enum,
        Bitmask
    };

    struct EnumValueInfo{
        vul::UpperSnakeCaseStr vk_name;
        vul::UpperCamelCaseStr name;
        std::string define_string;
        std::string alias_string;
    };

    class EnumInfo{
    public:
        vul::UpperCamelCaseStr vk_name;
        VkEnumNameInfo vk_name_info;
        vul::UpperCamelCaseStr raw_name;
        vul::UpperCamelCaseStr name;
        EnumType type;
        std::vector<EnumValueInfo> enum_values;
        bool removes_prefix;
        std::string underlying_type;
        std::string define_string;
        EnumInfo()=default;
        explicit EnumInfo(const pugi::xml_node &enums_node,  const gsl::span<const std::string>& tags = {}, const std::string_view& underlying_type = {},  bool removes_prefix = true, const std::string_view& define_string = {});
        EnumInfo& append(const pugi::xml_node& child_node, const std::string_view& child_define_string = {});
        [[nodiscard]]
        const std::vector<EnumValueInfo>& get_enum_values() const;
    private:
        std::unordered_set<std::string> m_already_set;
    };
}
#endif //VULKANPROJECT_ENUMINFO_H
