//
// Created by Bolt on 10/21/2023.
//

#include "openal_source.h"
#include "openal_context.h"
#include "openal_buffer.h"
#include <uul/assert.h>
#include <fmt/core.h>

namespace openal{
    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, float value) {
        alSourcef(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, double value) {
        alSourcedSOFT(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::int32_t value) {
        alSourcei(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::int64_t value) {
        alSourcei64SOFT(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, glm::vec3 value) {
        alSource3f(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, glm::dvec3 value) {
        alSource3dSOFT(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, glm::ivec3 value) {
        alSource3i(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }
    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, glm::i64vec3 value) {
        alSource3i64SOFT(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }



    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::span<const float> value) {
        alSourcefv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::span<const double> value) {
        alSourcedvSOFT(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::span<const std::int32_t> value) {
        alSourceiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set(SourceHandle::Attribute attribute, std::span<const std::int64_t> value) {
        alSourcei64vSOFT(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    template<>
    tl::expected<float, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        float value;
        alGetSourcef(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<double, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        double value;
        alGetSourcedSOFT(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<std::int32_t, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        std::int32_t value;
        alGetSourcei(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<std::int64_t, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        std::int64_t value;
        alGetSourcei64SOFT(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<glm::vec3, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        glm::vec3 value;
        alGetSource3f(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<glm::dvec3, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        glm::dvec3 value;
        alGetSource3dSOFT(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<glm::ivec3, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        glm::ivec3 value;
        alGetSource3i(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<glm::i64vec3, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute) const {
        glm::i64vec3 value;
        alGetSource3i64SOFT(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute, std::span<float> value) const {
        alGetSourcefv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute, std::span<double> value) const {
        alGetSourcedvSOFT(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute, std::span<std::int32_t> value) const {
        alGetSourceiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::get(SourceHandle::Attribute attribute, std::span<std::int64_t> value) const {
        alGetSourcei64vSOFT(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }


    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_source_relative(bool value) {
        return set(Attribute::SourceRelative, static_cast<std::int32_t>(value));
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_cone_inner_angle(float value) {
        return set(Attribute::ConeInnerAngle, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_cone_inner_angle(std::int32_t value) {
        return set(Attribute::ConeInnerAngle, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_cone_outer_angle(float value) {
        return set(Attribute::ConeOuterAngle, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_cone_outer_angle(std::int32_t value) {
        return set(Attribute::ConeOuterAngle, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_pitch(float value) {
        return set(Attribute::Pitch, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_position(glm::vec3 value) {
        return set(Attribute::Position, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_position(glm::ivec3 value) {
        return set(Attribute::Position, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_direction(glm::vec3 value) {
        return set(Attribute::Direction, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_direction(glm::ivec3 value) {
        return set(Attribute::Direction, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_velocity(glm::vec3 value) {
        return set(Attribute::Velocity, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_velocity(glm::ivec3 value) {
        return set(Attribute::Velocity, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_looping(bool value) {
        return set(Attribute::Looping, static_cast<std::int32_t>(value));
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_buffer(BufferHandle value) {
        return set(Attribute::Buffer, static_cast<std::int32_t>(value.get_handle()));
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_gain(float value) {
        return set(Attribute::Gain, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_min_gain(float value) {
        return set(Attribute::MinGain, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_max_gain(float value) {
        return set(Attribute::MaxGain, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_reference_distance(float value) {
        return set(Attribute::ReferenceDistance, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_rolloff_factor(float value) {
        return set(Attribute::RolloffFactor, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_cone_outer_gain(float value) {
        return set(Attribute::ConeOuterGain, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_maximum_distance(float value) {
        return set(Attribute::MaximumDistance, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_sec_offset(std::int32_t value) {
        return set(Attribute::SecOffset, value);
    }
    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_sec_offset(float value) {
        return set(Attribute::SecOffset, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_sample_offset(std::int32_t value) {
        return set(Attribute::SampleOffset, value);
    }
    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_sample_offset(float value) {
        return set(Attribute::SampleOffset, value);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_byte_offset(std::int32_t value) {
        return set(Attribute::ByteOffset, value);
    }
    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_byte_offset(float value) {
        return set(Attribute::ByteOffset, value);
    }




    tl::expected<bool, AlErrorState> SourceHandle::get_source_relative() const{
        return get<std::int32_t>(Attribute::SourceRelative).map([](auto value){
            return static_cast<bool>(value);
        });
    }

    tl::expected<float, AlErrorState> SourceHandle::get_cone_inner_angle() const{
        return get<float>(Attribute::ConeInnerAngle);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_cone_inner_angle_i() const{
        return get<std::int32_t>(Attribute::ConeInnerAngle);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_cone_outer_angle() const{
        return get<float>(Attribute::ConeOuterAngle);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_cone_outer_angle_i() const{
        return get<std::int32_t>(Attribute::ConeOuterAngle);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_pitch() const{
        return get<float>(Attribute::Pitch);
    }

    tl::expected<glm::vec3, AlErrorState> SourceHandle::get_position() const{
        return get<glm::vec3>(Attribute::Position);
    }

    tl::expected<glm::ivec3, AlErrorState> SourceHandle::get_position_iv() const{
        return get<glm::ivec3>(Attribute::Position);
    }

    tl::expected<glm::vec3, AlErrorState> SourceHandle::get_direction() const{
        return get<glm::vec3>(Attribute::Direction);
    }

    tl::expected<glm::ivec3, AlErrorState> SourceHandle::get_direction_iv() const{
        return get<glm::ivec3>(Attribute::Direction);
    }

    tl::expected<glm::vec3, AlErrorState> SourceHandle::get_velocity() const{
        return get<glm::vec3>(Attribute::Velocity);
    }

    tl::expected<glm::ivec3, AlErrorState> SourceHandle::get_velocity_iv() const{
        return get<glm::ivec3>(Attribute::Velocity);
    }

    tl::expected<bool, AlErrorState> SourceHandle::get_looping() const{
        return get<std::int32_t>(Attribute::Looping).map([](auto value){
            return static_cast<bool>(value);
        });
    }

    tl::expected<BufferHandle, AlErrorState> SourceHandle::get_buffer() const{
        return get<std::int32_t>(Attribute::Buffer).map([](auto value){
            return BufferHandle(static_cast<ALuint>(value));
        });
    }

    tl::expected<float, AlErrorState> SourceHandle::get_gain() const{
        return get<float>(Attribute::Gain);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_min_gain() const{
        return get<float>(Attribute::MinGain);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_max_gain() const{
        return get<float>(Attribute::MaxGain);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_reference_distance() const{
        return get<float>(Attribute::ReferenceDistance);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_rolloff_factor() const{
        return get<float>(Attribute::RolloffFactor);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_cone_outer_gain() const{
        return get<float>(Attribute::ConeOuterGain);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_maximum_distance() const{
        return get<float>(Attribute::MaximumDistance);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_sec_offset() const{
        return get<float>(Attribute::SecOffset);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_sec_offset_i() const{
        return get<std::int32_t>(Attribute::SecOffset);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_sample_offset() const{
        return get<float>(Attribute::SampleOffset);
    }
    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_sample_offset_i() const{
        return get<std::int32_t>(Attribute::SampleOffset);
    }


    tl::expected<float, AlErrorState> SourceHandle::get_byte_offset() const{
        return get<float>(Attribute::ByteOffset);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_byte_offset_i() const{
        return get<std::int32_t>(Attribute::ByteOffset);
    }

    tl::expected<SourceHandle::State, AlErrorState> SourceHandle::get_source_state() const {
        return get<std::int32_t>(Attribute::SourceState).map([](auto value){
            return static_cast<State>(value);
        });
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_buffers_queued() const {
        return get<std::int32_t>(Attribute::BuffersQueued);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_buffers_processed() const {
        return get<std::int32_t>(Attribute::BuffersProcessed);
    }

    tl::expected<SourceHandle::Type, AlErrorState> SourceHandle::get_source_type() const {
        return get<std::int32_t>(Attribute::SourceState).map([](auto value){
            return static_cast<Type>(value);
        });
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_stereo_angles(SourceHandle::StereoAngles value) {
        const auto temp_value = std::bit_cast<std::array<float, 2>>(value);
        return set(Attribute::StereoAngles, std::span<const float>(temp_value));
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_source_radius(float value) {
        return set(Attribute::SourceRadius, value);
    }

    tl::expected<SourceHandle::StereoAngles, AlErrorState> SourceHandle::get_stereo_angles() const {
        std::array<float, 2> value = {};
        return get(Attribute::StereoAngles, std::span(value)).map([value](auto result){
            return std::bit_cast<SourceHandle::StereoAngles>(value);
        });
    }

    tl::expected<float, AlErrorState> SourceHandle::get_source_radius() const {
        return get<float>(Attribute::SourceRadius);
    }

    tl::expected<SourceHandle::SampleOffsetLatency, AlErrorState> SourceHandle::get_sample_offset_latency() const {
        return get<std::int64_t, SampleOffsetLatency>(Attribute::SampleOffsetLatency);
    }

    tl::expected<SourceHandle::SecOffsetLatency, AlErrorState> SourceHandle::get_sec_offset_latency() const {
        return get<std::int64_t, SecOffsetLatency>(Attribute::SecOffsetLatency);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_byte_length() const {
        return get<std::int32_t>(Attribute::ByteLength);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_sample_length() const {
        return get<std::int32_t>(Attribute::SampleLength);
    }

    tl::expected<float, AlErrorState> SourceHandle::get_sec_length() const {
        return get<std::int32_t>(Attribute::SecLength);
    }

    tl::expected<std::int32_t, AlErrorState> SourceHandle::get_source_resampler() const {
        return get<std::int32_t>(Attribute::SourceResampler);
    }

    tl::expected<tl::monostate, AlErrorState> SourceHandle::set_spatialize_state(SpatializeState value) {
        return set(Attribute::SourceSpatialize, static_cast<std::int32_t>(value));
    }

    tl::expected<SourceHandle::SpatializeState, AlErrorState> SourceHandle::get_spatialize_state() const {
        return get<std::int32_t>(Attribute::SourceState).map([](auto value){
            return static_cast<SpatializeState>(value);
        });
    }

    tl::expected<SourceHandle::SampleOffsetClock, AlErrorState> SourceHandle::get_sample_offset_clock() const {
        return get<std::int64_t, SampleOffsetClock>(Attribute::SampleOffsetClock);
    }

    tl::expected<SourceHandle::SecOffsetClock, AlErrorState> SourceHandle::get_sec_offset_clock() const {
        return get<std::int64_t, SecOffsetClock>(Attribute::SecOffsetClock);
    }

    tl::expected<tl::monostate, AlErrorState>
    SourceHandle::set_direct_channels_state(SourceHandle::DirectChannelsState value) {
        return set(Attribute::DirectChannels, static_cast<std::int32_t>(value));
    }

    tl::expected<SourceHandle::DirectChannelsState, AlErrorState> SourceHandle::get_direct_channels_state() const {
        return get<std::int32_t>(Attribute::DirectChannels).map([](auto value){
            return static_cast<DirectChannelsState>(value);
        });
    }

    bool SourceHandle::is_valid() const {
        return static_cast<bool>(alIsSource(get_handle()));
    }


    Source::~Source() {
        if (*this != null_source_handle) {
            Context::get_current_thread_context().clear_errors();
            alDeleteSources(1, &get_handle());
            auto result = Context::get_current_thread_context().get_error();
            UUL_ASSERT(result.has_value(), fmt::format("{}", to_string(result.error())).c_str());
        }
    }

    Source::Source(Source &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
    }

    Source &Source::operator=(Source &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }

    tl::expected<Source, AlErrorState> Source::generate() {
        Context::get_current_thread_context().clear_errors();
        ALuint source;
        alGenSources(1, &source);
        if (auto result =  Context::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return Source(source);
    }

    tl::expected<std::vector<Source>, AlErrorState> Source::generate_many(ALsizei count) {
        ContextHandle::get_current_thread_context().clear_errors();
        std::vector<Source> sources(count);
        alGenSources(count, reinterpret_cast<ALuint *>(sources.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return sources;
    }


    tl::expected<tl::monostate, AlErrorState> Source::play() const {
        ContextHandle::get_current_thread_context().clear_errors();
        alSourcePlay(get_handle());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<std::vector<BufferHandle>, AlErrorState> Source::unqueue_buffers(ALsizei size) {
        std::vector<BufferHandle> buffer_handles(size);
        ContextHandle::get_current_thread_context().clear_errors();
        alSourceUnqueueBuffers(get_handle(), size, reinterpret_cast<ALuint *>(buffer_handles.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return buffer_handles;
    }

    tl::expected<tl::monostate, AlErrorState>
    Source::queue_buffers(std::span<BufferHandle> buffer_handles) {
        ContextHandle::get_current_thread_context().clear_errors();
        //TODO handle handles being too big?
        alSourceQueueBuffers(get_handle(), static_cast<ALsizei>(buffer_handles.size()), reinterpret_cast<ALuint *>(buffer_handles.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> Source::queue_buffers(std::span<Buffer> buffers) {
        return queue_buffers(std::span(reinterpret_cast<BufferHandle*>(buffers.data()), buffers.size()));
    }

    tl::expected<tl::monostate, AlErrorState> Source::clear_buffers() {
        return set_buffer(null_buffer_handle);
    }

}