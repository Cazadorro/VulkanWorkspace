//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_ALC_H
#define VULKANWORKSPACE_ALC_H
#include "openal/al.h"
#include "openal/utils.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <gsl/zstring>
#include <tl/expected.hpp>
#include <string>
#include <span>
#include <array>
#include <cstdint>
#include <bit>

//TODO return error?
namespace openal::alc{
    enum class ErrorState : ALenum {
        NoError = ALC_NO_ERROR,
        InvalidDevice = ALC_INVALID_DEVICE,
        InvalidContext = ALC_INVALID_CONTEXT,
        InvalidEnum = ALC_INVALID_ENUM,
        InvalidValue = ALC_INVALID_VALUE,
        OutOfMemory = ALC_OUT_OF_MEMORY,
    };


    [[nodiscard]]
    std::string to_string(ErrorState error_state) noexcept;

    template<typename T>
    using expected = tl::expected<T, ErrorState>;

    using result = tl::expected<tl::monostate, ErrorState>;

    enum class ContextAttribute : std::int32_t{
        NullTerminator = 0, // Context creation initialization list ends with null terminator
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
    struct ContextAttributeValue{
        ContextAttribute attribute = ContextAttribute::NullTerminator;
        std::int32_t value = 0;
    };
    static_assert(sizeof(ContextAttributeValue) == 8);


    enum class DeviceAttribute:  std::int32_t{
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

    /***
     * Creates a context using a specified device
     * @param device Pointer to a device.
     * @param attribute_list List of attribute value pairs
     * @return ALCcontext, if error InvalidValue (an additional context could not be created for the specified device)
     * or InvalidDevice (The specified device is not a valid output device).
     */
    [[nodiscard]]
    expected<ALCcontext*> create_context(ALCdevice *device, std::span<const ContextAttributeValue> attribute_list ) noexcept;

    /***
     * Makes the passed in context current.
     * @param context Context to be made current.
     * @return Nothing, or InvalidContext if specified context is invalid.
     */
    [[nodiscard]]
    result make_context_current(ALCcontext *context) noexcept;


    /***
     * Begin to process a resume suspended context, may be more efficient to apply properties when suspended see ::suspend_context()
     * @param context Context to resume.
     * @return Nothing, or InvalidContext if specified context is invalid.
     */
    result process_context(ALCcontext *context) noexcept;

    /***
     * Begin to process a suspended context, may be more efficient to apply properties when suspended see ::process_context()
     * @param context Context to suspend.
     * @return Nothing, or InvalidContext if specified context is invalid.
     */
    result suspend_context(ALCcontext *context) noexcept;

    /***
     * A context which is not current can be destroyed at any time (all sources within that context will also be deleted).
     * alc::make_context_current() should be used to make sure the context to be destroyed is not current
     * (nullptr is valid for alcMakeContextCurrent).
     * @param context Context to destroy.
     * @return Nothing, or InvalidContext if specified context is invalid.
     */
    result destroy_context(ALCcontext *context) noexcept;

    /***
     * Retrieves the pointer to the currently used context.
     * @return pointer to current context.
     */
    [[nodiscard]]
    ALCcontext* get_current_context() noexcept;

    /***
     * Gets device pointer for the given context
     * @param context context to retrieve device pointer from
     * @return the given contexts device or InvalidContext if the specified context is invalid.
     */
    [[nodiscard]]
    expected<ALCdevice*> get_contexts_device(ALCcontext *context) noexcept;

    /***
     * Opens device by null terminated string name.
     * @param devicename a null terminated string that describes the device to open
     * @return A pointer to the device, or nullptr if it failed to open.
     */
    [[nodiscard]]
    ALCdevice* open_device(const gsl::zstring& devicename) noexcept;

    /***
     * Closes the specified device
     * @param device device to close
     * @return true if device successfully closed, false otherwise, InvalidDevice if device didn't exist.
     */
    [[nodiscard]]
    expected<bool> close_device(ALCdevice *device) noexcept;

    /***
     * retrieves teh current context error state
     * @param device device to retrieve the error state from
     * @return Nothing, or the current error state if there is one.
     */
    [[nodiscard]]
    result get_error(ALCdevice *device) noexcept;

    /***
     * retrieves the specified context error state
     * @param context the context to find the current device to retrieve the current error state from.
     * @return Nothing, or the current error state if there is one.
     */
    [[nodiscard]]
    result get_error(ALCcontext *context) noexcept;

    /***
     * retrieves the current context error state
     * @return Nothing, or the current error state if there is one.
     */
    [[nodiscard]]
    result get_current_context_error() noexcept;

    /***
     * Finds if the specified extension name is availible for the current context
     * @param device device to query from
     * @param extname null terminated string of extension
     * @return true if extension is present, false if not, InvalidValue if extension name is invalid.
     */
    [[nodiscard]]
    expected<bool> is_extension_present(ALCdevice* device, const gsl::zstring& extname) noexcept;

