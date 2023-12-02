//
// Created by Bolt on 10/21/2023.
//

#include "openal_context.h"
#include "openal_device.h"
#include "openal_listener.h"
#include <uul/assert.h>
#include <fmt/core.h>

namespace openal{
    tl::expected<DeviceHandle, AlcErrorState> ContextHandle::get_device() const {
        set_current_thread_context().value();
        auto device_handle = DeviceHandle(alcGetContextsDevice(get_handle()));
        if (auto error = device_handle.get_error(); !error.has_value()) {
            return tl::make_unexpected(error.error());
        }
        return device_handle;
    }

    tl::expected<tl::monostate, AlErrorState> ContextHandle::get_error() const {
        set_current_thread_context().value();
        if (auto result = static_cast<AlErrorState>(alGetError());
                result != AlErrorState::NoError) {
            return tl::make_unexpected(result);
        }
        return tl::monostate();
    }

    void ContextHandle::clear_errors() const {
        set_current_thread_context().value();
        alGetError();
    }

    bool ContextHandle::clear_current_thread_context() {
        auto result = alcSetThreadContext(null_context_handle.get_handle());
        return result;
    }

    tl::expected<bool, AlcErrorState> ContextHandle::set_current_thread_context() const {
        auto result = alcSetThreadContext(get_handle());
        auto device_handle = DeviceHandle(alcGetContextsDevice(get_handle()));
        if (auto error = device_handle.get_error(); !error.has_value()) {
            return tl::make_unexpected(error.error());
        }
        if (auto error = device_handle.get_error(); !error.has_value()) {
            return tl::make_unexpected(error.error());
        }
        return result;
    }


    template<>
    tl::expected<bool, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute) const {
        set_current_thread_context().value();
        auto temp_value = alGetBoolean(static_cast<ALenum>(attribute));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return static_cast<bool>(temp_value);
    }

    template<>
    tl::expected<double, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute) const {
        set_current_thread_context().value();
        auto temp_value = alGetDouble(static_cast<ALenum>(attribute));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return temp_value;
    }

    template<>
    tl::expected<float, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute) const {
        set_current_thread_context().value();
        auto temp_value = alGetFloat(static_cast<ALenum>(attribute));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return temp_value;
    }

    template<>
    tl::expected<std::int32_t, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute) const {
        set_current_thread_context().value();
        auto temp_value = alGetInteger(static_cast<ALenum>(attribute));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return temp_value;
    }

    template<>
    tl::expected<std::string, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute) const {
        set_current_thread_context().value();
        auto temp_value = alGetString(static_cast<ALenum>(attribute));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return temp_value;
    }

    template<>
    tl::expected<std::string, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute, std::int32_t index) const {
        set_current_thread_context().value();
        auto temp_value = alGetStringiSOFT(static_cast<ALenum>(attribute), index);
        return get_error().map([&temp_value](auto temp){
            return temp_value;
        });
    }

    template<>
    tl::expected<std::vector<std::string>, AlErrorState>
    ContextHandle::get(ContextHandle::Attribute attribute, std::int32_t count) const {
        std::vector<std::string> value;
        value.reserve(count);
        for(std::int32_t i = 0; i < count; ++i){
            auto temp_value = alGetStringiSOFT(static_cast<ALenum>(attribute), i);
            if (auto result = get_error(); !result.has_value()) {
                return tl::make_unexpected(result.error());
            }
            value.emplace_back(temp_value);
        }
        return value;
    }

    tl::expected<tl::monostate, AlErrorState> ContextHandle::set_distance_model(ContextHandle::DistanceModel value) {
        set_current_thread_context().value();
        alDistanceModel(static_cast<ALenum>(value));
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> ContextHandle::set_doppler_factor(float value) {
        set_current_thread_context().value();
        alDopplerFactor(value);
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> ContextHandle::set_speed_of_sound_factor(float value_m_s) {
        set_current_thread_context().value();
        alSpeedOfSound(value_m_s);
        if (auto result =get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<ContextHandle::DistanceModel, AlErrorState> ContextHandle::get_distance_model() const{
        return get<std::int32_t>(Attribute::DistanceModel).map([](auto value){
            return static_cast<ContextHandle::DistanceModel>(value);
        });
    }

    tl::expected<float, AlErrorState> ContextHandle::get_doppler_factor() const{
        return get<float>(Attribute::DopplerFactor);
    }

    tl::expected<float, AlErrorState> ContextHandle::get_speed_of_sound_factor() const{
        return get<float>(Attribute::SpeedOfSound);
    }


    tl::expected<bool, AlErrorState> ContextHandle::get_are_deferred_updates() const {
        return get<bool>(Attribute::DeferredUpdates);
    }

    void ContextHandle::defer_updates() {
        set_current_thread_context().value();
        alDeferUpdatesSOFT();
    }

    void ContextHandle::process_updates() {
        set_current_thread_context().value();
        alProcessUpdatesSOFT();
    }

    ContextHandle ContextHandle::get_current_thread_context() {
        return ContextHandle{alcGetThreadContext()};
    }

    Listener ContextHandle::listener() {
        return Listener{*this};
    }

    const Listener ContextHandle::listener() const {
        return Listener{*this};
    }


    tl::expected<float, AlErrorState> ContextHandle::get_gain_limit() const {
        return get<float>(Attribute::GainLimit);
    }

    tl::expected<std::int32_t, AlErrorState> ContextHandle::get_default_resampler() const {
        return get<std::int32_t>(Attribute::DefaultResampler);
    }

    tl::expected<std::int32_t, AlErrorState> ContextHandle::get_num_resamplers() const {
        return get<std::int32_t>(Attribute::NumResamplers);
    }

    tl::expected<std::vector<std::string>, AlErrorState> ContextHandle::get_resampler_names() const {
        return get_num_resamplers().and_then([this](auto resampler_count) {
            return get<std::vector<std::string>>(Attribute::ResamplerName,  resampler_count);
        });
    }


    Context::Context(ALCcontext *context_ptr) : ContextHandle(context_ptr) {
        auto prev_context = alcGetThreadContext();
        set_current_thread_context().value();
        for (const auto &extension: al_required_extensions) {
            UUL_ASSERT(alIsExtensionPresent(extension),
                       fmt::format("Couldn't find AL extension {}", extension).c_str());
        }
        alcSetThreadContext(prev_context);
    }

    Context::~Context() {
        if (*this != null_context_handle) {
            UUL_ASSERT(clear_current_thread_context(), fmt::format("Unable to clear current thread context?").c_str());
            alcDestroyContext(get_handle());
            auto device = get_device().value();
            auto result = device.get_error();
            UUL_ASSERT(result.has_value(), fmt::format("{}", to_string(result.error())).c_str());
        }
    }

    Context::Context(Context &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
    }

    Context &Context::operator=(Context &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }


}