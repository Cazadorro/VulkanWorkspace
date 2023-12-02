//
// Created by Bolt on 10/21/2023.
//

#include "openal_device.h"
#include "openal_context_attribute_builder.h"
#include "openal_context.h"
#include <uul/assert.h>
#include <fmt/core.h>

namespace openal{
    alc::result DeviceHandle::get_error() const {
        return alc::get_error(get_handle());
    }


    alc::expected<Context> DeviceHandle::create_context(std::span<const std::int32_t> attribute_list) const{
        auto openal_context = alcCreateContext(get_handle(), attribute_list.data());
        if (auto error = get_error(); !error.has_value()) {
            return tl::make_unexpected(error.error());
        }
        return Context(openal_context);
    }

    alc::expected<Context>
    DeviceHandle::create_context(const InitAttributeBuilder &attribute_list) const {
        return create_context(attribute_list.to_span());
    }



    alc::result DeviceHandle::pause() {
        return alc::device_pause(get_handle());
    }

    alc::result DeviceHandle::resume() {
        return alc::device_resume(get_handle());
    }


    tl::expected<bool, AlcErrorState> DeviceHandle::reset_device(std::span<const std::int32_t> attribute_list) {
        auto reset_successful = alcResetDeviceSOFT(get_handle(), attribute_list.data());
        if (auto error = get_error(); !error.has_value()) {
            return tl::make_unexpected(error.error());
        }
        return reset_successful;
    }

    tl::expected<bool, AlcErrorState> DeviceHandle::reset_device(const InitAttributeBuilder &attribute_list) {
        return reset_device(attribute_list.to_span());
    }

    tl::expected<ALvoid*, AlcErrorState>DeviceHandle::get_proc_address(const ALCchar *funcname) const {
        auto result = alcGetProcAddress(get_handle(), funcname);
        return get_error().map([result](auto temp){
            return result;
        });
    }

    tl::expected<ALCenum , AlcErrorState> DeviceHandle::get_enum_value(const ALCchar *enumname) const {
        auto result = alcGetEnumValue(get_handle(), enumname);
        return get_error().map([result](auto temp){
            return result;
        });
    }

