//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_CONTEXT_ATTRIBUTE_BUILDER_H
#define VULKANWORKSPACE_OPENAL_CONTEXT_ATTRIBUTE_BUILDER_H
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <span>
#include <array>
#include <optional>
#include <cstdint>
namespace openal {
    enum class InitAttribute : ALint {
        NullTerminator = 0,
        Frequency = ALC_FREQUENCY, // hz
        Refresh = ALC_REFRESH, //hz
        Sync = ALC_SYNC, //true false
        MonoSources = ALC_MONO_SOURCES, //count?
        StereoSources = ALC_STEREO_SOURCES,
        // ALC_SOFT_HRTF
        Hrtf = ALC_HRTF_SOFT, // TRUE FALSE
        HrtfId = ALC_HRTF_ID_SOFT,

        OutputLimiter = ALC_OUTPUT_LIMITER_SOFT, //ALC_SOFT_output_limiter
        DontCare = ALC_DONT_CARE_SOFT,
    };

    struct InitAttributePair {
        InitAttribute attribute = {};
        ALint value = {};

        constexpr InitAttributePair() = default;

        constexpr explicit InitAttributePair(InitAttribute attribute, ALint value) :
                attribute(attribute), value(value) {};

        constexpr explicit InitAttributePair(InitAttribute attribute, bool value) :
                InitAttributePair(attribute, static_cast<ALint>(value ? AL_TRUE : AL_FALSE)) {};
    };

    class InitAttributeBuilder {
    public:
        std::array<InitAttributePair, 9> attribute_list = {
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{InitAttribute::DontCare, 0},
                InitAttributePair{}
        };

        [[nodiscard]]
        std::span<const std::int32_t> to_span() const;
        [[nodiscard]]
        std::span<std::int32_t> to_span();
        template<typename T>
        void set(InitAttribute attribute, T value) {
            auto output_index = to_index(attribute);
            attribute_list[output_index] = InitAttributePair{attribute, value};
        }

        template<typename T>
        [[nodiscard]]
        std::optional<T> get(InitAttribute attribute) const{
            auto index = to_index(attribute);
            if( attribute_list[index].attribute == InitAttribute::DontCare ||
                attribute_list[index].attribute == InitAttribute::NullTerminator){
                return std::nullopt;
            }
            return static_cast<T>(attribute_list[index].value);
        }

        InitAttributeBuilder& set_frequency(std::int32_t value_hz);
        InitAttributeBuilder& set_refresh(std::int32_t value_hz);
        InitAttributeBuilder& set_sync(bool value);
        InitAttributeBuilder& set_mono_sources(std::int32_t value);
        InitAttributeBuilder& set_stereo_sources(std::int32_t value);
        InitAttributeBuilder& set_hrtf(bool value);
        InitAttributeBuilder& set_hrtf_id(std::int32_t value);


        [[nodiscard]]
        std::optional<std::int32_t> get_frequency() const;
        [[nodiscard]]
        std::optional<std::int32_t> get_refresh() const;
        [[nodiscard]]
        std::optional<bool> get_sync() const;
        [[nodiscard]]
        std::optional<std::int32_t> get_mono_sources() const;
        [[nodiscard]]
        std::optional<std::int32_t> get_stereo_sources() const;
        [[nodiscard]]
        std::optional<bool> get_hrtf() const;
        [[nodiscard]]
        std::optional<std::int32_t> get_hrtf_id() const;

        InitAttributeBuilder& set_output_limiter(bool value);
        [[nodiscard]]
        std::optional<bool> get_output_limiter() const;
    private:
        [[nodiscard]]
        static std::size_t to_index(InitAttribute attribute);

    };
}
#endif //VULKANWORKSPACE_OPENAL_CONTEXT_ATTRIBUTE_BUILDER_H
