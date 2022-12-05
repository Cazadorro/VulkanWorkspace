//
// Created by Shae Bolt on 5/17/2021.
//
#include "enumcodegen.h"
#include "featurecodegen.h"
#include <pugixml.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

struct ObjtypeEnumValue {
    std::string objtypeenum;
    std::string platform;
};

int main() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file((std::string(VUL_GEN_RESOURCE_PATH) + "/resources/vk.xml").c_str());
    if (!result)
        return -1;
    auto root = doc.root().first_child(); //get attributes first.
//    for(auto temp : root.children()){
//        fmt::print("{}\n", temp.name());
//    }
    pugi::xpath_node_set ns = root.select_nodes("enums");
    std::unordered_map<std::string, vul::EnumInfo> enum_infos;
    std::vector<std::string> tags;
    for (auto tag : root.child("tags").children()) {
        std::string name = tag.attribute("name").as_string();
        tags.push_back(name);
    }
    for (const auto &tag : tags) {
        fmt::print("tag {}\n", tag);
    }
    std::unordered_map<std::string, std::string> platform_define_map;
    std::unordered_map<std::string, std::string> enum_platform_define_map;
    std::unordered_map<std::string, std::string> type_platform_define_map;
    std::unordered_map<std::string, std::string> command_platform_define_map;

    for (pugi::xml_node platform: root.child("platforms").children(
            "platform")) {
        auto name = platform.attribute("name").as_string();
        auto define = platform.attribute("protect").as_string();
        platform_define_map[name] = define;
    }

    for (pugi::xml_node extensions: root.children("extensions")) {
        for (pugi::xml_node extension: extensions.children("extension")) {
            auto platform_name = extension.attribute("platform").as_string();
            if (std::string(platform_name).empty()) {
                continue;
            }
            for (pugi::xml_node require: extension.children("require")) {
                for (pugi::xml_node member: require.children("enum")) {
                    auto member_name = member.attribute("name").as_string();
                    enum_platform_define_map[member_name] = platform_define_map[platform_name];
                }
                for (pugi::xml_node member: require.children("type")) {
                    auto member_name = member.attribute("name").as_string();
                    type_platform_define_map[member_name] = platform_define_map[platform_name];
                }
                for (pugi::xml_node member: require.children("command")) {
                    auto member_name = member.attribute("name").as_string();
                    command_platform_define_map[member_name] = platform_define_map[platform_name];
                }
            }
        }
    }

