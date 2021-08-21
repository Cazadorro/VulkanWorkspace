//
// Created by Shae Bolt on 5/24/2021.
//

#ifndef VULKANPROJECT_CODEGENUTILS_H
#define VULKANPROJECT_CODEGENUTILS_H

#include <gsl/span>
#include <string>
#include <string_view>
#include <functional>
#include <optional>


namespace pugi{
    class xml_node;
}
namespace vul {
    [[nodiscard]]
    std::string add_to_each_line(const std::string_view &string,
                                 const std::string_view &prepend,
                                 const std::string_view &append = {});

    [[nodiscard]]
    std::string format_if_statement(const std::string_view &indent,
                                    const std::string_view &condition,
                                    const std::string_view &body,
                                    const std::string_view &pre_string = "",
                                    const std::string_view &post_string = "");

    [[nodiscard]]
    std::string format_else_if_statement(const std::string_view &indent,
                                         const std::string_view &condition,
                                         const std::string_view &body,
                                         const std::string_view &pre_string = "",
                                         const std::string_view &post_string = "");

    [[nodiscard]]
    std::string format_case_statement(const std::string_view &indent,
                                      const std::string_view &condition,
                                      const std::string_view &body,
                                      const std::string_view &pre_string = "",
                                      const std::string_view &post_string = "");
    [[nodiscard]]
    std::string format_else_statement(const std::string_view &indent,
                                     const std::string_view &body,
                                     const std::string_view &pre_string = "",
                                     const std::string_view &post_string = "");
    [[nodiscard]]
    std::string format_default_case_statement(const std::string_view &indent,
                                              const std::string_view &body);

    struct CodeGen {
        std::string header;
        std::string source;
    };



    [[nodiscard]]
    std::string start_define(const std::string_view &string);
    [[nodiscard]]
    std::string else_define();

    [[nodiscard]]
    std::string end_define(const std::string_view &string);

    std::vector<std::string> split(const std::string_view& string, char delimiter);

    template<typename T>
    [[nodiscard]]
    bool contains(const gsl::span<const T>& array, const T& value){
        auto itr = std::find(array.begin(), array.end(), value);
        return itr != array.end();
    }
    template<typename T>
    [[nodiscard]]
    bool contains(const gsl::span<T>& array, const T& value){
        auto itr = std::find(array.begin(), array.end(), value);
        return itr != array.end();
    }

    [[nodiscard]]
    bool structextends(const pugi::xml_node& node, const std::string_view& string);
    [[nodiscard]]
    bool is_alias(const pugi::xml_node& node);

    std::string get_name(const pugi::xml_node& node);
    std::string get_text(const pugi::xml_node& node, const std::string_view& child_string);


    std::string start_include_guard(const std::string_view& string_view);
    std::string end_include_guard(const std::string_view& string_view);
}
#endif //VULKANPROJECT_CODEGENUTILS_H
