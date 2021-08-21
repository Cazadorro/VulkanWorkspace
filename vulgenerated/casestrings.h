//
// Created by Shae Bolt on 5/20/2021.
//

#ifndef VULKANPROJECT_CASESTRINGS_H
#define VULKANPROJECT_CASESTRINGS_H
#include <gsl/span>
#include <string>
#include <string_view>
#include <ostream>
#include <vector>
#include "vknameutils.h"


namespace vul{

    class UpperSnakeCaseStr;
    [[nodiscard]]
    bool end_matches(const std::string_view& lhs, const std::string_view& rhs);
    class UpperCamelCaseStr{
    public:

        UpperCamelCaseStr() = default;
        explicit UpperCamelCaseStr(const std::string_view& string);
        explicit UpperCamelCaseStr(const UpperSnakeCaseStr& string, const gsl::span<const std::string>& tags = {});
        [[nodiscard]]
        static UpperCamelCaseStr from_validated(const std::string_view& string, const gsl::span<const std::string>& tags = {}) noexcept(false);

        [[nodiscard]]
        static UpperCamelCaseStr from_validated(const UpperSnakeCaseStr& string, const gsl::span<const std::string>& tags = {}) noexcept(false);

        [[nodiscard]]
        static bool validate(const std::string_view& string, const gsl::span<const std::string>& tags = {});
        [[nodiscard]]
        std::string string() const;
        [[nodiscard]]
        std::string to_lower_camel_case() const;
        [[nodiscard]]
        std::string_view string_view() const;
        UpperCamelCaseStr& operator+=(const UpperCamelCaseStr& rhs);
        //split along capital letters
        [[nodiscard]]
        std::vector<std::string> split() const;
        [[nodiscard]]
        std::vector<std::string> split_with_numbers() const;

        //counts number of things that can be split
        [[nodiscard]]
        std::size_t element_count() const;
        [[nodiscard]]
        std::size_t element_count_with_numbers() const;

        [[nodiscard]]
        UpperCamelCaseStr strip_elements(std::size_t prefix_count, std::size_t suffix_count = 0) const;

        [[nodiscard]]
        static std::string capital_to_camel(const std::string_view& string);
        [[nodiscard]]
        static std::string to_upper_camel_case(const UpperSnakeCaseStr& string, const gsl::span<const std::string>& tags = {});

        [[nodiscard]]
        std::string::size_type size() const;
        [[nodiscard]]
        bool empty() const;
    private:
        std::string m_string;
    };
    [[nodiscard]]
    UpperCamelCaseStr operator+(const UpperCamelCaseStr& lhs, const UpperCamelCaseStr& rhs);
    std::ostream& operator<< (std::ostream& os, const UpperCamelCaseStr& str);

    class UpperSnakeCaseStr{
    public:
        UpperSnakeCaseStr() = default;
        explicit UpperSnakeCaseStr(const std::string_view& string);
        [[nodiscard]]
        static UpperSnakeCaseStr from_validated(const std::string_view& string) noexcept(false);
        [[nodiscard]]
        static bool validate(const std::string_view& string);
        [[nodiscard]]
        std::string string() const;
        [[nodiscard]]
        std::string_view string_view() const;
        UpperSnakeCaseStr& operator+=(const UpperSnakeCaseStr& rhs);
        //split along '_'
        [[nodiscard]]
        std::vector<std::string> split() const;
        [[nodiscard]]
        std::size_t element_count() const;

        [[nodiscard]]
        UpperSnakeCaseStr strip_elements(std::size_t prefix_count, std::size_t suffix_count = 0) const;
        [[nodiscard]]
        static std::string camel_to_capital(const std::string_view& string);
        [[nodiscard]]
        std::string::size_type size() const;
        [[nodiscard]]
        bool empty() const;
        [[nodiscard]]
        std::string get_tag() const;
    private:
        std::string m_string;
    };


    [[nodiscard]]
    UpperSnakeCaseStr operator+(const UpperSnakeCaseStr& lhs, const UpperSnakeCaseStr& rhs);
    std::ostream& operator<< (std::ostream& os, const UpperSnakeCaseStr& str);
}
#endif //VULKANPROJECT_CASESTRINGS_H