//    <type requires="VkFramebufferCreateFlagBits"      category="bitmask">typedef <type>VkFlags</type> <name>VkFramebufferCreateFlags</name>;</type>
    std::unordered_map<std::string, std::string> enum_underlying_type_map;
    std::vector<vul::FeatureStructInfo> feature_struct_infos;
    vul::FeatureStructInfo physical_device_feature_struct_info;
    std::unordered_map<std::string, ObjtypeEnumValue> objtypeenum_map;

    std::vector<std::string> accepted_feature_types = {
            "VkPhysicalDeviceVulkan11Features",
            "VkPhysicalDeviceVulkan12Features",
            "VkPhysicalDeviceVulkan13Features",
            "VkPhysicalDeviceShaderAtomicFloatFeaturesEXT",
            "VkPhysicalDeviceSynchronization2FeaturesKHR"
    };
    for (pugi::xml_node type_node: root.child("types").children("type")) {
        std::string objtypeenum = type_node.attribute(
                "objtypeenum").as_string();
        if (!objtypeenum.empty()) {
            std::string type_name = vul::get_text(type_node, "name");
            std::string platform;
            if (auto platform_itr = type_platform_define_map.find(type_name);
                    platform_itr != type_platform_define_map.end()) {
                platform = platform_itr->second;
            }
            objtypeenum_map[type_name] = {objtypeenum, platform};
        }
        if (type_node.attribute("category").as_string() ==
            std::string("bitmask")) {
            std::string requires_string = type_node.attribute(
                    "requires").as_string();
            if (requires_string.empty()) {
                requires_string = type_node.attribute("bitvalues").as_string();
            }
            if (!requires_string.empty()) {
                enum_underlying_type_map[requires_string] = type_node.child(
                        "name").text().as_string();
            }
        }
        if (vul::structextends(type_node, "VkPhysicalDeviceFeatures2")) {
            if (!vul::is_alias(type_node)) {
                if(std::find(accepted_feature_types.begin(), accepted_feature_types.end(), vul::get_name(type_node)) != accepted_feature_types.end()){
                    feature_struct_infos.emplace_back(type_node);
                }
            }
        }
        if (vul::get_name(type_node) ==
            std::string("VkPhysicalDeviceFeatures")) {
            physical_device_feature_struct_info = vul::FeatureStructInfo(
                    type_node);
        }
    }
    //TODO iterate through feature->require->enum->extends,name,bitpos,offset
    //TODO iterate through extensions->extension->require->enum->extends,name,bitpos,offset

    for (pugi::xml_node enums: root.children("enums")) {
        std::string name = enums.attribute("name").as_string();
        if(name == "VkAccessFlagBits2"){
            std::cout << name << std::endl;
        }
        if (name == "API Constants") {
            continue;
        }

        std::cout << name << std::endl;
        std::string underlying_type;
        if (auto enum_itr = enum_underlying_type_map.find(name);
                enum_itr != enum_underlying_type_map.end()) {
            underlying_type = enum_itr->second;
        }
        std::string define_string;
        if (auto enum_itr = type_platform_define_map.find(name);
                enum_itr != type_platform_define_map.end()) {
            define_string = enum_itr->second;
        }
        if (name == "VkResult") {
            enum_infos[name] = vul::EnumInfo(enums, tags, underlying_type,
                                             false, define_string);
        } else {
            enum_infos[name] = vul::EnumInfo(enums, tags, underlying_type, true,
                                             define_string);
        }
    }

    //TODO iterate through feature->require->enum->extends,name,bitpos,offset
    //TODO iterate through extensions->extension->require->enum->extends,name,bitpos,offset
    for (pugi::xml_node feature: root.children("feature")) {
        for (pugi::xml_node require: feature.children("require")) {
            if (require.attribute("comment").as_string() ==
                std::string("API constants")) {
                continue;
            }
            for (auto child : require.children("enum")) {
                std::string name = child.attribute("name").as_string();
                std::string extends = child.attribute("extends").as_string();
                if (extends.empty()) {
                    continue;
                }
                if (auto enum_itr = enum_infos.find(extends); enum_itr !=
                                                              enum_infos.end()) {
                    enum_itr->second.append(child);
                } else {
                    throw std::runtime_error(fmt::format(
                            "Can't find pre-existing enum for enum with name {}",
                            extends));
                }
            }
        }
    }

    for (pugi::xml_node extensions: root.children("extensions")) {
        for (pugi::xml_node extension: extensions.children("extension")) {
            auto platform_name = extension.attribute("platform").as_string();
            if (extension.attribute("supported").as_string() ==
                std::string("disabled")) {
                continue;
            }
            for (pugi::xml_node require: extension.children("require")) {
                for (auto child : require.children("enum")) {
                    std::string name = child.attribute("name").as_string();
                    //already added....
                    //"Note that this defines what was previously a core enum, and so uses the 'value' attribute rather than 'offset', and does not have a suffix. This is a special case, and should not be repeated"/
                    if (name ==
                        "VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE") {
                        continue;
                    }


                    std::string extends = child.attribute(
                            "extends").as_string();
                    if (extends.empty()) {
                        continue;
                    }
                    if (auto enum_itr = enum_infos.find(extends);
                            enum_itr !=
                            enum_infos.end()) {

                        enum_itr->second.append(child,
                                                platform_define_map[platform_name]);
                    } else {
                        throw std::runtime_error(fmt::format(
                                "Can't find pre-existing enum for enum with name {}",
                                extends));
                    }
                }
            }
        }
    }




