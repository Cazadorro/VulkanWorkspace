//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_DEVICE_H
#define VULKANWORKSPACE_OPENAL_DEVICE_H

#include "openal_util.h"
#include "openal/alc.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>
#include <array>

namespace openal{
    class Context;
    class InitAttributeBuilder;

    class DeviceHandle : public Handle<ALCdevice *, DeviceHandle> {
    public:
        using Handle::Handle;

        [[nodiscard]]
        alc::result get_error() const;

        [[nodiscard]]
        alc::expected<Context> create_context(std::span<const std::int32_t> attribute_list) const;
        [[nodiscard]]
        alc::expected<Context> create_context(const InitAttributeBuilder& attribute_list) const;


        //extensions
        [[nodiscard]]
        tl::expected<tl::monostate, AlcErrorState> pause();

        [[nodiscard]]
        tl::expected<tl::monostate, AlcErrorState> resume();

        [[nodiscard]]
        tl::expected<bool, AlcErrorState> reset_device(std::span<const std::int32_t> attribute_list);
        [[nodiscard]]
        tl::expected<bool, AlcErrorState> reset_device(const InitAttributeBuilder& attribute_list);

        enum class Attribute: ALenum{
            MajorVersion = ALC_MAJOR_VERSION,
            MinorVersion = ALC_MINOR_VERSION,
            AttributesSize = ALC_ATTRIBUTES_SIZE,
            AllAttributes = ALC_ALL_ATTRIBUTES,

            //ALC_SOFT_HRTF
            Hrtf = ALC_HRTF_SOFT, // int
            HrtfId = ALC_HRTF_ID_SOFT, // only in alcDeviceResetSOFT and attribute list
            DontCare = ALC_DONT_CARE_SOFT, //don't use except for attribute list
            HrtfStatus = ALC_HRTF_STATUS_SOFT, // int
            NumHrtfSpecifiers = ALC_NUM_HRTF_SPECIFIERS_SOFT,
            HrtfSpecifier = ALC_HRTF_SPECIFIER_SOFT, // string
            OutputLimiter = ALC_OUTPUT_LIMITER_SOFT, //ALC_SOFT_output_limiter

            EfxMajorVersion = ALC_EFX_MAJOR_VERSION, // ALC_EXT_EFX
            EfxMinorVersion = ALC_EFX_MINOR_VERSION,
            MaxAuxiliarySends = ALC_MAX_AUXILIARY_SENDS,

            DeviceClock = ALC_DEVICE_CLOCK_SOFT,
            DeviceLatency = ALC_DEVICE_LATENCY_SOFT,
            DeviceClockLatency = ALC_DEVICE_CLOCK_LATENCY_SOFT,
        };

        enum class HrtfStatus : ALenum { //ALC_SOFT_HRTF
            Disabled = ALC_HRTF_DISABLED_SOFT,
            Enabled = ALC_HRTF_ENABLED_SOFT,
            Denied = ALC_HRTF_DENIED_SOFT,
            Required = ALC_HRTF_REQUIRED_SOFT,
            HeadphonesDetected = ALC_HRTF_HEADPHONES_DETECTED_SOFT,
            UnsupportedFormat = ALC_HRTF_UNSUPPORTED_FORMAT_SOFT,
        };



        [[nodiscard]]
        tl::expected<ALvoid*, AlcErrorState> get_proc_address(const ALCchar * funcname) const;

        [[nodiscard]]
        tl::expected<ALCenum , AlcErrorState> get_enum_value(const ALCchar *enumname) const;

        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlcErrorState> get(Attribute attribute) const = delete;

        template<>
        [[nodiscard]]
        tl::expected<std::string, AlcErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<std::int64_t, AlcErrorState> get(Attribute attribute) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlcErrorState> get(Attribute attribute, std::span<std::int32_t> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlcErrorState> get(Attribute attribute, std::span<std::int64_t> value) const;


        template<typename T, typename U>
        [[nodiscard]]
        tl::expected<U, AlcErrorState> get(Attribute attribute) const{
            static constexpr auto array_size = sizeof(U) / sizeof(T);
            static_assert(array_size * sizeof(T) == sizeof(U), "Expected T to exactly fill U");
            std::array<T, array_size> value = {};
            return get(attribute, std::span(value)).map([value](auto result){
                return std::bit_cast<U>(value);
            });
        }


        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlcErrorState> get(Attribute attribute, std::int32_t value) const = delete;
        template<>
        [[nodiscard]]
        tl::expected<std::string, AlcErrorState> get(Attribute attribute, std::int32_t index) const;
        template<>
        [[nodiscard]]
        tl::expected<std::vector<std::string>, AlcErrorState> get(Attribute attribute, std::int32_t count) const;


        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_major_version() const;
        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_minor_version() const;
        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_attribute_size() const;
        [[nodiscard]]
        tl::expected<std::vector<std::int32_t>, AlcErrorState> get_all_attributes() const;


        [[nodiscard]]
        tl::expected<bool, AlcErrorState> get_hrtf() const;

        [[nodiscard]]
        tl::expected<HrtfStatus, AlcErrorState> get_hrtf_status() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_num_hrtf_specifiers() const;

        [[nodiscard]]
        tl::expected<std::vector<std::string>, AlcErrorState> get_hrtf_specifiers() const;

        [[nodiscard]]
        tl::expected<bool, AlcErrorState> get_output_limiter() const;

        [[nodiscard]]
        tl::expected<std::int64_t, AlcErrorState> get_device_clock() const;
        [[nodiscard]]
        tl::expected<std::int64_t, AlcErrorState> get_device_latency() const;

        struct ClockLatency{
            std::int64_t clock_ns;
            std::int64_t latency_ns;
        };
        [[nodiscard]]
        tl::expected<ClockLatency, AlcErrorState> get_device_clock_latency() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_efx_major_version() const;
        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_efx_minor_version() const;
        [[nodiscard]]
        tl::expected<std::int32_t, AlcErrorState> get_max_auxiliary_sends() const;
    private:
    };

    static constexpr auto null_device_handle = DeviceHandle();


    class Device : public DeviceHandle {
    public:
        using DeviceHandle::DeviceHandle;

        explicit Device(ALCdevice *device_ptr);

        [[nodiscard]]
        static std::optional<Device> from_name(const char *device_name);

        [[nodiscard]]
        static std::optional<Device> from_name(std::string &device_name);

        [[nodiscard]]
        static std::optional<Device> from_default();

        ~Device();

        Device(Device &&rhs) noexcept;

        Device &operator=(Device &&rhs) noexcept;

        Device(const Device &rhs) = delete;

        Device &operator=(const Device &rhs) = delete;

    private:
    };

    static_assert(sizeof(Device) == sizeof(Handle<ALCdevice *, DeviceHandle>));

}
#endif //VULKANWORKSPACE_OPENAL_DEVICE_H
