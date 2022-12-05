//
// Created by Shae Bolt on 5/24/2021.
//

#include "enumcodegen.h"
#include <fmt/ostream.h>
#include <iostream>

vul::CodeGen vul::generate_to_string_vk_enum(const vul::EnumInfo &info,
                                             const std::string_view &namespace_str,
                                             const std::string_view &indent) {
    std::string declaration = fmt::format(
            "{2}std::string to_vk_string({0}::{1} enum_value);\n", namespace_str, info.name,
            indent);
    std::string vk_declaration = fmt::format(
            "{2}std::string to_string({1} enum_value);\n", namespace_str, info.vk_name,
            indent);
    std::string definition = fmt::format(
            "std::string {0}::to_vk_string({0}::{1} enum_value){{\n", namespace_str,
            info.name);
    definition += fmt::format("{0}switch(get(enum_value)){{\n", indent);

    std::string vk_definition = fmt::format(
            "std::string {0}::to_string({1} enum_value){{\n", namespace_str,
            info.vk_name);
    vk_definition += fmt::format("{0}switch((enum_value)){{\n", indent);
    for(const auto& enum_value_info: info.get_enum_values()){
        std::string case_string;
        if(enum_value_info.alias_string.empty()){
            std::string body = fmt::format("return \"{1}\";\n",indent, enum_value_info.vk_name.string());
            std::string conditional = enum_value_info.vk_name.string();
            if(enum_value_info.define_string.empty()){
                case_string += format_case_statement(indent, conditional, body);
            }else{
                std::string pre_define = fmt::format("\n#if defined( {} )\n", enum_value_info.define_string);
                std::string post_define = fmt::format("\n#endif /*{}*/\n", enum_value_info.define_string);
                case_string += format_case_statement(indent, conditional, body, pre_define, post_define);
            }
        }
        //TODO maybe not apply this to every line?
        definition += add_to_each_line(case_string, fmt::format("{0}{0}", indent));
        vk_definition += add_to_each_line(case_string, fmt::format("{0}{0}", indent));
    }
    definition += add_to_each_line(format_default_case_statement(indent, "return \"\";\n"), fmt::format("{0}{0}", indent));
    definition += fmt::format("{0}}}\n", indent);
    definition += fmt::format("}}\n");

    vk_definition += add_to_each_line(format_default_case_statement(indent, "return \"\";\n"), fmt::format("{0}{0}", indent));
    vk_definition += fmt::format("{0}}}\n", indent);
    vk_definition += fmt::format("}}\n");
    return {declaration, definition};
//    return {declaration + vk_declaration, definition + vk_definition};
}

vul::CodeGen vul::generate_to_string_enum_class(const vul::EnumInfo &info,
                                                const std::string_view &namespace_str,
                                                const std::string_view &indent) {
    std::string declaration = fmt::format(
            "{2}std::string to_string({0}::{1} enum_value);\n", namespace_str,
            info.name, indent);
    std::string definition = fmt::format(
            "std::string {0}::to_string({0}::{1} enum_value){{\n",
            namespace_str, info.name);
    definition += fmt::format("{0}switch(enum_value){{\n", indent);
    for(const auto& enum_value_info: info.get_enum_values()){
        std::string case_string;
        if(enum_value_info.alias_string.empty()){
            std::string body = fmt::format("return \"{1}\";\n",indent, enum_value_info.name);
            std::string conditional = fmt::format("{}::{}", info.name, enum_value_info.name);
            if(enum_value_info.define_string.empty()){
                case_string += format_case_statement(indent, conditional, body);
            }else{
                std::string pre_define = fmt::format("\n#if defined( {} )\n", enum_value_info.define_string);
                std::string post_define = fmt::format("\n#endif /*{}*/\n", enum_value_info.define_string);
                case_string += format_case_statement(indent, conditional, body, pre_define, post_define);
            }
        }
        //TODO maybe not apply this to every line?
        definition += add_to_each_line(case_string, fmt::format("{0}{0}", indent));
    }
    definition += add_to_each_line(format_default_case_statement(indent, "return \"\";\n"), fmt::format("{0}{0}", indent));
    definition += fmt::format("{0}}}\n", indent);
    definition += fmt::format("}}\n");
    return {declaration, definition};
}

