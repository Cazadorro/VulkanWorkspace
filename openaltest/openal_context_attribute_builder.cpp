//
// Created by Bolt on 10/21/2023.
//

#include "openal_context_attribute_builder.h"
#include <uul/assert.h>
namespace openal{
    std::size_t InitAttributeBuilder::to_index(InitAttribute attribute) {
        switch(attribute){
            case InitAttribute::NullTerminator:
                return 8;
            case InitAttribute::Frequency:
                return 0;
            case InitAttribute::Refresh:
                return 1;
            case InitAttribute::Sync:
                return 2;
            case InitAttribute::MonoSources:
                return 3;
            case InitAttribute::StereoSources:
                return 4;
            case InitAttribute::Hrtf:
                return 5;
            case InitAttribute::HrtfId:
                return 6;
            case InitAttribute::OutputLimiter:
                return 7;
//            case InitAttribute::DontCare:
//                break;
            default:
                UUL_ASSERT(false, "attribute outside of range");
                break;
        }
    }

    InitAttributeBuilder& InitAttributeBuilder::set_frequency(std::int32_t value_hz) {
        attribute_list[to_index(InitAttribute::Frequency)].value = value_hz;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_refresh(std::int32_t value_hz) {
        attribute_list[to_index(InitAttribute::Refresh)].value = value_hz;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_sync(bool value) {
        attribute_list[to_index(InitAttribute::Sync)].value = value;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_mono_sources(std::int32_t value) {
        attribute_list[to_index(InitAttribute::MonoSources)].value = value;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_stereo_sources(std::int32_t value) {
        attribute_list[to_index(InitAttribute::StereoSources)].value = value;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_hrtf(bool value) {
        attribute_list[to_index(InitAttribute::Hrtf)].value = value;
        return *this;
    }

    InitAttributeBuilder& InitAttributeBuilder::set_hrtf_id(std::int32_t value) {
        attribute_list[to_index(InitAttribute::HrtfId)].value = value;
        return *this;
    }

    std::optional<std::int32_t> InitAttributeBuilder::get_frequency() const {
        return get<std::int32_t>(InitAttribute::Frequency);
    }

    std::optional<std::int32_t> InitAttributeBuilder::get_refresh() const {
        return get<std::int32_t>(InitAttribute::Refresh);
    }

    std::optional<bool> InitAttributeBuilder::get_sync() const {
        return get<bool>(InitAttribute::Sync);
    }

    std::optional<std::int32_t> InitAttributeBuilder::get_mono_sources() const {
        return get<std::int32_t>(InitAttribute::MonoSources);
    }

    std::optional<std::int32_t> InitAttributeBuilder::get_stereo_sources() const {
        return get<std::int32_t>(InitAttribute::StereoSources);
    }

    std::optional<bool> InitAttributeBuilder::get_hrtf() const {
        return get<bool>(InitAttribute::Hrtf);
    }

    std::optional<std::int32_t> InitAttributeBuilder::get_hrtf_id() const {
        return get<std::int32_t>(InitAttribute::HrtfId);
    }

    std::span<const std::int32_t> InitAttributeBuilder::to_span() const {
        return {reinterpret_cast<const std::int32_t*>(attribute_list.data()), attribute_list.size()};
    }

    std::span<std::int32_t> InitAttributeBuilder::to_span() {
        return {reinterpret_cast<std::int32_t*>(attribute_list.data()), attribute_list.size()};
    }

    InitAttributeBuilder& InitAttributeBuilder::set_output_limiter(bool value) {
        attribute_list[to_index(InitAttribute::OutputLimiter)].value = value;
        return *this;
    }

    std::optional<bool> InitAttributeBuilder::get_output_limiter() const {
        return get<bool>(InitAttribute::OutputLimiter);
    }

}