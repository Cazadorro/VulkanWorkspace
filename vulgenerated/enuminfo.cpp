//
// Created by Shae Bolt on 5/23/2021.
//

#include "enuminfo.h"
#include <pugixml.hpp>
#include <fmt/core.h>
#include <algorithm>



vul::EnumInfo::EnumInfo(const pugi::xml_node &enums_node,
                        const gsl::span<const std::string> &tags,
                        const std::string_view &underlying_type,
                        bool removes_prefix_t,
                        const std::string_view& define_string) : vk_name(
        enums_node.attribute("name").as_string()), removes_prefix(
        removes_prefix_t),
                                                 underlying_type(
                                                         underlying_type), define_string(define_string) {
    vk_name_info = VkEnumNameInfo(vk_name, tags);
    raw_name = vk_name_info.extract_raw_name(vk_name);
    name = vk_name_info.extract_name(vk_name);
    std::string type_str = enums_node.attribute("type").as_string();
    if (type_str == "enum") {
        type = EnumType::Enum;
    } else if (type_str == "bitmask") {
        type = EnumType::Bitmask;
    } else {
        throw std::runtime_error(
                fmt::format("Expected 'enum' or 'bitmask' found {}", type_str));
    }

    for (auto child : enums_node.children("enum")) {
        append(child);
    }
}

vul::EnumInfo &
vul::EnumInfo::append(const pugi::xml_node& child_node,  const std::string_view& child_define_string) {
    if (child_node.attribute("comment").as_string() ==
        std::string("Backwards-compatible alias containing a typo")) {
        return *this;
    }
    auto child_vk_name = vul::UpperSnakeCaseStr(child_node.attribute("name").as_string());
    if(m_already_set.find(child_vk_name.string()) != m_already_set.end()){
        return *this;
    }else{
        m_already_set.insert(child_vk_name.string());
    }
    auto child_alias_string = child_node.attribute("alias").as_string();
    std::size_t prefix_count = vk_name_info.prefix_count() + (vk_name_info.get_suffix_number().empty() ? 0 : 1);
    if (removes_prefix) {
        prefix_count += raw_name.element_count();
        if (type == EnumType::Bitmask) {
            prefix_count -= 2; //Flag and Bits need to be ignored in prefix, as they don't show up.
        }
    }
    std::size_t suffix_count = vk_name_info.suffix_count();
    vul::UpperCamelCaseStr child_name;
    if (!vk_name_info.get_tag().empty() &&
        child_vk_name.get_tag() != vk_name_info.get_tag()) {
        child_name = vul::UpperCamelCaseStr(
                child_vk_name.strip_elements(prefix_count, suffix_count - 1));
    } else {
        child_name = vul::UpperCamelCaseStr(
                child_vk_name.strip_elements(prefix_count, suffix_count));
    }
    if (std::isdigit((child_name.string().front()))) {
        child_name = vul::UpperCamelCaseStr("_") + child_name;
    }
    enum_values.push_back(EnumValueInfo{child_vk_name, child_name, std::string(child_define_string), child_alias_string});
    return *this;
}

const std::vector<vul::EnumValueInfo> &vul::EnumInfo::get_enum_values() const {
    return enum_values;
}




