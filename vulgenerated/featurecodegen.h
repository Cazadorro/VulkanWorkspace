//
// Created by Shae Bolt on 5/26/2021.
//

#ifndef VULKANPROJECT_FEATURECODEGEN_H
#define VULKANPROJECT_FEATURECODEGEN_H

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
namespace pugi {
    class xml_node;
}
namespace vul {
    struct CodeGen;
    struct FeatureMemberInfo {
        std::string type;
        std::string type_name;
    };

    class FeatureStructInfo {
    public:
        FeatureStructInfo() = default;

        explicit FeatureStructInfo(const pugi::xml_node &node);

        [[nodiscard]]
        std::string get_name() const;

        [[nodiscard]]
        std::string get_structure_value() const;

        [[nodiscard]]
        const std::vector<FeatureMemberInfo> &get_members() const;

    private:
        std::string m_name;
        std::string m_structure_value;
        std::vector<FeatureMemberInfo> m_members;
    };

    [[nodiscard]]
    CodeGen generate_feature_struct_code(
            const std::vector<FeatureStructInfo> &feature_infos,
            const FeatureStructInfo &physical_device_features_info,
            const std::string_view &indent,
            const std::string_view &namespace_str,
            const std::unordered_map<std::string, std::string>& type_platform_define_map);

    [[nodiscard]]
    std::string generate_feature_struct_forward(const std::vector<FeatureStructInfo> &feature_infos);
}
#endif //VULKANPROJECT_FEATURECODEGEN_H
