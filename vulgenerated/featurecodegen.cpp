//
// Created by Shae Bolt on 5/26/2021.
//

#include "featurecodegen.h"
#include "vknameutils.h"
#include "codegenutils.h"
#include "casestrings.h"

#include <pugixml.hpp>
#include <fmt/core.h>
#include <algorithm>

vul::FeatureStructInfo::FeatureStructInfo(const pugi::xml_node &node) {
    m_name = vul::get_name(node);
    auto current_child = node.first_child();
    m_structure_value = current_child.attribute("values").as_string();
    //skip over sType and pNext;
    auto feature_member_start = (m_structure_value.empty()) ? node.begin()
                                                            : ++(++(node.begin()));
    for (auto it = feature_member_start; it != node.end(); ++it) {
        auto member_type = get_text(*it, "type");
        auto member_name = get_text(*it, "name");
        m_members.push_back({member_type, member_name});
    }
}

std::string vul::FeatureStructInfo::get_name() const {
    return m_name;
}

std::string vul::FeatureStructInfo::get_structure_value() const {
    return m_structure_value;
}

const std::vector<vul::FeatureMemberInfo> &
vul::FeatureStructInfo::get_members() const {
    return m_members;
}


std::string generate_member_compare(const vul::FeatureStructInfo &feature_info,
                                    const std::string_view &indent,
                                    const std::string_view &namespace_str) {
    std::string match_definition = fmt::format(
            "bool {1}::matches(const {0}& lhs, const {0}& rhs){{\n",
            feature_info.get_name(), namespace_str);
    for (const auto &member_info: feature_info.get_members()) {
        match_definition += fmt::format(
                "{0}if(lhs.{1} == VK_FALSE && rhs.{1} == VK_TRUE){{ return false; }}\n",
                indent, member_info.type_name);
    }
    match_definition += fmt::format("{0}return true;\n"
                                    "}}\n", indent);
    return match_definition;
}