    template<>
    tl::expected<std::string, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute) const {
        auto cstring = alcGetString(get_handle(), static_cast<ALenum>(attribute));
        return get_error().map([cstring](auto temp){
            return cstring;
        });
    }

    tl::expected<tl::monostate, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute, std::span<std::int32_t> value) const {
        alcGetIntegerv(get_handle(), static_cast<ALenum>(attribute), static_cast<ALsizei>(value.size()), value.data());
        return get_error();
    }

    tl::expected<tl::monostate, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute, std::span<std::int64_t> value) const {
        alcGetInteger64vSOFT(get_handle(), static_cast<ALenum>(attribute), static_cast<ALsizei>(value.size()), value.data());
        return get_error();
    }

    template<>
    tl::expected<std::int32_t, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute) const {
        std::int32_t value;
        alcGetIntegerv(get_handle(), static_cast<ALenum>(attribute), 1, &value);
        return get_error().map([value](auto temp){
            return value;
        });
    }

    template<>
    tl::expected<std::int64_t, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute) const {
        std::int64_t value;
        alcGetInteger64vSOFT(get_handle(), static_cast<ALenum>(attribute), 1, &value);
        return get_error().map([value](auto temp){
            return value;
        });
    }

    template<>
    tl::expected<std::string, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute, std::int32_t index) const {
        auto cstring = alcGetStringiSOFT(get_handle(), static_cast<ALenum>(attribute), index);
        return get_error().map([cstring](auto temp){
            return cstring;
        });
    }

    template<>
    tl::expected<std::vector<std::string>, AlcErrorState>
    DeviceHandle::get(DeviceHandle::Attribute attribute, std::int32_t count) const {
        std::vector<std::string> value;
        value.reserve(count);
        for(std::int32_t i = 0; i < count; ++i){
            auto temp_value = alcGetStringiSOFT(get_handle(), static_cast<ALenum>(attribute), i);
            if (auto result = get_error(); !result.has_value()) {
                return tl::make_unexpected(result.error());
            }
            value.emplace_back(temp_value);
        }
        return value;
    }


    tl::expected<bool, AlcErrorState> DeviceHandle::get_hrtf() const {
        return get<std::int32_t>(Attribute::Hrtf).map([](auto value){
            return static_cast<bool>(value);
        });
    }

    tl::expected<DeviceHandle::HrtfStatus, AlcErrorState> DeviceHandle::get_hrtf_status() const {
        return get<std::int32_t>(Attribute::HrtfStatus).map([](auto value){
            return static_cast<HrtfStatus>(value);
        });
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_num_hrtf_specifiers() const {
        return get<std::int32_t>(Attribute::NumHrtfSpecifiers);
    }

    tl::expected<std::vector<std::string>, AlcErrorState> DeviceHandle::get_hrtf_specifiers() const {
        return get_num_hrtf_specifiers().and_then([this](auto specifier_count) {
            return get<std::vector<std::string>>(Attribute::HrtfSpecifier,  specifier_count);
        });
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_major_version() const {
        return get<std::int32_t>(Attribute::MajorVersion);
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_minor_version() const {
        return get<std::int32_t>(Attribute::MinorVersion);
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_attribute_size() const {
        return get<std::int32_t>(Attribute::AttributesSize);
    }

    tl::expected<std::vector<std::int32_t>, AlcErrorState> DeviceHandle::get_all_attributes() const {
        return get_attribute_size().and_then([this](auto attribute_count) {
            std::vector<std::int32_t> attributes(attribute_count);
            return get(Attribute::AllAttributes,  std::span(attributes)).and_then([&attributes](auto result)
                                                                                          -> tl::expected<std::vector<std::int32_t>, AlcErrorState>{
                return attributes;
            });
        });
    }

    tl::expected<bool, AlcErrorState> DeviceHandle::get_output_limiter() const {
        return get<std::int32_t>(Attribute::OutputLimiter).map([](auto value){
            return static_cast<bool>(value);
        });
    }

    tl::expected<std::int64_t, AlcErrorState> DeviceHandle::get_device_clock() const {
        return get<std::int32_t>(Attribute::DeviceClock);
    }

    tl::expected<std::int64_t, AlcErrorState> DeviceHandle::get_device_latency() const {
        return get<std::int32_t>(Attribute::DeviceLatency);
    }

    tl::expected<DeviceHandle::ClockLatency, AlcErrorState> DeviceHandle::get_device_clock_latency() const {
        return  get<std::int32_t, DeviceHandle::ClockLatency>(Attribute::DeviceClockLatency);
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_efx_major_version() const {
        return get<std::int32_t>(Attribute::EfxMajorVersion);
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_efx_minor_version() const {
        return get<std::int32_t>(Attribute::EfxMinorVersion);
    }

    tl::expected<std::int32_t, AlcErrorState> DeviceHandle::get_max_auxiliary_sends() const {
        return get<std::int32_t>(Attribute::MaxAuxiliarySends);
    }


    openal::Device::Device(ALCdevice *device_ptr) : DeviceHandle(device_ptr) {
        for (const auto &extension: alc_required_extensions) {
            UUL_ASSERT(alcIsExtensionPresent(get_handle(), extension),
                       fmt::format("Couldn't find ALC extension {}", extension).c_str());
        }
    }

    std::optional<openal::Device> openal::Device::from_name(const char *device_name) {
        Device device(alcOpenDevice(device_name));
//    UUL_ASSERT(device.m_device_ptr != nullptr, fmt::format("Could not construct device from name {}", device_name).c_str());
        if (device == null_device_handle) {
            return std::nullopt;
        } else {
            return device;
        }
    }

    std::optional<openal::Device> openal::Device::from_name(std::string &device_name) {
        return from_name(device_name.c_str());
    }

    std::optional<openal::Device> openal::Device::from_default() {
        //nullptr = default device according to documentation
        Device device(alcOpenDevice(nullptr));
        if (device == null_device_handle) {
            return std::nullopt;
        } else {
            return device;
        }
    }

    openal::Device::~Device() {
        if (*this != null_device_handle) {
            auto closed = alcCloseDevice(get_handle());
            UUL_ASSERT(closed,
                       fmt::format("Could not close device, error # {}", to_string(get_error().error())).c_str());
        }
    }


    openal::Device::Device(openal::Device &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());

    }

    openal::Device &openal::Device::operator=(openal::Device &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }

}