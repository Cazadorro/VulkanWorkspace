//
// Created by Shae Bolt on 5/26/2021.
//

#include "vknameutils.h"
#include "casestrings.h"
#include <algorithm>


vul::VkEnumNameInfo::VkEnumNameInfo(const vul::UpperCamelCaseStr &enum_name,
                                    const gsl::span<const std::string> &tags) {
    auto sections = enum_name.split_with_numbers();
    m_prefix = sections.front();
    auto &back = sections.back();
    if (std::any_of(tags.begin(), tags.end(), [&back](const std::string &tag) {
        return tag == back;
    })) {
        m_tag = sections.back();
    }
    if (sections.size() > 2) {
        bool all_digits = true;
        auto second_to_last_section = sections[sections.size() - 2];
        for (const auto c :  second_to_last_section) {
            if (!std::isdigit(c)) {
                all_digits = false;
                break;
            }
        }
        if (all_digits) {
            m_number = second_to_last_section;
        }
    }
    bool back_is_number = false;
    try{
        auto number = std::stoi(sections.back());
        back_is_number =true;
    }catch(std::exception&e){
        back_is_number = false;
    }
    if(back_is_number){
        m_suffix_number = sections.back();
    }
}

std::string vul::VkEnumNameInfo::get_prefix() const {
    return m_prefix;
}

std::string vul::VkEnumNameInfo::get_tag() const {
    return m_tag;
}

std::string vul::VkEnumNameInfo::get_number() const {
    return m_number;
}

vul::UpperCamelCaseStr vul::VkEnumNameInfo::extract_raw_name(
        const vul::UpperCamelCaseStr &enum_name) const {

    auto sections = enum_name.split_with_numbers();
    std::size_t start = 0;
    std::size_t end = sections.size();
    if (!m_prefix.empty()) {
        start += 1;
    }
    //don't want to remove number?
//    if(!m_number.empty()){
//        end -= 1;
//    }
    if (!m_tag.empty()) {
        end -= 1;
    }

    vul::UpperCamelCaseStr raw_name;
    for (std::size_t i = start; i < end; ++i) {
        raw_name += vul::UpperCamelCaseStr(sections[i]);
    }
    return raw_name;
}

std::size_t vul::VkEnumNameInfo::prefix_count() const {
    std::size_t count = 0;
    if (!m_number.empty()) {
        count += 1;
    }
    if (!m_prefix.empty()) {
        count += 1;
    }
    return count;
}

std::size_t vul::VkEnumNameInfo::suffix_count() const {
    std::size_t count = 0;
//    if(!m_number.empty()){
//        count += 1;
//    }
    if (!m_tag.empty()) {
        count += 1;
    }
    return count;
}

vul::UpperCamelCaseStr vul::VkEnumNameInfo::extract_name(
        const vul::UpperCamelCaseStr &enum_name) const {

    auto sections = enum_name.split_with_numbers();
    std::size_t start = 0;
    std::size_t end = sections.size();
    if (!m_prefix.empty()) {
        start += 1;
    }
    vul::UpperCamelCaseStr name;
    for (std::size_t i = start; i < end; ++i) {
        name += vul::UpperCamelCaseStr(sections[i]);
    }
    return name;
}

std::string vul::VkEnumNameInfo::get_suffix_number() const {
    return m_suffix_number;
}

std::string vul::vk_value_name(const std::string_view &string) {
    auto vk_name = vul::UpperCamelCaseStr(string);
    auto name_info = vul::VkEnumNameInfo(vk_name);
    auto name = name_info.extract_name(vk_name);
    return name.to_lower_camel_case();
}
