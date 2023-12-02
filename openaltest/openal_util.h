//
// Created by Bolt on 10/18/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_UTIL_H
#define VULKANWORKSPACE_OPENAL_UTIL_H

#define AL_ALEXT_PROTOTYPES

#include "openal/alc.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-presets.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tl/expected.hpp>
#include <concepts>
#include <bit>
#include <optional>
#include <vector>
#include <span>
#include <array>
#include <string>
#include <cstdint>

//TODO convert all gets into return value things to make things simpler. use templates
//TODO use custom units/std::units for relevant things allow units to be extended with macros? for mp/units?
//TODO create "level 0" wrapper for low level functions *below* "level 1" wrappers.
namespace openal {
    constexpr static std::array al_required_extensions = {
            "AL_EXT_FLOAT32",
            "AL_EXT_double",
            "AL_EXT_MULAW",
            "AL_EXT_ALAW",
            "AL_EXT_MCFORMATS",
            "AL_EXT_MULAW_MCFORMATS",
            "AL_EXT_IMA4",
            "AL_SOFT_MSADPCM",
            "AL_EXT_BFORMAT",
            "AL_EXT_MULAW_BFORMAT",

//            "AL_EXT_vorbis",  not found!, but may depend on external lib being loaded anyway.
            "AL_EXT_STATIC_BUFFER", //Not found!
            "AL_EXT_source_distance_model",
//            "AL_SOFT_buffer_sub_data",  Not found!
            "AL_SOFT_loop_points",
//            "AL_SOFT_buffer_samples",  Not found!
            "AL_SOFT_direct_channels",
            "AL_EXT_STEREO_ANGLES",
            "AL_EXT_SOURCE_RADIUS",

            "AL_SOFT_source_latency",
            "AL_SOFT_deferred_updates",
            "AL_SOFT_block_alignment",

            "AL_SOFT_source_length",
            "AL_SOFT_buffer_length_query",
            "AL_SOFT_gain_clamp_ex",
            "AL_SOFT_source_resampler",
            "AL_SOFT_source_spatialize",
            "AL_SOFT_direct_channels_remix",
            "AL_SOFT_bformat_ex",
            "AL_SOFT_effect_target",
            "AL_SOFT_events",
            "AL_SOFT_callback_buffer",
    };

    constexpr static std::array alc_required_extensions = {
//            "ALC_EXT_DEFAULT_FILTER_ORDER", not supported
            "ALC_EXT_EFX",
            "ALC_EXT_disconnect",
            "ALC_EXT_thread_local_context",
            "ALC_SOFT_loopback", //TODO do loop back stuff
            "ALC_SOFT_pause_device",
            "ALC_SOFT_HRTF",
            "ALC_SOFT_output_limiter",
            "ALC_SOFT_device_clock",
            "ALC_SOFT_loopback_bformat", //TODO do loop back stuff
            "ALC_SOFT_reopen_device",
            "ALC_SOFT_output_mode"
    };
    enum class AlcStringAttribute : ALenum {
        DefaultDeviceSpecifier = ALC_DEFAULT_DEVICE_SPECIFIER,
        CaptureDefaultDeviceSpecifier = ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER,
        DeviceSpecifier = ALC_DEVICE_SPECIFIER,
        CaptureDeviceSpecifier = ALC_CAPTURE_DEVICE_SPECIFIER,
        Extensions = ALC_EXTENSIONS
    };

    enum class AlcErrorState : ALenum {
        NoError = ALC_NO_ERROR,
        InvalidDevice = ALC_INVALID_DEVICE,
        InvalidContext = ALC_INVALID_CONTEXT,
        InvalidEnum = ALC_INVALID_ENUM,
        InvalidValue = ALC_INVALID_VALUE,
        OutOfMemory = ALC_OUT_OF_MEMORY
    };


    [[nodiscard]]
    std::string to_string(AlcErrorState error_state);

    enum class AlErrorState : ALenum {
        NoError = AL_NO_ERROR,
        InvalidName = AL_INVALID_NAME,
        InvalidEnum = AL_INVALID_ENUM,
        InvalidValue = AL_INVALID_VALUE,
        InvalidOperation = AL_INVALID_OPERATION,
        OutOfMemory = AL_OUT_OF_MEMORY
    };

    [[nodiscard]]
    std::string to_string(AlErrorState error_state);

    [[nodiscard]]
    std::vector<std::string> get_device_name_list();

    [[nodiscard]]
    std::string get_default_device_name();

    [[nodiscard]]
    std::vector<std::string> get_capture_device_name_list();

    [[nodiscard]]
    std::string get_default_capture_device_name();


