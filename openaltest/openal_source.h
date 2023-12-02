//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_SOURCE_H
#define VULKANWORKSPACE_OPENAL_SOURCE_H

#include "openal_util.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>

namespace openal{
    class BufferHandle;
    class Buffer;
    class SourceHandle : public Handle<ALuint, SourceHandle> {
    public:
        using Handle::Handle;

        enum class Attribute : ALenum {
            SourceRelative = AL_SOURCE_RELATIVE, //Bool
            ConeInnerAngle = AL_CONE_INNER_ANGLE, //0 - 360 int,float
            ConeOuterAngle = AL_CONE_INNER_ANGLE, //0 - 360 int,float
            Pitch = AL_PITCH, // Float
            Position = AL_POSITION, // Int3/Float3 Right Handed! Like OpenGL
            Direction = AL_DIRECTION,
            Velocity = AL_VELOCITY,
            Looping = AL_LOOPING, // bool
            Buffer = AL_BUFFER, // ALuint
            Gain = AL_GAIN, // float
            MinGain = AL_MIN_GAIN,
            MaxGain = AL_MAX_GAIN,
//            Orientation = AL_ORIENTATION, // float 6
            SourceState = AL_SOURCE_STATE, // Read Only, Alint, 4 states
            BuffersQueued = AL_BUFFERS_QUEUED, // Read only int
            BuffersProcessed = AL_BUFFERS_PROCESSED, // read only int
            ReferenceDistance = AL_REFERENCE_DISTANCE, // Float
            RolloffFactor = AL_ROLLOFF_FACTOR, //float
            ConeOuterGain = AL_CONE_OUTER_GAIN, //float
            MaximumDistance = AL_MAX_DISTANCE, // float
            SecOffset = AL_SEC_OFFSET, //int
            SampleOffset = AL_SAMPLE_OFFSET,
            ByteOffset = AL_BYTE_OFFSET,
            SourceType = AL_SOURCE_TYPE, // readonly int 3 types

            //extensions
            StereoAngles = AL_STEREO_ANGLES, // AL_EXT_STEREO_ANGLES, two floats
            SourceRadius = AL_SOURCE_RADIUS, // AL_EXT_SOURCE_RADIUS, float

            DirectChannels = AL_DIRECT_CHANNELS_SOFT, //AL_SOFT_direct_channels and AL_SOFT_direct_channels_remix

            SampleOffsetLatency = AL_SAMPLE_OFFSET_LATENCY_SOFT, // AL_SOFT_source_latency
            SecOffsetLatency = AL_SEC_OFFSET_LATENCY_SOFT,

            ByteLength = AL_BYTE_LENGTH_SOFT, //AL_SOFT_source_length int
            SampleLength = AL_SAMPLE_LENGTH_SOFT, //int
            SecLength = AL_SEC_LENGTH_SOFT,  //float

            SourceResampler = AL_SOURCE_RESAMPLER_SOFT, //AL_SOFT_source_resampler int

            SourceSpatialize = AL_SOURCE_SPATIALIZE_SOFT, // AL_SOFT_source_spatialize enum

            SampleOffsetClock = AL_SAMPLE_OFFSET_CLOCK_SOFT, // ALC_SOFT_device_clock  dual int
            SecOffsetClock = AL_SEC_OFFSET_CLOCK_SOFT, // dual double

            //AL_SOFT_direct_channels_remix
        };
        enum class State : ALenum {
            Initial = AL_INITIAL,
            Playing = AL_PLAYING,
            Paused = AL_PAUSED,
            Stopped = AL_STOPPED
        };

        enum class Type : ALenum {
            Static = AL_STATIC,
            Streaming = AL_STREAMING,
            Undetermined = AL_UNDETERMINED
        };


