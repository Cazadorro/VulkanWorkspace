//
// Created by Shae Bolt on 8/22/2021.
//

#ifndef VULKANWORKSPACE_VERTEXINPUTSTATE_H
#define VULKANWORKSPACE_VERTEXINPUTSTATE_H
#include "vul/struct_to_tuple.h"
#include "vul/enums.h"
#include <glm/fwd.hpp>
#include <vulkan/vulkan.h>
#include <tuple>
namespace vul{
    template<typename T>
    [[nodiscard]]
    vul::Format getAttributeFormat();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::uint8_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::int8_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::uint16_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::int16_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::uint32_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::int32_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::uint64_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<std::int64_t>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<float>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<double>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u8vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i8vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u16vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i16vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u32vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i32vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u64vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i64vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f32vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f64vec2>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u8vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i8vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u16vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i16vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u32vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i32vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u64vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i64vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f32vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f64vec3>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u8vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i8vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u16vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i16vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u32vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i32vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::u64vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::i64vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f32vec4>();
    template<>
    [[nodiscard]]
    vul::Format getAttributeFormat<glm::f64vec4>();

    template<typename T>
    VkVertexInputBindingDescription createVertexBindingDescription(std::uint32_t binding = 0, vul::VertexInputRate inputRate = vul::VertexInputRate::Vertex){
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = binding;
        bindingDescription.stride = sizeof(T);
        bindingDescription.inputRate = vul::get(inputRate);

        return bindingDescription;
    }

    template<typename T, size_t... I>
    auto
    reverse_tuple_impl(T&& t, std::index_sequence<I...>)
    {
        return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
    }

    template<typename T>
    auto
    reverse_tuple(T&& t)
    {
        return reverse_tuple_impl(std::forward<T>(t),
                            std::make_index_sequence<std::tuple_size<T>::value>());
    }

    template<typename T>
    auto createVertexAttributeDescriptions(std::uint32_t binding = 0) {
        auto attribute_tuple = reverse_tuple(vul::struct_to_tuple(T{}));
        using tuple_type = decltype(attribute_tuple);
        std::array<VkVertexInputAttributeDescription,
        std::tuple_size_v<tuple_type>> attributeDescriptions;
        std::size_t reversing_factor = attributeDescriptions.size() > 0 ?attributeDescriptions.size() - 1 : 0;
        std::uint32_t array_index = 0;
        std::uint32_t offset = 0;
        // for each attribute description for each element in the struct,
        // we are going to extract the tuple type for the format and calculate offset
        auto foo = [&array_index, &offset, &binding, &attributeDescriptions, &reversing_factor](auto argument){
            std::uint32_t reverse_index = array_index;
            attributeDescriptions[reverse_index].location = reverse_index;
            attributeDescriptions[reverse_index].binding = binding;
            attributeDescriptions[reverse_index].format = vul::get(getAttributeFormat<decltype(argument)>());
            auto element_size = sizeof(argument);
            auto alignment = std::alignment_of<decltype(argument)>();
            auto remainder = offset % alignment;
            if(remainder == 0 || remainder > element_size){
                attributeDescriptions[reverse_index].offset = offset;
            }else{
                offset += remainder;
                attributeDescriptions[reverse_index].offset = offset;
            }
            offset += element_size;
            array_index += 1;
            return 0;
        };
        std::apply(
                [&foo](auto&&... tupleArgs){
                    std::make_tuple(foo(tupleArgs)...);
                }, attribute_tuple//attribute_tuple
        );
        return attributeDescriptions;
    }


//    struct Vertex {
//        glm::vec3 pos;
//        glm::vec3 color;
//        glm::vec2 texCoord;
//    };
//    void foo(){
//        auto bindingDescription = createVertexBindingDescription<Vertex>();
//        std::array<VkVertexInputAttributeDescription, 3> attributeDescription = createVertexAttributeDescriptions<Vertex>();
//    }

//    template<typename T>
//    class VertexInputStateBuilder{
//    public:
//        VertexInputStateBuilder(){
//            m_createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//            m_createInfo.pNext = nullptr;
//            m_createInfo.flags = 0;
//
//        }
//
//    private:
//        VkPipelineVertexInputStateCreateInfo m_createInfo = {};
//    };
//TODO put this inside the pipeline creation.
//        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
//        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//        auto bindingDescription = Vertex::getBindingDescription();
//        auto attributeDescriptions = Vertex::getAttributeDescriptions();
//
//        vertexInputInfo.vertexBindingDescriptionCount = 1;
//        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
//        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
//        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

}


#endif //VULKANWORKSPACE_VERTEXINPUTSTATE_H