vul::CodeGen vul::generate_from_string_vk_enum(const vul::EnumInfo &info,
                                               const std::string_view &namespace_str,
                                               const std::string_view &indent) {
    std::string declaration = fmt::format(
            "{1}std::optional<{0}> to_{0}(const std::string_view& enum_string);\n",
            info.vk_name, indent);
    std::string definition = fmt::format(
            "std::optional<{1}> {0}::to_{1}(const std::string_view& enum_string){{\n",
            namespace_str, info.vk_name);

    bool first_added = false;
    for(const auto& enum_value_info: info.get_enum_values()){
        std::string case_string;
        if(enum_value_info.alias_string.empty()){
            std::string body = fmt::format("return {1};\n",indent, enum_value_info.vk_name);
            std::string conditional = fmt::format("enum_string == \"{0}\"", enum_value_info.vk_name);
            if(enum_value_info.define_string.empty()){
                if(!first_added) {
                    case_string += format_if_statement(indent, conditional,
                                                       body);
                    first_added = true;
                }else{
//                    had to change this because MSVC can only support 128 max else ifs....
//                    case_string += format_else_if_statement(indent, conditional, body);
                    case_string += format_if_statement(indent, conditional,
                                                       body);
                }
            }else{
                std::string pre_define = fmt::format("\n#if defined( {} )\n", enum_value_info.define_string);
                std::string post_define = fmt::format("\n#endif /*{}*/\n", enum_value_info.define_string);
                if(!first_added) {
                    case_string += format_if_statement(indent, conditional,
                                                       body, pre_define,
                                                       post_define);
                    first_added = true;
                }else{
                    //                    had to change this because MSVC can only support 128 max else ifs....
//                    case_string += format_else_if_statement(indent, conditional, body, pre_define, post_define);
                    case_string += format_if_statement(indent, conditional,
                                                       body, pre_define,
                                                       post_define);
                }
            }
        }
        //TODO maybe not apply this to every line?
        definition += add_to_each_line(case_string, fmt::format("{0}", indent));
    }
    definition += add_to_each_line(format_else_statement(indent, "return std::nullopt;\n"), fmt::format("{0}", indent));
    definition += fmt::format("}}\n");
    return {declaration, definition};
}

vul::CodeGen vul::generate_from_string_enum_class(const vul::EnumInfo &info,
                                                  const std::string_view &namespace_str,
                                                  const std::string_view &indent)  {
    std::string declaration = fmt::format(
            "{2}std::optional<{0}::{1}> to_{1}(const std::string_view& enum_string);\n",
            namespace_str, info.name, indent);
    std::string definition = fmt::format(
            "std::optional<{0}::{1}> {0}::to_{1}(const std::string_view& enum_string){{\n",
            namespace_str, info.name);

    bool first_added = false;
    for(const auto& enum_value_info: info.get_enum_values()){
        std::string case_string;
        if(enum_value_info.alias_string.empty()){
            std::string body = fmt::format("return {2}::{1};\n",indent, enum_value_info.name, info.name);
            std::string conditional = fmt::format("enum_string == \"{0}\"", enum_value_info.name);
            if(enum_value_info.define_string.empty()){
                if(!first_added) {
                    case_string += format_if_statement(indent, conditional,
                                                       body);
                    first_added = true;
                }else{
//                    had to change this because MSVC can only support 128 max else ifs....
//                    case_string += format_else_if_statement(indent, conditional, body);
                    case_string += format_if_statement(indent, conditional,
                                                       body);
                }
            }else{
                std::string pre_define = fmt::format("\n#if defined( {} )\n", enum_value_info.define_string);
                std::string post_define = fmt::format("\n#endif /*{}*/\n", enum_value_info.define_string);
                if(!first_added) {
                    case_string += format_if_statement(indent, conditional,
                                                       body, pre_define,
                                                       post_define);
                    first_added = true;
                }else{
//                    had to change this because MSVC can only support 128 max else ifs....
//                    case_string += format_else_if_statement(indent, conditional, body, pre_define, post_define);
                    case_string += format_if_statement(indent, conditional,
                                                       body, pre_define,
                                                       post_define);
                }
            }
        }
        //TODO maybe not apply this to every line?
        definition += add_to_each_line(case_string, fmt::format("{0}", indent));
    }
    definition += add_to_each_line(format_else_statement(indent, "return std::nullopt;\n"), fmt::format("{0}", indent));
    definition += fmt::format("}}\n");

    return {declaration, definition};
}