        [[nodiscard]]
        bool is_valid() const;
        template<typename T>
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, T value) = delete;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, double value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::int64_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::dvec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::i64vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const float> value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const double> value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const std::int32_t> value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const std::int64_t> value);

        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlErrorState> get(Attribute attribute) const = delete;

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
        tl::expected<std::int64_t, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<glm::dvec3, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get(Attribute attribute) const;

        template<>
        [[nodiscard]]
        tl::expected<glm::i64vec3, AlErrorState> get(Attribute attribute) const;

        template<typename T>
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<T> value) const = delete;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<float> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<double> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<std::int32_t> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<std::int64_t> value) const;

        template<typename T, typename U>
        [[nodiscard]]
        tl::expected<U, AlErrorState> get(Attribute attribute) const{
            static constexpr auto array_size = sizeof(U) / sizeof(T);
            static_assert(array_size * sizeof(T) == sizeof(U), "Expected T to exactly fill U");
            std::array<T, array_size> value = {};
            return get(attribute, std::span(value)).map([value](auto result){
                return std::bit_cast<U>(value);
            });
        }

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_source_relative(bool value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_cone_inner_angle(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_cone_inner_angle(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_cone_outer_angle(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_cone_outer_angle(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_pitch(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_position(glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_position(glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_direction(glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_direction(glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_velocity(glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_velocity(glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_looping(bool value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_buffer(BufferHandle value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_gain(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_min_gain(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_max_gain(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_reference_distance(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_rolloff_factor(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_cone_outer_gain(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_maximum_distance(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_sec_offset(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_sec_offset(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_sample_offset(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_sample_offset(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_byte_offset(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_byte_offset(float value);


        [[nodiscard]]
        tl::expected<bool, AlErrorState> get_source_relative() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_cone_inner_angle() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_cone_inner_angle_i() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_cone_outer_angle() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_cone_outer_angle_i() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_pitch() const;

        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get_position() const;

        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get_position_iv() const;

        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get_direction() const;

        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get_direction_iv() const;

        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get_velocity() const;

        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get_velocity_iv() const;

        [[nodiscard]]
        tl::expected<bool, AlErrorState> get_looping() const;

        [[nodiscard]]
        tl::expected<BufferHandle, AlErrorState> get_buffer() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_gain() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_min_gain() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_max_gain() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_reference_distance() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_rolloff_factor() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_cone_outer_gain() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_maximum_distance() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_sec_offset() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_sec_offset_i() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_sample_offset() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_sample_offset_i() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_byte_offset() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_byte_offset_i() const;

        [[nodiscard]]
        tl::expected<State, AlErrorState> get_source_state() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_buffers_queued() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_buffers_processed() const;

        [[nodiscard]]
        tl::expected<Type, AlErrorState> get_source_type() const;

        //extensions
        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_byte_length() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_sample_length() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_sec_length() const;


        struct StereoAngles {
            float left_angle;
            float right_angle;
        };

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_stereo_angles(StereoAngles value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_source_radius(float value);

        [[nodiscard]]
        tl::expected<StereoAngles, AlErrorState> get_stereo_angles() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_source_radius() const;


        struct SampleOffsetLatency {
            FixedPoint<std::uint32_t> latency;
            double latency_ns;
        };
        static_assert(sizeof(SampleOffsetLatency) == 16);
        struct SecOffsetLatency {
            double offset_s;
            double latency_n;
        };
        static_assert(sizeof(SecOffsetLatency) == 16);

        [[nodiscard]]
        tl::expected<SampleOffsetLatency, AlErrorState> get_sample_offset_latency() const;

        [[nodiscard]]
        tl::expected<SecOffsetLatency, AlErrorState> get_sec_offset_latency() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_source_resampler() const;

        enum class SpatializeState{
            False = AL_FALSE,
            True = AL_TRUE,
            Auto = AL_AUTO_SOFT
        };
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_spatialize_state(SpatializeState value);
        [[nodiscard]]
        tl::expected<SpatializeState, AlErrorState> get_spatialize_state() const;


        struct SampleOffsetClock {
            FixedPoint<std::uint32_t> latency;
            double clock_ns;
        };
        static_assert(sizeof(SampleOffsetClock) == 16);
        struct SecOffsetClock {
            double offset_s;
            double clock_n;
        };
        static_assert(sizeof(SecOffsetClock) == 16);

        [[nodiscard]]
        tl::expected<SampleOffsetClock, AlErrorState> get_sample_offset_clock() const;

        [[nodiscard]]
        tl::expected<SecOffsetClock, AlErrorState> get_sec_offset_clock() const;

        enum class DirectChannelsState : ALenum {
            False = AL_FALSE,
            DropUnmatched = AL_DROP_UNMATCHED_SOFT,
            RemixUnmatched = AL_REMIX_UNMATCHED_SOFT,
        };

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_direct_channels_state(DirectChannelsState value);
        [[nodiscard]]
        tl::expected<DirectChannelsState, AlErrorState> get_direct_channels_state() const;

    private:
    };

    static constexpr auto null_source_handle = SourceHandle();

    class Source : public SourceHandle {
    public:
        using SourceHandle::SourceHandle;

        ~Source();

        Source(Source &&rhs) noexcept;

        Source &operator=(Source &&rhs) noexcept;

        Source(const Source &rhs) = delete;

        Source &operator=(const Source &rhs) = delete;

        [[nodiscard]]
        static tl::expected<Source, AlErrorState> generate();

        [[nodiscard]]
        static tl::expected<std::vector<Source>, AlErrorState> generate_many(ALsizei count);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> play() const;

        [[nodiscard]]
        tl::expected<std::vector<BufferHandle>, AlErrorState> unqueue_buffers(ALsizei size);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> queue_buffers(std::span<BufferHandle> buffer_handles);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> queue_buffers(std::span<Buffer> buffer_handles);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> clear_buffers();

    private:
    };
}
#endif //VULKANWORKSPACE_OPENAL_SOURCE_H
