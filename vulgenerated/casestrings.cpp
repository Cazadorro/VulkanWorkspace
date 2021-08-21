//
// Created by Shae Bolt on 5/20/2021.
//

#include "casestrings.h"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <algorithm>


bool
vul::end_matches(const std::string_view &lhs, const std::string_view &rhs) {
    if (lhs.size() < rhs.size()) {
        return false;
    }
    std::string_view::size_type start = lhs.size() - rhs.size();
    return lhs.substr(start, std::string_view::npos) == rhs;
}


vul::UpperCamelCaseStr::UpperCamelCaseStr(const std::string_view &string)
        : m_string(string) {

}

vul::UpperCamelCaseStr
vul::UpperCamelCaseStr::from_validated(const std::string_view &string,
                                       const gsl::span<const std::string> &tags) noexcept(false) {
    if (!validate(string, tags)) {
        throw std::runtime_error(
                fmt::format("string {} is not UpperCamelCaseStr", string));
    }
    return UpperCamelCaseStr(string);
}

std::string vul::UpperCamelCaseStr::string() const {
    return m_string;
}
std::string_view vul::UpperCamelCaseStr::string_view() const {
    return m_string;
}

vul::UpperCamelCaseStr &
vul::UpperCamelCaseStr::operator+=(const vul::UpperCamelCaseStr &rhs) {
    m_string += rhs.m_string;
    return *this;
}

bool vul::UpperCamelCaseStr::validate(const std::string_view &string,
                                      const gsl::span<const std::string> &tags) {
    if (string.empty()) {
        return true;
    }
    std::string matched_tag;
    for (const auto &tag : tags) {
        if (end_matches(string, tag)) {
            matched_tag = tag;
        }
    }
    std::size_t tag_start_idx = string.size() - matched_tag.size();
    if (!matched_tag.empty() && ((string.size() + 1) > matched_tag.size())) {
        if (std::isalpha(string[tag_start_idx - 1]) &&
            std::isupper(string[tag_start_idx - 1])) {
            return false;
        }
    }
    auto substr = string.substr(0, string.size() - matched_tag.size());
    if (std::isalpha(substr[0]) && std::islower(substr[0])) {
        return false;
    }
    return !std::any_of(substr.begin() + 1, substr.end(), [](char c) {
        return c == '_';
    });
}

vul::UpperCamelCaseStr::UpperCamelCaseStr(
        const vul::UpperSnakeCaseStr &string,
        const gsl::span<const std::string> &tags): m_string(to_upper_camel_case(string, tags)){
}



std::vector<std::string> vul::UpperCamelCaseStr::split() const {
    std::vector<std::string> sections;
    bool last_char_lower = false;
    std::size_t section_start = 0;
    for (std::size_t i = 1; i < m_string.size(); ++i) {
        auto c = m_string[i];
        if (std::isupper(c)) {
            if (last_char_lower) {
                std::size_t section_length = i - section_start;
                sections.push_back(
                        m_string.substr(section_start, section_length));
                section_start = i;
            }
            last_char_lower = false;
        } else {
            last_char_lower = true;
        }
    }
    //will catch extensions
    std::size_t section_length = m_string.size() - section_start;
    sections.push_back(
            m_string.substr(section_start, section_length));

    return sections;
}

std::string
vul::UpperCamelCaseStr::capital_to_camel(const std::string_view &string) {
    std::string camel_string(string);
    bool last_value_digit =
            !camel_string.empty() && std::isdigit(camel_string[0]);
    for(std::size_t i = 1; i < camel_string.size(); ++i){
        if(!last_value_digit){
            camel_string[i] = static_cast<char>(std::tolower(camel_string[i]));
        }
    }
    return camel_string;
}

std::string
vul::UpperCamelCaseStr::to_upper_camel_case(const UpperSnakeCaseStr& string,
                                            const gsl::span<const std::string> &tags) {
    auto sections = string.split();
    auto& back = sections.back();
    bool has_extension = std::any_of(tags.begin(), tags.end(), [&back](const std::string& tag){
        return tag == back;
    });

    std::string camel_string;
    for(std::size_t i = 0; i < (has_extension ? sections.size() - 1 : sections.size()); ++i){
        camel_string += capital_to_camel(sections[i]);
    }
    if(has_extension){
        camel_string += back;
    }
    return camel_string;
}

vul::UpperCamelCaseStr
vul::UpperCamelCaseStr::from_validated(const vul::UpperSnakeCaseStr &string,
                                       const gsl::span<const std::string> &tags) noexcept(false) {
    auto camel_case = to_upper_camel_case(string, tags);
    if (!validate(camel_case, tags)) {
        throw std::runtime_error(
                fmt::format("converted string {} from UpperSnakeCaseStr {} is not UpperCamelCaseStr", camel_case, string));
    }
    return UpperCamelCaseStr(camel_case);
}