    enum class Format : ALenum {
        Mono8 = AL_FORMAT_MONO8,
        Mono16 = AL_FORMAT_MONO16,
        Stereo8 = AL_FORMAT_STEREO8,
        Stereo16 = AL_FORMAT_STEREO16,
        MonoFloat32 = AL_FORMAT_MONO_FLOAT32,
        StereoFloat32 = AL_FORMAT_STEREO_FLOAT32,
        MonoDouble = AL_FORMAT_MONO_DOUBLE_EXT,
        StereoDouble = AL_FORMAT_STEREO_DOUBLE_EXT,
        MonoMulaw = AL_FORMAT_MONO_MULAW_EXT,
        StereoMulaw = AL_FORMAT_STEREO_MULAW_EXT,
        MonoAlaw = AL_FORMAT_MONO_ALAW_EXT,
        StereoAlaw = AL_FORMAT_STEREO_ALAW_EXT,
        QuadMulaw = AL_FORMAT_QUAD_MULAW,
        RearMulaw = AL_FORMAT_REAR_MULAW,
        SurroundSound_5Point1_ChnMulaw = AL_FORMAT_51CHN_MULAW,
        SurroundSound_6Point1_ChnMulaw = AL_FORMAT_61CHN_MULAW,
        SurroundSound_7Point1_ChnMulaw = AL_FORMAT_71CHN_MULAW,
        Quad8 = AL_FORMAT_QUAD8,
        Quad16 = AL_FORMAT_QUAD16,
        Quad32 = AL_FORMAT_QUAD32,
        Rear8 = AL_FORMAT_REAR8,
        Rear16 = AL_FORMAT_REAR16,
        Rear32 = AL_FORMAT_REAR32,
        SurroundSound_5Point1_Chn8 = AL_FORMAT_51CHN8,
        SurroundSound_5Point1_Chn16 = AL_FORMAT_51CHN16,
        SurroundSound_5Point1_Chn32 = AL_FORMAT_51CHN32,
        SurroundSound_6Point1_Chn8 = AL_FORMAT_61CHN8,
        SurroundSound_6Point1_Chn16 = AL_FORMAT_61CHN16,
        SurroundSound_6Point1_Chn32 = AL_FORMAT_61CHN32,
        SurroundSound_7Point1_Chn8 = AL_FORMAT_71CHN8,
        SurroundSound_7Point1_Chn16 = AL_FORMAT_71CHN16,
        SurroundSound_7Point1_Chn32 = AL_FORMAT_71CHN32,
        MonoIma4 = AL_FORMAT_MONO_IMA4,
        StereoIma4 = AL_FORMAT_STEREO_IMA4,
        MonoMsadpcm = AL_FORMAT_MONO_MSADPCM_SOFT,
        StereoMsadpcm = AL_FORMAT_STEREO_MSADPCM_SOFT,
        Bformat2d8 = AL_FORMAT_BFORMAT2D_8,
        Bformat2d16 = AL_FORMAT_BFORMAT2D_16,
        Bformat2dFloat32 = AL_FORMAT_BFORMAT2D_FLOAT32,
        Bformat3d8 = AL_FORMAT_BFORMAT3D_8,
        Bformat3d16 = AL_FORMAT_BFORMAT3D_16,
        Bformat3dFloat32 = AL_FORMAT_BFORMAT3D_FLOAT32,
        Bformat2dMulaw = AL_FORMAT_BFORMAT2D_MULAW,
        Bformat3dMulaw = AL_FORMAT_BFORMAT3D_MULAW,
    };

    Format make_format(std::uint8_t channel_count, std::uint8_t bits_per_sample);


    //added value so that derived are not accidentally comparable.
    template<typename T, typename Derived_T>
    class Handle {
    public:
        explicit constexpr Handle(std::nullptr_t) noexcept {};

        constexpr Handle() noexcept = default;

        constexpr explicit Handle(T handle) noexcept: m_handle(handle) {};

        [[nodiscard]]
        constexpr explicit operator T() const noexcept {
            return m_handle;
        };

        [[nodiscard]]
        constexpr const T &get_handle() const noexcept {
            return m_handle;
        }

        [[nodiscard]]
        constexpr T &get_handle() noexcept {
            return m_handle;
        }

    private:
        T m_handle = T{};
    };

    //TODO may not work according to https://openal-soft.org/openal-extensions/SOFT_source_latency.txt
    // due to endianness.
    template<std::unsigned_integral T>
    struct FixedPoint {
        T upper_bits;
        T lower_bits;
    };


    template<typename T, typename Derived_T>
    [[nodiscard]]
    constexpr bool operator==(const Handle<T, Derived_T> &lhs, const Handle<T, Derived_T> &rhs) noexcept {
        return lhs.get_handle() == rhs.get_handle();
    }

    template<typename T, typename Derived_T>
    [[nodiscard]]
    constexpr bool operator!=(const Handle<T, Derived_T> &lhs, const Handle<T, Derived_T> &rhs) noexcept {
        return lhs.get_handle() != rhs.get_handle();
    }

    template<typename T>
    struct AtUp {
        glm::vec<3, T> at;
        glm::vec<3, T> up;

        [[nodiscard]]
        std::span<const T> as_span() const {
            return std::span<const T>(glm::value_ptr(at), 6);
        }

        [[nodiscard]]
        std::span<T> as_span() {
            return std::span<T>(glm::value_ptr(at), 6);
        }
    };

    static_assert(sizeof(AtUp<float>) == sizeof(float) * 6);
    static_assert(sizeof(AtUp<std::int32_t>) == sizeof(std::int32_t) * 6);

}

#endif //VULKANWORKSPACE_OPENAL_UTIL_H
