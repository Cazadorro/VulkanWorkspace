//
// Created by Shae Bolt on 5/26/2021.
//

#ifndef VULKANPROJECT_VKNAMEUTILS_H
#define VULKANPROJECT_VKNAMEUTILS_H
#include <gsl/span>
#include <string>
#include <string_view>
namespace vul{
    class UpperCamelCaseStr;
    class UpperSnakeCaseStr;
    class VkEnumNameInfo{
    public:
        VkEnumNameInfo() = default;
        explicit VkEnumNameInfo(const vul::UpperCamelCaseStr& enum_name, const gsl::span<const std::string>& tags = {});
        [[nodiscard]]
        std::string get_prefix()const;
        [[nodiscard]]
        std::string get_tag()const;
        [[nodiscard]]
        std::string get_number()const;

        [[nodiscard]]
        vul::UpperCamelCaseStr extract_raw_name(const vul::UpperCamelCaseStr& enum_name)const;
        [[nodiscard]]
        vul::UpperCamelCaseStr extract_name(const vul::UpperCamelCaseStr& enum_name)const;
        [[nodiscard]]
        std::size_t prefix_count() const;
        [[nodiscard]]
        std::size_t suffix_count() const;
    private:
        std::string m_prefix;
        std::string m_tag;
        std::string m_number;
    };
    [[nodiscard]]
    std::string vk_value_name(const std::string_view& string);
}
#endif //VULKANPROJECT_VKNAMEUTILS_H