vul::CodeGen vul::generate_enum_get(const vul::EnumInfo &info,
                                    const std::string_view &namespace_str,
                                    const std::string_view &indent) {
    std::string declaration = fmt::format("{3}{2} get({0}::{1} enum_value);\n",
                                          namespace_str, info.name,
                                          info.vk_name, indent);
    std::string definition = fmt::format("{2} {0}::get({0}::{1} enum_value){{\n"
                                         "{3}return static_cast<{2}>(enum_value);\n"
                                         "}}\n", namespace_str, info.name,
                                         info.vk_name, indent);
    return {declaration, definition};
}

std::string vul::generate_enum_class(const vul::EnumInfo &info,
                                     const std::string_view &indent) {
    std::string underlying_string;
    if(!info.underlying_type.empty()){
        underlying_string = fmt::format(" : {} ",info.underlying_type);
    }

    std::string declaration = fmt::format("{}enum class {}{}{{\n", indent,
                                          info.name, underlying_string);
    for (const auto& enum_value_info: info.get_enum_values()) {
        if(!enum_value_info.define_string.empty()){
            declaration += start_define(enum_value_info.define_string);
        }
        declaration += fmt::format("{0}{0}{1} = {2},\n", indent, enum_value_info.name, enum_value_info.vk_name);
        if(!enum_value_info.define_string.empty()){
            declaration += end_define(enum_value_info.define_string);
        }
    }
    declaration += fmt::format("{}}};\n", indent);
    return declaration;
}

std::string vul::generate_forward_declaration(const vul::EnumInfo &info,
                                              const std::string_view &indent) {
    std::string underlying_string;
    if(!info.underlying_type.empty()){
        //TODO use the true (std::uint32_t or std::uint64_t) underlying type to further reduce compilation.
        underlying_string = fmt::format(" : {} ",info.underlying_type);
    }
    std::string declaration = fmt::format("{} enum class {}{};\n", indent,
                                          info.name, underlying_string);
    return declaration;
}

