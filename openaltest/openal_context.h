//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_CONTEXT_H
#define VULKANWORKSPACE_OPENAL_CONTEXT_H

#include "openal_util.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>



namespace openal{

    class DeviceHandle;
    class Listener;
    class ContextHandle : public Handle<ALCcontext *, ContextHandle> {
    public:

        //TODO handle attribute list stuff? ALC_SOFT_HRTF still needs attribute list handled.
        //  Need seperate list?
        using Handle::Handle;


        [[nodiscard]]
        tl::expected<DeviceHandle, AlcErrorState> get_device() const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get_error() const;

        void clear_errors() const;

        [[nodiscard]]
        static bool clear_current_thread_context();

        [[nodiscard]]
        tl::expected<bool, AlcErrorState> set_current_thread_context() const;

        [[nodiscard]]
        static ContextHandle get_current_thread_context();

        [[nodiscard]]
        Listener listener();

        [[nodiscard]]
        const Listener listener() const;

        enum class Attribute : ALenum {
            DopplerFactor = AL_DOPPLER_FACTOR,
            SpeedOfSound = AL_SPEED_OF_SOUND,
            DistanceModel = AL_DISTANCE_MODEL,

            NumResamplers = AL_NUM_RESAMPLERS_SOFT, //AL_SOFT_source_resampler int
            DefaultResampler = AL_DEFAULT_RESAMPLER_SOFT, //int
            ResamplerName = AL_RESAMPLER_NAME_SOFT, //String at index


            GainLimit = AL_GAIN_LIMIT_SOFT, //AL_SOFT_gain_clamp_ex float
            DeferredUpdates = AL_DEFERRED_UPDATES_SOFT, //AL_SOFT_deferred_updates
        };

        enum class DistanceModel : ALenum {
            InverseDistance = AL_INVERSE_DISTANCE,
            InverseDistanceClamped = AL_INVERSE_DISTANCE_CLAMPED,
            LinearDistance = AL_LINEAR_DISTANCE,
            LinearDistanceClamped = AL_LINEAR_DISTANCE_CLAMPED,
            ExponentDistance = AL_EXPONENT_DISTANCE,
            ExponentDistanceClamped = AL_EXPONENT_DISTANCE_CLAMPED
        };


        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlErrorState> get(Attribute attribute) const = delete;

        template<>
        [[nodiscard]]
        tl::expected<bool, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<float, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<double, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<std::string, AlErrorState> get(Attribute attribute) const;

        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlErrorState> get(Attribute attribute, std::int32_t value) const = delete;
        template<>
        [[nodiscard]]
        tl::expected<std::string, AlErrorState> get(Attribute attribute, std::int32_t index) const;
        template<>
        [[nodiscard]]
        tl::expected<std::vector<std::string>, AlErrorState> get(Attribute attribute, std::int32_t count) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_distance_model(DistanceModel value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_doppler_factor(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_speed_of_sound_factor(float value_m_s);

        [[nodiscard]]
        tl::expected<DistanceModel, AlErrorState> get_distance_model() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_doppler_factor() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_speed_of_sound_factor() const;

        //extensions
        [[nodiscard]]
        tl::expected<bool, AlErrorState> get_are_deferred_updates() const;

        void defer_updates();

        void process_updates();


        [[nodiscard]]
        tl::expected<float, AlErrorState> get_gain_limit() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_default_resampler() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_num_resamplers() const;

        [[nodiscard]]
        tl::expected<std::vector<std::string>, AlErrorState> get_resampler_names() const;

    private:
    };

    static constexpr auto null_context_handle = ContextHandle();

    class Context : public ContextHandle {
    public:
        using ContextHandle::ContextHandle;

        explicit Context(ALCcontext *context_ptr);

        ~Context();

        Context(Context &&rhs) noexcept;

        Context &operator=(Context &&rhs) noexcept;

        Context(const Context &rhs) = delete;

        Context &operator=(const Context &rhs) = delete;

    private:
    };

}
#endif //VULKANWORKSPACE_OPENAL_CONTEXT_H
