//
// Created by Shae Bolt on 5/24/2021.
//

#include "codegenutils.h"
#include <pugixml.hpp>
#include <fmt/core.h>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/range/conversion.hpp>
#include <sstream>
#include <iostream>

std::string vul::add_to_each_line(const std::string_view &string,
                                  const std::string_view &prepend,
                                  const std::string_view &append) {
    std::istringstream generated_case((std::string(string)));
    std::string line;
    std::string return_string;
    while (std::getline(generated_case, line)) {
        return_string += fmt::format("{}{}{}\n", prepend,
                                     line, append);
    }
    return return_string;
}

std::string vul::start_define(const std::string_view &string) {
    return fmt::format("#if defined( {} )\n", string);
}

std::string vul::end_define(const std::string_view &string) {
    return fmt::format("#endif /*{}*/\n", string);
}

std::string vul::format_if_statement(const std::string_view &indent,
                                     const std::string_view &condition,
                                     const std::string_view &body,
                                     const std::string_view &pre_string,
                                     const std::string_view &post_string) {
    return fmt::format("{3}"
                       "if({1}){{\n"
                       "{2}"
                       "}}"
                       "{4}",
                       indent, condition, add_to_each_line(body, indent),
                       pre_string, post_string);
}

std::string
vul::format_else_if_statement(const std::string_view &indent,
                              const std::string_view &condition,
                              const std::string_view &body,
                              const std::string_view &pre_string,
                              const std::string_view &post_string) {

    return fmt::format("{3}"
                       "else if({1}){{\n"
                       "{2}"
                       "}}"
                       "{4}",
                       indent, condition, add_to_each_line(body, indent),
                       pre_string, post_string);
}

std::string vul::format_else_statement(const std::string_view &indent,
                                       const std::string_view &body,
                                       const std::string_view &pre_string,
                                       const std::string_view &post_string) {
    return fmt::format("{2}"
                       "else{{\n"
                       "{1}"
                       "}}\n"
                       "{3}",
                       indent, add_to_each_line(body, indent),
                       pre_string, post_string);
}


std::string
vul::format_case_statement(const std::string_view &indent,
                           const std::string_view &condition,
                           const std::string_view &body,
                           const std::string_view &pre_string,
                           const std::string_view &post_string) {
    return fmt::format("{3}"
                       "case {1}: {{\n"
                       "{2}"
                       "}}\n"
                       "{4}",
                       indent, condition, add_to_each_line(body, indent),
                       pre_string, post_string);
}

std::string vul::format_default_case_statement(const std::string_view &indent,
                                               const std::string_view &body) {
    return fmt::format("default: {{\n"
                       "{0}"
                       "}}\n",
                       add_to_each_line(body, indent));
}

std::vector<std::string>
vul::split(const std::string_view &string, char delimiter) {
    return string | ranges::views::split(delimiter) | ranges::to<std::vector<std::string>>();
}

bool vul::structextends(const pugi::xml_node &node, const std::string_view& string) {
    auto structextends = vul::split(node.attribute("structextends").as_string(), ',');
    return vul::contains(gsl::span(structextends), std::string(string));
}

bool vul::is_alias(const pugi::xml_node &node) {
    return !(std::string(node.attribute("alias").as_string()).empty());
}

std::string vul::get_name(const pugi::xml_node &node) {
    return node.attribute("name").as_string();
}

std::string vul::get_text(const pugi::xml_node &node,
                                const std::string_view &child_string) {
    return node.child(std::string(child_string).c_str()).text().as_string();
}

std::string vul::else_define() {
    return fmt::format("#else\n");
}

std::string vul::start_include_guard(const std::string_view &string_view) {
    return fmt::format("#ifndef {0}\n"
                       "#define {0}\n",
    string_view);
}

std::string vul::end_include_guard(const std::string_view &string_view) {
    return fmt::format("#endif //{0}\n",
                       string_view);
}