vul::CodeGen vul::generate_feature_struct_code(
        const std::vector<FeatureStructInfo> &feature_infos,
        const FeatureStructInfo &physical_device_features_info,
        const std::string_view &indent,
        const std::string_view &namespace_str,
        const std::unordered_map<std::string, std::string> &type_platform_define_map) {
    auto pdf_type_name = physical_device_features_info.get_name();
    std::string member_declarations;

    std::string match_declarations;

    match_declarations += fmt::format("[[nodiscard]]\n"
            "bool matches(const {0}& lhs, const {0}& rhs);\n", pdf_type_name);
    for (const auto &feature_info : feature_infos) {
        auto feature_type_name = feature_info.get_name();
        auto feature_value_name = vk_value_name(feature_type_name);
        auto platform_itr = type_platform_define_map.find(feature_type_name);
        if (platform_itr != type_platform_define_map.end()) {
            member_declarations += start_define(platform_itr->second);
            match_declarations += start_define(platform_itr->second);
        }

        member_declarations += fmt::format("{0}{1} {2} = {{}};\n", indent,
                                           feature_type_name,
                                           feature_value_name);
        match_declarations += fmt::format("[[nodiscard]]\n");
        match_declarations += fmt::format(
                "bool matches(const {0}& lhs, const {0}& rhs);\n",
                feature_info.get_name());
        if (platform_itr != type_platform_define_map.end()) {
            member_declarations += end_define(platform_itr->second);
            match_declarations += end_define(platform_itr->second);
        }
    }
    std::string declaration = fmt::format("class Features{{\n"
                                          "public:\n"
                                          "{0}Features();\n"
                                          "{0}explicit Features(VkPhysicalDevice physicalDevice);\n"
                                          "{0}[[nodiscard]]\n"
                                          "{0}{1}& getPhysicalDeviceFeatures();\n"
                                          "{0}[[nodiscard]]\n"
                                          "{0}const {1}& getPhysicalDeviceFeatures() const;\n"
                                          "[[nodiscard]]\n"
                                          "{0}const VkPhysicalDeviceFeatures2& get() const;\n"
                                          "{2}"
                                          "private:\n"
                                          "{0}VkPhysicalDeviceFeatures2 physicalDeviceFeatures2;\n"
                                          "}};\n"
                                          "[[nodiscard]]\n"
                                          "bool matches(const Features& lhs, const Features& rhs);\n",
                                          indent,
                                          pdf_type_name,
                                          member_declarations
    );
    declaration += match_declarations;

    std::string match_definitions = generate_member_compare(
            physical_device_features_info, indent, namespace_str);
    std::string constructor_definitions = fmt::format(
            "{0}void**prev_pNext = &(physicalDeviceFeatures2.pNext);\n",
            indent);
    std::string feature_match_definition = fmt::format(
            "bool {0}::matches(const {0}::Features& lhs, const {0}::Features& rhs){{\n",
            namespace_str);
    feature_match_definition += fmt::format(
            "{0}return matches(lhs.{1}, rhs.{1})", indent, "getPhysicalDeviceFeatures()");
    std::unordered_map<std::string, std::string> previous_non_version_feature_struct;
    for (const auto &feature_info : feature_infos) {
        auto feature_type_name = feature_info.get_name();
        auto feature_value_name = vk_value_name(feature_type_name);
        auto feature_name_value = feature_info.get_structure_value();
        auto platform_itr = type_platform_define_map.find(feature_type_name);
        if (platform_itr != type_platform_define_map.end()) {
            constructor_definitions += start_define(platform_itr->second);
            feature_match_definition +=
                    "\n" + start_define(platform_itr->second);
            match_definitions += start_define(platform_itr->second);
        }
        constructor_definitions += fmt::format("{0}*prev_pNext = &{1};\n",
                                               indent,
                                               feature_value_name);
        constructor_definitions += fmt::format("{0}{2}.sType = {1};\n", indent,
                                               feature_name_value,
                                               feature_value_name);
        constructor_definitions += fmt::format(
                "{0}prev_pNext = &({1}.pNext);\n", indent, feature_value_name);
        match_definitions += generate_member_compare(feature_info, indent,
                                                     namespace_str);
        feature_match_definition += fmt::format(
                "\n{0}{0}&& matches(lhs.{1}, rhs.{1})", indent,
                feature_value_name);
        if (platform_itr != type_platform_define_map.end()) {
            constructor_definitions += end_define(platform_itr->second);
            feature_match_definition += "\n" + end_define(platform_itr->second);
            match_definitions += end_define(platform_itr->second);
        }
    }
    feature_match_definition += fmt::format(";\n"
                                            "}}\n");
    constructor_definitions += fmt::format("{0}*prev_pNext = {1};\n", indent,
                                           "nullptr");
    std::string definition = fmt::format(
            "{1}::Features::Features(){{\n"
            "{0}physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;\n"
            "{0}physicalDeviceFeatures2.features = {{}};\n"
            "{2}"
            "}}\n",
            indent,
            namespace_str,
            constructor_definitions);

    definition += fmt::format(
            "{1}::Features::Features(VkPhysicalDevice physicalDevice) : vul::Features() {{\n"
            "{0}vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);\n"
            "}}\n",
            indent,
            namespace_str);

//    "{0}const VkPhysicalDeviceFeatures2& get() const;\n"
    definition += fmt::format("const VkPhysicalDeviceFeatures2& {1}::Features::get() const{{\n"
                "{0}return physicalDeviceFeatures2;\n"
                "}}\n",
                indent,
                namespace_str);
    definition += fmt::format(
            "{2}& {1}::Features::getPhysicalDeviceFeatures(){{\n"
            "{0}return physicalDeviceFeatures2.features;\n"
            "}}\n"
            "const {2}& {1}::Features::getPhysicalDeviceFeatures() const{{\n"
            "{0}return physicalDeviceFeatures2.features;\n"
            "}}\n",
            indent, namespace_str, pdf_type_name);

    definition += match_definitions;
    definition += feature_match_definition;
    return {declaration, definition};
}

std::string vul::generate_feature_struct_forward(
        const std::vector<FeatureStructInfo> &feature_infos) {
    return std::string();
}