std::vector<std::string> vul::UpperCamelCaseStr::split_with_numbers() const {
    std::vector<std::string> sections;
    bool last_char_lower = false;
    bool last_char_digit = false;
    std::size_t section_start = 0;
    for (std::size_t i = 1; i < m_string.size(); ++i) {
        auto c = m_string[i];
        if (std::isupper(c)) {
            if (last_char_lower || last_char_digit) {
                std::size_t section_length = i - section_start;
                sections.push_back(
                        m_string.substr(section_start, section_length));
                section_start = i;
            }
            last_char_lower = false;
            last_char_digit = false;
        }else if(std::isdigit(c)){
            if (!last_char_digit) {
                std::size_t section_length = i - section_start;
                sections.push_back(
                        m_string.substr(section_start, section_length));
                section_start = i;
            }
            last_char_lower = false;
            last_char_digit = true;
        } else {
            last_char_lower = true;
            last_char_digit = false;
        }
    }
    //will catch extensions and anything else missed.
    std::size_t section_length = m_string.size() - section_start;
    sections.push_back(
            m_string.substr(section_start, section_length));
    return sections;
}

std::string::size_type vul::UpperCamelCaseStr::size() const {
    return m_string.size();
}

bool vul::UpperCamelCaseStr::empty() const {
    return m_string.empty();
}

std::size_t vul::UpperCamelCaseStr::element_count() const {
    return split().size();
}

std::size_t vul::UpperCamelCaseStr::element_count_with_numbers() const {
    return split_with_numbers().size();
}

vul::UpperCamelCaseStr
vul::UpperCamelCaseStr::strip_elements(std::size_t prefix_count,
                                       std::size_t suffix_count) const {
    auto sections = split();
    std::string new_string;
    for(std::size_t i = prefix_count; i < sections.size() - suffix_count; i++){
        new_string += sections.at(i);
    }
    return vul::UpperCamelCaseStr(new_string);
}

std::string vul::UpperCamelCaseStr::to_lower_camel_case() const {
    if(m_string.empty()){
        return "";
    }else{
        auto new_string = m_string;
        new_string[0] = static_cast<char>(std::tolower(new_string[0]));
        return new_string;
    }
}


vul::UpperCamelCaseStr vul::operator+(const vul::UpperCamelCaseStr &lhs,
                                      const vul::UpperCamelCaseStr &rhs) {
    auto temp = lhs;
    temp += rhs;
    return temp;
}

std::ostream &
vul::operator<<(std::ostream &os, const vul::UpperCamelCaseStr &str) {
    return os << str.string();
}

vul::UpperSnakeCaseStr::UpperSnakeCaseStr(const std::string_view &string)
        : m_string(string) {

}

vul::UpperSnakeCaseStr
vul::UpperSnakeCaseStr::from_validated(
        const std::string_view &string) noexcept(false) {
    if (!validate(string)) {
        throw std::runtime_error(
                fmt::format("string {} is not UpperSnakeCaseStr", string));
    }
    return UpperSnakeCaseStr(string);
}

bool vul::UpperSnakeCaseStr::validate(const std::string_view &string) {
    if (string.empty()) {
        return true;
    }
    return !std::any_of(string.begin(), string.end(), [](char c) {
        return std::islower(c);
    });
}

std::string vul::UpperSnakeCaseStr::string() const {
    return m_string;
}
std::string_view vul::UpperSnakeCaseStr::string_view() const {
    return m_string;
}




vul::UpperSnakeCaseStr &
vul::UpperSnakeCaseStr::operator+=(const vul::UpperSnakeCaseStr &rhs) {
    m_string += rhs.m_string;
    return *this;
}

std::vector<std::string> vul::UpperSnakeCaseStr::split() const {
    std::vector<std::string> sections;
    std::size_t section_start = 0;
    for (std::size_t i = 0; i < m_string.size(); ++i) {
        auto c = m_string[i];
        if (c == '_') {
            if (i > 0) {
                std::size_t section_length = i - section_start;
                sections.push_back(
                        m_string.substr(section_start, section_length));
            } else {
                sections.emplace_back("");
            }
            section_start = i+1;
        }
    }
    if(section_start < m_string.size()){
        std::size_t section_length = m_string.size() - section_start;
        sections.push_back(
                m_string.substr(section_start, section_length));
    }
    return sections;
}

std::string
vul::UpperSnakeCaseStr::camel_to_capital(const std::string_view &string) {
    std::string capital_string(string);
    for(auto& c : capital_string){
        c = static_cast<char>(std::toupper(c));
    }
    return capital_string;
}

std::string::size_type vul::UpperSnakeCaseStr::size() const {
    return m_string.size();
}

bool vul::UpperSnakeCaseStr::empty() const {
    return m_string.empty();
}

std::size_t vul::UpperSnakeCaseStr::element_count() const {
    return split().size();
}

vul::UpperSnakeCaseStr
vul::UpperSnakeCaseStr::strip_elements(std::size_t prefix_count,
                                       std::size_t suffix_count) const {
    auto sections = split();
    std::string new_string = sections.at(prefix_count);
    for(std::size_t i = prefix_count+1; i < sections.size() - suffix_count; i++){
        new_string += "_"+sections.at(i);
    }
    return vul::UpperSnakeCaseStr(new_string);
}

std::string vul::UpperSnakeCaseStr::get_tag() const {
    for(std::size_t i = 0; i < m_string.size(); ++i){
        auto ri = m_string.size() - 1 - i;
        if(m_string[ri] == '_'){
            return m_string.substr(ri+1, std::string::npos);
        }
    }
    return "";
}


vul::UpperSnakeCaseStr
vul::operator+(const UpperSnakeCaseStr &lhs, const UpperSnakeCaseStr &rhs) {
    auto temp = lhs;
    temp += rhs;
    return temp;
}

std::ostream &
vul::operator<<(std::ostream &os, const UpperSnakeCaseStr &str) {
    return os << str.string();
}