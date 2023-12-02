//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_BUFFER_H
#define VULKANWORKSPACE_OPENAL_BUFFER_H
#include "openal_util.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>

namespace openal{
    class BufferHandle : public Handle<ALuint, BufferHandle> {
    public:
        using Handle::Handle;
        enum class Attribute {
            Frequency = AL_FREQUENCY,
            Bits = AL_BITS,
            Channels = AL_CHANNELS,
            Size = AL_SIZE,
            UnpackBlockAlignment = AL_UNPACK_BLOCK_ALIGNMENT_SOFT, //AL_SOFT_block_alignment int32
            PackBlockAlignment = AL_PACK_BLOCK_ALIGNMENT_SOFT,
            ByteLength = AL_BYTE_LENGTH_SOFT, //AL_SOFT_buffer_length_query int
            SampleLength = AL_SAMPLE_LENGTH_SOFT, //int
            SecLength = AL_SEC_LENGTH_SOFT,  //float

            AmbisonicLayout = AL_AMBISONIC_LAYOUT_SOFT, // AL_SOFT_bformat_ex enum/
            AmbisonicScaling = AL_AMBISONIC_SCALING_SOFT, //enum
        };

        [[nodiscard]]
        bool is_valid() const;
        template<typename T>
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, T value) = delete;
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const float> value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const std::int32_t> value);

        template<typename T>
        [[nodiscard]]
        tl::expected<T, AlErrorState> get(Attribute attribute) const = delete;
        template<>
        [[nodiscard]]
        tl::expected<float, AlErrorState> get(Attribute attribute) const;
        template<>
        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get(Attribute attribute) const;
        template<>
        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get(Attribute attribute) const;
        template<>
        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get(Attribute attribute) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<float> value) const;
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<std::int32_t> value) const;


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
        tl::expected<tl::monostate, AlErrorState>
        set_data(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz);

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_frequency() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_bits() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_channels() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_size() const;


//extensions

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_unpack_block_alignment(std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_pack_block_alignment(std::int32_t value);

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_unpack_block_alignment() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_pack_block_alignment() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_byte_length() const;

        [[nodiscard]]
        tl::expected<std::int32_t, AlErrorState> get_sample_length() const;

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_sec_length() const;

        enum class AmbisonicLayout{
            Fuma = AL_FUMA_SOFT,
            Acn = AL_ACN_SOFT,
        };
        enum class AmbisonicScaling{
            Fuma = AL_FUMA_SOFT,
            Sn3d = AL_SN3D_SOFT,
            N3d = AL_N3D_SOFT,
        };

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_ambisonic_layout(AmbisonicLayout value);

        [[nodiscard]]
        tl::expected<AmbisonicLayout, AlErrorState> get_ambisonic_layout() const;


        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_ambisonic_scaling(AmbisonicScaling value);

        [[nodiscard]]
        tl::expected<AmbisonicScaling, AlErrorState> get_ambisonic_scaling() const;

    private:

    };

    static constexpr auto null_buffer_handle = BufferHandle();


    class Buffer : public BufferHandle {
    public:
        using BufferHandle::BufferHandle;

        ~Buffer();

        Buffer(Buffer &&rhs) noexcept;

        Buffer &operator=(Buffer &&rhs) noexcept;

        Buffer(const Buffer &rhs) = delete;

        Buffer &operator=(const Buffer &rhs) = delete;

        [[nodiscard]]
        static tl::expected<Buffer, AlErrorState> generate();

        [[nodiscard]]
        static tl::expected<std::vector<Buffer>, AlErrorState> generate_many(ALsizei buffer_count);

        [[nodiscard]]
        static tl::expected<Buffer, AlErrorState>
        from_data(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz);

        [[nodiscard]]
        static tl::expected<std::vector<Buffer>, AlErrorState>
        from_data_many(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz,
                       ALsizei buffer_count, ALsizei buffer_size_bytes);

    private:
    };

}
#endif //VULKANWORKSPACE_OPENAL_BUFFER_H
