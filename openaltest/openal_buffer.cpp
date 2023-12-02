//
// Created by Bolt on 10/21/2023.
//

#include "openal_buffer.h"
#include "openal_context.h"
#include <uul/assert.h>
#include <fmt/core.h>
#include <range/v3/view/enumerate.hpp>

namespace openal{
    bool BufferHandle::is_valid() const {
        return alIsBuffer(get_handle());
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, float value) {
        alBufferf(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, std::int32_t value) {
        alBufferi(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }


    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, glm::vec3 value) {
        alBuffer3f(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, glm::ivec3 value) {
        alBuffer3i(get_handle(), static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, std::span<const float> value) {
        alBufferfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }




    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set(BufferHandle::Attribute attribute, std::span<const std::int32_t> value) {
        alBufferiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    template<>
    tl::expected<float, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute) const {
        float value;
        alGetBufferf(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }
    template<>
    tl::expected<std::int32_t, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute) const {
        std::int32_t value;
        alGetBufferi(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }
    template<>
    tl::expected<glm::vec3, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute) const {
        glm::vec3 value;
        alGetBuffer3f(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<glm::ivec3, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute) const {
        glm::ivec3 value;
        alGetBuffer3i(get_handle(), static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute, std::span<float> value) const {
        alGetBufferfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::get(BufferHandle::Attribute attribute, std::span<std::int32_t> value) const {
        alGetBufferiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set_data(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz) {
        UUL_ASSERT(data.size() <= std::numeric_limits<ALsizei>::max(),
                   fmt::format("OpenAL Soft can only deal with buffer sizes at max {}  bytes",
                               std::numeric_limits<ALsizei>::max()).c_str());
        alBufferData(get_handle(), static_cast<ALenum>(format), data.data(), static_cast<ALsizei>(data.size()), sample_rate_hz);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_frequency() const {
        return get<std::int32_t>(Attribute::Frequency);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_bits() const {
        return get<std::int32_t>(Attribute::Bits);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_channels() const {
        return get<std::int32_t>(Attribute::Channels);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_size() const {
        return get<std::int32_t>(Attribute::Size);
    }

    tl::expected<tl::monostate, AlErrorState> BufferHandle::set_unpack_block_alignment(std::int32_t value) {
        return set(Attribute::UnpackBlockAlignment, value);
    }

    tl::expected<tl::monostate, AlErrorState> BufferHandle::set_pack_block_alignment(std::int32_t value) {
        return set(Attribute::PackBlockAlignment, value);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_unpack_block_alignment() const {\
        return get<std::int32_t>(Attribute::UnpackBlockAlignment);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_pack_block_alignment() const {
        return get<std::int32_t>(Attribute::PackBlockAlignment);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_byte_length() const {
        return get<std::int32_t>(Attribute::ByteLength);
    }

    tl::expected<std::int32_t, AlErrorState> BufferHandle::get_sample_length() const {
        return get<std::int32_t>(Attribute::SampleLength);
    }

    tl::expected<float, AlErrorState> BufferHandle::get_sec_length() const {
        return get<float>(Attribute::SecLength);
    }

    tl::expected<tl::monostate, AlErrorState> BufferHandle::set_ambisonic_layout(BufferHandle::AmbisonicLayout value) {
        return set(Attribute::AmbisonicLayout, static_cast<std::int32_t>(value));
    }

    tl::expected<BufferHandle::AmbisonicLayout, AlErrorState> BufferHandle::get_ambisonic_layout() const {
        return get<std::int32_t>(Attribute::AmbisonicLayout).map([](auto value){
            return static_cast<AmbisonicLayout>(value);
        });
    }

    tl::expected<tl::monostate, AlErrorState>
    BufferHandle::set_ambisonic_scaling(BufferHandle::AmbisonicScaling value) {
        return set(Attribute::AmbisonicScaling, static_cast<std::int32_t>(value));
    }

    tl::expected<BufferHandle::AmbisonicScaling, AlErrorState> BufferHandle::get_ambisonic_scaling() const {
        return get<std::int32_t>(Attribute::AmbisonicScaling).map([](auto value){
            return static_cast<AmbisonicScaling>(value);
        });
    }


    Buffer::~Buffer() {
        if (*this != null_buffer_handle) {
            ContextHandle::get_current_thread_context().clear_errors();
            alDeleteBuffers(1, &get_handle());
            auto error = ContextHandle::get_current_thread_context().get_error();
            UUL_ASSERT(error.has_value(), fmt::format("{}", to_string(error.error())).c_str());
        }
    }

    Buffer::Buffer(Buffer &&rhs) noexcept {

        std::swap(get_handle(), rhs.get_handle());
    }

    Buffer &Buffer::operator=(Buffer &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }

    tl::expected<Buffer, AlErrorState> Buffer::generate() {
        ContextHandle::get_current_thread_context().clear_errors();
        ALuint buffer;
        alGenBuffers(1, &buffer);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return Buffer{buffer};
    }

    tl::expected<std::vector<Buffer>, AlErrorState> Buffer::generate_many(ALsizei buffer_count) {
        ContextHandle::get_current_thread_context().clear_errors();
        std::vector<Buffer> buffers(buffer_count);
        alGenBuffers(buffer_count, reinterpret_cast<ALuint *>(buffers.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return buffers;
    }

    tl::expected<Buffer, AlErrorState>
    Buffer::from_data(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz) {
        return Buffer::generate().and_then([format, data, sample_rate_hz](Buffer new_buffer) -> tl::expected<Buffer, AlErrorState>{
            if(auto result = new_buffer.set_data(format, data, sample_rate_hz); !result.has_value()){
                return tl::make_unexpected(result.error());
            }
            return new_buffer;
        });
    }

    tl::expected<std::vector<Buffer>, AlErrorState>
    Buffer::from_data_many(Format format, std::span<const std::byte> data, std::int32_t sample_rate_hz,
                           ALsizei buffer_count, ALsizei buffer_size_bytes) {
        UUL_ASSERT(buffer_count * buffer_size_bytes <= data.size(),
                   fmt::format("size of data is smaller than data to be read into buffers {} < {}",
                               buffer_count * buffer_size_bytes, data.size()).c_str());
        auto result = Buffer::generate_many(buffer_count);
        return result.and_then([format, data, sample_rate_hz, buffer_size_bytes](std::vector<Buffer>& new_buffers)
                                       -> tl::expected<std::vector<Buffer>, AlErrorState>{
            for (auto [idx, buffer]: ranges::views::enumerate(new_buffers)) {
                if (auto result = buffer.set_data(format, data.subspan(idx * buffer_size_bytes, buffer_size_bytes),
                                                  sample_rate_hz); !result.has_value()) {
                    return tl::make_unexpected(result.error());
                }
            }
            return std::move(new_buffers);
        });
    }

}