    /***
     * Retrieves the address of a specified context extension function
     * @param device device to query from
     * @param funcname null terminated string of function name
     * @return Address of the function, nullptr if not found, or InvalidValue if function name is invalid.
     */
    [[nodiscard]]
    expected<ALvoid*> get_proc_address(ALCdevice* device, const gsl::zstring& funcname) noexcept;

    /***
     * Retrieves the enum value for the specified enumeration name
     * @param device device to query from
     * @param enumname null terminate string of enum name
     * @return value of enumname, or InvalidValue if invalid enumname
     */
    [[nodiscard]]
    expected<std::int32_t> get_enum_value(ALCdevice* device, const gsl::zstring& enumname) noexcept;

    /***
     * Requires ALC_SOFT_pause_device
     *
     *      When paused, no contexts associated with the device will be processed or
     *      updated. Playing sources will not produce sound, have their offsets
     *      incremented, or process any more buffers, until the device is resumed.
     *      Pausing a device that is already paused is a legal no-op.
     *
     *      To resume playback of a paused device, use the function
     *
     * @param device device to pause.
     * @return nothing, or InvalidDevice if device is null, invalid, or is not a playback device.
     */
    result device_pause(ALCdevice *device) noexcept;

    /***
     * Requires ALC_SOFT_pause_device
     *
     * This will restart processing on the device -- sources will resume playing
     * sound as normal. Resuming playback on a device that is not paused is a
     * legal no-op.
     *
     * These functions are not reference counted. alcDeviceResumeSOFT only needs
     * to be called once to resume playback, regardless of how many times
     * alcDevicePauseSOFT was called.
     *
     * @param device device to resume.
     * @return nothing, or InvalidDevice if device is null, invalid, or is not a playback device.
     */
    result device_resume(ALCdevice *device) noexcept;

    [[nodiscard]]
    expected<bool> reset_device(ALCdevice* device, std::span<const ContextAttributeValue> attribute_list) noexcept ;

    [[nodiscard]]
    expected<ALCdevice*> capture_open_device(const gsl::zstring& devicename, std::uint32_t frequency, al::BufferFormat format, std::int32_t buffersize) noexcept;

    [[nodiscard]]
    expected<bool> capture_close_device(ALCdevice* device) noexcept;

    result capture_start(ALCdevice* device) noexcept;

    result capture_stop(ALCdevice* device) noexcept;

    result capture_samples(ALCdevice* device, std::span<std::byte> buffer, std::int32_t samples) noexcept;

    template<typename T>
    result capture_samples(ALCdevice* device, std::span<T> buffer) noexcept{
        return capture_samples(device, std::as_bytes(buffer), static_cast<std::size_t>(buffer.size()));
    }


    template<typename T>
    [[nodiscard]]
    expected<T> get(ALCdevice* device, DeviceAttribute attribute) noexcept = delete;

    template<>
    [[nodiscard]]
    expected<std::string> get(ALCdevice* device, DeviceAttribute attribute) noexcept;

    template<>
    [[nodiscard]]
    expected<bool> get(ALCdevice* device, DeviceAttribute attribute) noexcept;

    template<>
    [[nodiscard]]
    expected<std::int32_t> get(ALCdevice* device, DeviceAttribute attribute) noexcept;

    template<>
    [[nodiscard]]
    expected<std::int64_t> get(ALCdevice* device, DeviceAttribute attribute) noexcept;

    [[nodiscard]]
    result get(ALCdevice* device, DeviceAttribute attribute, std::span<std::int32_t> value) noexcept;

    [[nodiscard]]
    result get(ALCdevice* device, DeviceAttribute attribute, std::span<std::int64_t> value) noexcept;

    template<typename T, typename U>
    [[nodiscard]]
    expected<U> get(ALCdevice* device, DeviceAttribute attribute) {
        static constexpr auto array_size = sizeof(U) / sizeof(T);
        static_assert(array_size * sizeof(T) == sizeof(U), "Expected T to exactly fill U");
        std::array<T, array_size> value = {};
        return get(device, attribute, std::span(value)).map([value](auto result){
            return std::bit_cast<U>(value);
        });
    }

    template<typename T>
    [[nodiscard]]
    expected<T> get(ALCdevice* device, DeviceAttribute attribute, std::int32_t value) noexcept= delete;
    template<>
    [[nodiscard]]
    expected<std::string> get(ALCdevice* device, DeviceAttribute attribute, std::int32_t index) noexcept;
    template<>
    [[nodiscard]]
    expected<std::vector<std::string>> get(ALCdevice* device, DeviceAttribute attribute, std::int32_t count) noexcept;

}
#endif //VULKANWORKSPACE_ALC_H