//    {
//        auto[declaration, definition] = enums_infos["VkPointClippingBehavior"].generate_declaration_definition(
//                tags, "vul");
//        fmt::print("{}", declaration);
//        fmt::print("{}", definition);
//    }
    std::string namespace_str("vul");
    std::string indent("    ");
    std::string file_name = "enums";
    std::string enum_source = fmt::format("#include \"{}.h\"\n", file_name);
    std::string enum_header = fmt::format("{}\n"
            "#include <vulkan/vulkan.h>\n"
                                          "#include <string_view>\n"
                                          "#include <string>\n"
                                          "#include <optional>\n"
                                          "namespace {}{{\n", vul::start_include_guard("VUL_ENUMS_H"), namespace_str);
    std::string enum_forward = fmt::format("{}\n"
                                           "#include <vulkan/vulkan.h>\n"
                                           "namespace {}{{\n", vul::start_include_guard("VUL_ENUMSFWD_H"),namespace_str);

    std::string enum_util_header = fmt::format("{}\n"
                                               "#include <vulkan/vulkan.h>\n"
                                               "#include <string_view>\n"
                                               "#include <string>\n"
                                               "#include <optional>\n"
                                               "namespace {}{{\n",
                                               vul::start_include_guard("VUL_ENUMUTIL_H"),namespace_str);
    std::string enum_util_source = fmt::format("#include \"{}.h\"\n",
                                               "enumutils");
    std::string bitmask_forward = fmt::format("{}\n"
                                              "namespace {}{{\n",
                                              vul::start_include_guard("VUL_BITMASKSFWD_H"),namespace_str);
    std::string bitmask_header = fmt::format("{}\n"
                                             "#include \"{}.h\"\n"
                                             "#include <type_traits>\n"
                                             "namespace {}{{\n", vul::start_include_guard("VUL_BITMASKS_H"), file_name,
                                             namespace_str);
    std::string bitmask_source = fmt::format("#include \"{}.h\"\n", "bitmasks");
    for (auto[key, enum_info] : enum_infos) {
        if (enum_info.get_enum_values().empty()) {
            continue;
        }
        if (!enum_info.define_string.empty()) {
            auto define_str = vul::start_define(enum_info.define_string);
            enum_source += define_str;
            enum_header += define_str;
            enum_forward += define_str;
            bitmask_source += define_str;
            bitmask_header += define_str;
            bitmask_forward += define_str;
        }
        enum_header += vul::generate_enum_class(enum_info, indent);
        {
            auto[declaration, definition] = vul::generate_to_string_vk_enum(
                    enum_info, namespace_str, indent);
            enum_header += declaration;
            enum_source += definition;
        }
        {
            auto[declaration, definition] = vul::generate_to_string_enum_class(
                    enum_info, namespace_str, indent);
            enum_header += declaration;
            enum_source += definition;
        }
        {
            auto[declaration, definition] = vul::generate_from_string_vk_enum(
                    enum_info, namespace_str, indent);
            enum_header += declaration;
            enum_source += definition;
        }
        {
            auto[declaration, definition] = vul::generate_from_string_enum_class(
                    enum_info, namespace_str, indent);
            enum_header += declaration;
            enum_source += definition;
        }
        {
            auto[declaration, definition] = vul::generate_enum_get(enum_info,
                                                                   namespace_str,
                                                                   indent);
            enum_header += declaration;
            enum_source += definition;
        }
        enum_forward += vul::generate_forward_declaration(enum_info, indent);
        if (enum_info.type == vul::EnumType::Bitmask) {
            auto[declaration, definition] = vul::generate_bitmask_wrapper(
                    enum_info, namespace_str, indent);
            bitmask_header += declaration;
            bitmask_source += definition;
            bitmask_forward += vul::generate_bitmask_forward(enum_info, indent);
        }
        if (!enum_info.define_string.empty()) {
            auto define_str = vul::end_define(enum_info.define_string);
            enum_source += define_str;
            enum_header += define_str;
            enum_forward += define_str;
            bitmask_source += define_str;
            bitmask_header += define_str;
            bitmask_forward += define_str;
        }
    }
    enum_header += fmt::format("}}\n");
    enum_forward += fmt::format("}}\n");
    bitmask_header += fmt::format("}}\n");
    bitmask_forward += fmt::format("}}\n");
    enum_header += vul::end_include_guard("VUL_ENUMS_H");
    enum_forward += vul::end_include_guard("VUL_ENUMSFWD_H");
    bitmask_header += vul::end_include_guard("VUL_BITMASKS_H");
    bitmask_forward += vul::end_include_guard("VUL_BITMASKSFWD_H");

    //TODO need a make directory thing here if doesn't exist.
    std::string output_dir = std::string(VUL_GEN_RESOURCE_PATH) + "/../vul/generated_sources/vul/";
    std::filesystem::create_directories(std::filesystem::path(output_dir));

    std::ofstream enum_forward_file(output_dir + "enumsfwd.h");
    std::ofstream enum_header_file(output_dir + "enums.h");
    std::ofstream enum_source_file(output_dir + "enums.cpp");
    std::ofstream bitmask_forward_file(output_dir + "bitmasksfwd.h");
    std::ofstream bitmask_header_file(output_dir + "bitmasks.h");
    std::ofstream bitmask_source_file(output_dir + "bitmasks.cpp");

    enum_forward_file << enum_forward;
    enum_header_file << enum_header;
    enum_source_file << enum_source;
    bitmask_forward_file << bitmask_forward;
    bitmask_header_file << bitmask_header;
    bitmask_source_file << bitmask_source;


    //add includes plus namespace
    //add forward declare.
    std::string feature_header = fmt::format("{0}\n"
                                             "#include \"vulkan/vulkan.h\"\n"
                                             "namespace {1}{{\n",
                                             vul::start_include_guard("VUL_FEATURES_H"),namespace_str);
    std::string feature_source = "#include \"features.h\"\n";
    std::string feature_forward = fmt::format("{0}\n"
                                              "namespace {2}{{\n"
                                              "{1}class Features;\n"
                                              "}}\n"
                                              "{3}", vul::start_include_guard("VUL_FEATURESFWD_H"),indent, namespace_str,
                                              vul::end_include_guard("VUL_FEATURESFWD_H"));

    {
        auto[declaration, definition] = vul::generate_feature_struct_code(
                feature_struct_infos, physical_device_feature_struct_info,
                indent, namespace_str, type_platform_define_map);
        feature_header += vul::add_to_each_line(declaration, indent);
        feature_source += definition;
    }
    feature_header += fmt::format("}}\n");
    feature_header += vul::end_include_guard("VUL_FEATURES_H");
    std::ofstream feature_forward_file(output_dir + "featuresfwd.h");
    std::ofstream feature_header_file(output_dir + "features.h");
    std::ofstream feature_source_file(output_dir + "features.cpp");

    feature_forward_file << feature_forward;
    feature_header_file << feature_header;
    feature_source_file << feature_source;

    std::string object_type_header = fmt::format("{0}\n"
            "#include \"vulkan/vulkan.h\"\n"
            "#include \"enums.h\"\n"
            "namespace {1}{{\n"
            "{2}template<typename T>\n"
            "{2}ObjectType getObjectType() {{\n"
            "{2}{2}return  static_cast<ObjectType>(VK_OBJECT_TYPE_UNKNOWN);\n"
            "{2}}}\n", vul::start_include_guard("VUL_OBJECTTYPEUTILS_H"), namespace_str, indent);
    std::string object_type_source = "#include \"objecttypeutils.h\"\n";

    {
        for (const auto&[type_name, values]: objtypeenum_map) {
            if (!values.platform.empty()) {
                auto define_str = vul::start_define(values.platform);
                object_type_header += define_str;
                object_type_source += define_str;
            }
            object_type_header += fmt::format("{0}template<>\n"
                                              "{0}ObjectType getObjectType<{1}>();\n",
                                              indent, type_name);
            object_type_source += fmt::format("template<>\n"
                                              "{2}::ObjectType {2}::getObjectType<{1}>(){{\n"
                                              "{0}return static_cast<ObjectType>({3});\n"
                                              "}}\n",
                                              indent, type_name, namespace_str,
                                              values.objtypeenum);
            if (!values.platform.empty()) {
                auto define_str = vul::end_define(values.platform);
                object_type_header += define_str;
                object_type_source += define_str;
            }
        }
    }
    object_type_header += fmt::format("}}\n");
    object_type_header += vul::end_include_guard("VUL_OBJECTTYPEUTILS_H");
    std::ofstream object_type_header_file(output_dir + "objecttypeutils.h");
    std::ofstream object_type_source_file(output_dir + "objecttypeutils.cpp");

    object_type_header_file << object_type_header;
    object_type_source_file << object_type_source;

    fmt::print("hello world \n");
}