vul::CodeGen vul::generate_bitmask_wrapper(const vul::EnumInfo &info,
                                           const std::string_view &namespace_str,
                                           const std::string_view &indent) {
    std::string bitmask_name = std::string(info.name.strip_elements(0, 2).string());
    if(!(info.vk_name_info.get_suffix_number().empty())){
        bitmask_name += info.vk_name_info.get_suffix_number();
    }
    std::string declaration = fmt::format("{2}class {0}BitMask {{\n"
                "{2}public:\n"
                "{2}{2}using UnderlyingType = std::underlying_type_t<{1}>;\n"
                "\n"
                "{2}{2}{0}BitMask();\n"
                "\n"
                "{2}{2}{0}BitMask({1} bit);\n"
                "\n"
                "{2}{2}explicit {0}BitMask(UnderlyingType flags);\n"
                "\n"
                "{2}{2}[[nodiscard]]\n"
                "{2}{2}bool contains({0}BitMask rhs) const;\n"
                "\n"
                "{2}{2}explicit operator UnderlyingType() const;\n"
                "\n"
                "{2}{2}[[nodiscard]]\n"
                "{2}{2}UnderlyingType to_underlying() const;\n"
                "\n"
                "{2}{2}[[nodiscard]]\n"
                "{2}{2}static UnderlyingType to_underlying({1} rhs);\n"
                "\n"
                "{2}{2}[[nodiscard]]\n"
                "{2}{2}static {0}BitMask full();\n"
                "\n"
                "{2}{2}{0}BitMask &operator=({1} rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator&=({0}BitMask rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator&=({1} rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator|=({0}BitMask rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator|=({1} rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator^=({0}BitMask rhs);\n"
                "\n"
                "{2}{2}{0}BitMask &operator^=({1} rhs);\n"
                "\n"
                "{2}{2}{0}BitMask & clear({1} bit);\n"
                "\n"
                "{2}{2}{0}BitMask & set({1} bit);\n"
                "\n"
                "{2}{2}{0}BitMask & set({1} bit, bool state);\n"
                "\n"
                "{2}{2}[[nodiscard]]\n"
                "{2}{2}bool get({1} bit) const;\n"
                "{2}{2}friend {0}BitMask operator~({0}BitMask other);\n"
                "{2}private:\n"
                "{2}{2}UnderlyingType m_bit_mask;\n"
                "{2}}};\n"
                "\n"
                "{2}bool operator==({0}BitMask lhs, {0}BitMask rhs);\n"
                "\n"
                "{2}bool operator==({0}BitMask lhs, {1} rhs);\n"
                "\n"
                "{2}bool operator!=({0}BitMask lhs, {0}BitMask rhs);\n"
                "\n"
                "{2}bool operator!=({0}BitMask lhs, {1} rhs);\n"
                "\n"
                "{2}{0}BitMask operator&({0}BitMask lhs, {0}BitMask rhs);\n"
                "\n"
                "{2}{0}BitMask operator&({0}BitMask lhs, {1} rhs);\n"
                "\n"
                "{2}{0}BitMask operator|({0}BitMask lhs, {0}BitMask rhs);\n"
                "\n"
                "{2}{0}BitMask operator|({0}BitMask lhs, {1} rhs);\n"
                "\n"
                "{2}{0}BitMask operator^({0}BitMask lhs, {0}BitMask rhs);\n"
                "\n"
                "{2}{0}BitMask operator^({0}BitMask lhs, {1} rhs);\n"
                "\n"
                "{2}{0}BitMask operator~({0}BitMask other);\n"
                "\n"
                "{2}{0}BitMask operator~({1} rhs);\n", bitmask_name, info.name, indent);
    std::string definition = fmt::format("{3}::{0}BitMask::{0}BitMask() : m_bit_mask(0) {{\n"
                                         "\n"
                                         "}}\n"
                                         "{3}::{0}BitMask::{0}BitMask({1} bit) : m_bit_mask(\n"
                                         "{2}{2}static_cast<UnderlyingType>(bit)) {{\n"
                                         "\n"
                                         "}}\n"
                                         "{3}::{0}BitMask::{0}BitMask(UnderlyingType flags): m_bit_mask(flags){{\n"
                                         "}}\n"
                                         "bool {3}::{0}BitMask::contains({0}BitMask rhs) const{{\n"
                                         "{2}return (m_bit_mask & rhs.m_bit_mask) == rhs.m_bit_mask;\n"
                                         "}}\n"
                                         "{3}::{0}BitMask::operator {3}::{0}BitMask::UnderlyingType() const {{\n"
                                         "{2}return m_bit_mask;\n"
                                         "}}\n"
                                         "{3}::{0}BitMask::UnderlyingType {3}::{0}BitMask::to_underlying() const {{\n"
                                         "{2}return m_bit_mask;\n"
                                         "}}\n"
                                         "{3}::{0}BitMask::UnderlyingType {3}::{0}BitMask::to_underlying({1} rhs) {{\n"
                                         "{2}return static_cast<UnderlyingType>(rhs);\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::{0}BitMask::full() {{\n"
                                         "{2}{0}BitMask mask;\n"
                                         "{2}mask.m_bit_mask = ~mask.m_bit_mask;\n"
                                         "{2}return mask;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask& {3}::{0}BitMask::operator=({1} rhs) {{\n"
                                         "{2}m_bit_mask = to_underlying(rhs);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator&=({0}BitMask rhs) {{\n"
                                         "{2}m_bit_mask &= rhs.m_bit_mask;\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator&=({1} rhs) {{\n"
                                         "{2}m_bit_mask &= to_underlying(rhs);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator|=({0}BitMask rhs) {{\n"
                                         "{2}m_bit_mask |= rhs.m_bit_mask;\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator|=({1} rhs) {{\n"
                                         "{2}m_bit_mask |= to_underlying(rhs);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator^=({0}BitMask rhs) {{\n"
                                         "{2}m_bit_mask ^= rhs.m_bit_mask;\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask &{3}::{0}BitMask::operator^=({1} rhs) {{\n"
                                         "{2}m_bit_mask ^= to_underlying(rhs);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask & {3}::{0}BitMask::clear({1} bit) {{\n"
                                         "{2}m_bit_mask &= ~to_underlying(bit);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "{3}::{0}BitMask & {3}::{0}BitMask::set({1} bit) {{\n"
                                         "{2}m_bit_mask |= to_underlying(bit);\n"
                                         "{2}return *this;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask & {3}::{0}BitMask::set({1} bit, bool state) {{\n"
                                         "{2}if (state) {{\n"
                                         "{2}{2}return set(bit);\n"
                                         "{2}}} else {{\n"
                                         "{2}{2}return clear(bit);\n"
                                         "{2}}}\n"
                                         "}}\n"
                                         "\n"
                                         "bool {3}::{0}BitMask::get({1} bit) const {{\n"
                                         "{2}return (m_bit_mask & to_underlying(bit)) != 0;\n"
                                         "}}\n"
                                         "\n"
                                         "\n"
                                         "bool {3}::operator==({0}BitMask lhs, {0}BitMask rhs) {{\n"
                                         "{2}return lhs.to_underlying() == rhs.to_underlying();\n"
                                         "}}\n"
                                         "\n"
                                         "bool {3}::operator==({0}BitMask lhs, {1} rhs) {{\n"
                                         "{2}return lhs.to_underlying() == {0}BitMask::to_underlying(rhs);\n"
                                         "}}\n"
                                         "\n"
                                         "bool {3}::operator!=({0}BitMask lhs, {0}BitMask rhs) {{\n"
                                         "{2}return lhs.to_underlying() != rhs.to_underlying();\n"
                                         "}}\n"
                                         "\n"
                                         "bool {3}::operator!=({0}BitMask lhs, {1} rhs) {{\n"
                                         "{2}return lhs.to_underlying() != {0}BitMask::to_underlying(rhs);\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator&({0}BitMask lhs, {0}BitMask rhs) {{\n"
                                         "{2}lhs &= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator&({0}BitMask lhs, {1} rhs) {{\n"
                                         "{2}lhs &= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator|({0}BitMask lhs, {0}BitMask rhs) {{\n"
                                         "{2}lhs |= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator|({0}BitMask lhs, {1} rhs) {{\n"
                                         "{2}lhs |= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator^({0}BitMask lhs, {0}BitMask rhs) {{\n"
                                         "{2}lhs ^= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator^({0}BitMask lhs, {1} rhs) {{\n"
                                         "{2}lhs ^= rhs;\n"
                                         "{2}return lhs;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator~({0}BitMask other) {{\n"
                                         "{2}other.m_bit_mask = ~other.m_bit_mask;\n"
                                         "{2}return other;\n"
                                         "}}\n"
                                         "\n"
                                         "{3}::{0}BitMask {3}::operator~({1} rhs) {{\n"
                                         "{2}return ~{0}BitMask(rhs);\n"
                                         "}}\n", bitmask_name, info.name, indent, namespace_str);
    return {declaration, definition};
}

std::string vul::generate_bitmask_forward(const vul::EnumInfo &info,
                                          const std::string_view &indent) {
    std::string bitmask_name = std::string(info.name.strip_elements(0, 2).string());
    if(!(info.vk_name_info.get_suffix_number().empty())){
        bitmask_name += info.vk_name_info.get_suffix_number();
    }
    return fmt::format("{1}class {0}BitMask;\n", bitmask_name, indent);
}
