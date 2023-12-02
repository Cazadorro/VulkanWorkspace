//
// Created by Bolt on 10/21/2023.
//
#define AL_ALEXT_PROTOTYPES
#include "openal/alc.h"
#include "alc.h"


#include "AL/alext.h"
namespace openal::alc{
    std::string to_string(ErrorState error_state) noexcept{
        switch (error_state) {
            case ErrorState::NoError:
                return "NoError";
            case ErrorState::InvalidDevice:
                return "InvalidDevice";
            case ErrorState::InvalidContext:
                return "InvalidContext";
            case ErrorState::InvalidEnum:
                return "InvalidEnum";
            case ErrorState::InvalidValue:
                return "InvalidValue";
            case ErrorState::OutOfMemory:
                return "OutOfMemory";
            default:
                return "Unknown";
        }
    }


    expected<ALCcontext *>create_context(ALCdevice *device, std::span<const ContextAttributeValue> attribute_list) noexcept{
        return get_error(device).map([=](auto){
            return alcCreateContext(device, reinterpret_cast<const std::int32_t*>(attribute_list.data()));
        });
    }

    result make_context_current(ALCcontext *context) noexcept{
        alcMakeContextCurrent(context);
        return get_error(context);
    }

    result process_context(ALCcontext *context) noexcept{
        alcProcessContext(context);
        return get_error(context);
    }

    result suspend_context(ALCcontext *context) noexcept{
        alcSuspendContext(context);
        return get_error(context);
    }

    result destroy_context(ALCcontext *context) noexcept{
        alcDestroyContext(context);
        return get_error(context);
    }

    ALCcontext* get_current_context() noexcept{
        return alcGetCurrentContext();
    }

    expected<ALCdevice*> get_contexts_device(ALCcontext *context) noexcept{
        auto result = alcGetContextsDevice(context);
        return get_error(context).map([=](auto){
            return result;
        });
    }

    ALCdevice* open_device(const gsl::zstring& devicename) noexcept{
        return alcOpenDevice(devicename);
    }

    expected<bool> close_device(ALCdevice *device) noexcept{
        auto result = alcCloseDevice(device);
        return get_error(device).map([=](auto){
            return static_cast<bool>(result);
        });
    }

    result get_error(ALCdevice *device) noexcept{
        if (auto result = static_cast<ErrorState>(alcGetError(device)); result != ErrorState::NoError) {
            return tl::make_unexpected(result);
        }
        return tl::monostate();
    }

    result get_error(ALCcontext *context) noexcept{
        auto device = alcGetContextsDevice(context);
        return get_error(device);
    }

    result get_current_context_error() noexcept{
        return get_error(get_current_context());
    }

    expected<bool> is_extension_present(ALCdevice* device, const gsl::zstring& extname) noexcept{
        auto result = alcIsExtensionPresent(device, extname);
        return get_error(device).map([=](auto){
            return static_cast<bool>(result);
        });
    }

    expected<ALvoid*> get_proc_address(ALCdevice* device, const gsl::zstring& funcname) noexcept{
        auto result = alcGetProcAddress(device, funcname);
        return get_error(device).map([=](auto){
            return result;
        });
    }

    expected<std::int32_t> get_enum_value(ALCdevice* device, const gsl::zstring& enumname) noexcept{
        auto result = alcGetEnumValue(device, enumname);
        return get_error(device).map([=](auto){
            return static_cast<std::int32_t>(result);
        });
    }

    result device_pause(ALCdevice *device) noexcept{
        alcDevicePauseSOFT(device);
        return get_error(device);
    }

    result device_resume(ALCdevice *device) noexcept{
        alcDeviceResumeSOFT(device);
        return get_error(device);
    }

    expected<bool> reset_device(ALCdevice* device, std::span<const ContextAttributeValue> attribute_list) noexcept{
        auto result = alcResetDeviceSOFT(device, reinterpret_cast<const std::int32_t*>(attribute_list.data()));
        return get_error(device).map([=](auto){
            return static_cast<bool>(result);
        });
    }

    expected<ALCdevice*> capture_open_device(const gsl::zstring& devicename, std::uint32_t frequency, al::BufferFormat format, std::int32_t buffersize) noexcept{
        auto device = alcCaptureOpenDevice(devicename, frequency, static_cast<ALenum>(format), buffersize);
        return get_error(device).map([=](auto){
            return device;
        });
    }

    expected<bool> capture_close_device(ALCdevice* device) noexcept{
        auto result = alcCaptureCloseDevice(device);
        return get_error(device).map([=](auto){
            return static_cast<bool>(result);
        });
    }

    result capture_start(ALCdevice* device) noexcept{
        alcCaptureStart(device);
        return get_error(device);
    }

    result capture_stop(ALCdevice* device) noexcept{
        alcCaptureStop(device);
        return get_error(device);
    }

    result capture_samples(ALCdevice* device, std::span<std::byte> buffer, std::int32_t samples) noexcept{
        alcCaptureSamples(device, buffer.data(), samples);
        return get_error(device);
    }

    template<>
    expected<std::string> get(ALCdevice* device, DeviceAttribute attribute) noexcept{
        auto result = alcGetString(device, static_cast<ALenum>(attribute));
        return get_error(device).map([=](auto){
            return std::string(result);
        });
    }

    template<>
    expected<bool> get(ALCdevice* device, DeviceAttribute attribute) noexcept{
        return get<std::int32_t>(device, attribute).map([=](auto value){
            return static_cast<bool>(value);
        });
    }

    template<>
    expected<std::int32_t> get(ALCdevice* device, DeviceAttribute attribute) noexcept{
        std::int32_t result;
        alcGetIntegerv(device, static_cast<ALenum>(attribute), 1, &result);
        return get_error(device).map([=](auto){
            return result;
        });
    }

    template<>
    expected<std::int64_t> get(ALCdevice* device, DeviceAttribute attribute) noexcept{
        std::int64_t result;
        alcGetInteger64vSOFT(device, static_cast<ALenum>(attribute), 1, &result);
        return get_error(device).map([=](auto){
            return result;
        });
    }

    result get(ALCdevice* device, DeviceAttribute attribute, std::span<std::int32_t> value) noexcept{
        alcGetIntegerv(device, static_cast<ALenum>(attribute), static_cast<std::int32_t>(value.size()), value.data());
        return get_error(device);
    }

    result get(ALCdevice* device, DeviceAttribute attribute, std::span<std::int64_t> value) noexcept{
        alcGetInteger64vSOFT(device, static_cast<ALenum>(attribute), static_cast<std::int32_t>(value.size()), value.data());
        return get_error(device);
    }

    template<>
    expected<std::string> get(ALCdevice* device, DeviceAttribute attribute, std::int32_t index) noexcept{
        auto result = alcGetStringiSOFT(device, static_cast<ALenum>(attribute), index);
        return get_error(device).map([=](auto){
            return std::string(result);
        });
    }
    template<>
    [[nodiscard]]
    expected<std::vector<std::string>> get(ALCdevice* device, DeviceAttribute attribute, std::int32_t count) noexcept{
        std::vector<std::string> value;
        value.reserve(count);
        for(std::int32_t i = 0; i < count; ++i){
            auto temp_value = alcGetStringiSOFT(device, static_cast<ALenum>(attribute), i);
            if (auto result = get_error(device); !result.has_value()) {
                return tl::make_unexpected(result.error());
            }
            value.emplace_back(temp_value);
        }
        return value;
    }


}

