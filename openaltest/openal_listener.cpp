//
// Created by Bolt on 10/21/2023.
//

#include "openal_listener.h"
namespace openal{

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, float value) {
        m_context_handle.set_current_thread_context().value();
        alListenerf(static_cast<ALenum>(attribute), value);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, glm::vec3 value) {
        m_context_handle.set_current_thread_context().value();
        alListener3f(static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, std::span<const float> value) {
        m_context_handle.set_current_thread_context().value();
        alListenerfv(static_cast<ALenum>(attribute), value.data());
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, std::int32_t value) {
        m_context_handle.set_current_thread_context().value();
        alListeneri(static_cast<ALenum>(attribute), value);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, glm::ivec3 value) {
        m_context_handle.set_current_thread_context().value();
        alListener3i(static_cast<ALenum>(attribute), value[0], value[1], value[2]);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::set(Listener::Attribute attribute, std::span<const std::int32_t> value) {
        m_context_handle.set_current_thread_context().value();
        alListeneriv(static_cast<ALenum>(attribute), value.data());
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, float &value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListenerf(static_cast<ALenum>(attribute), &value);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, glm::vec3 &value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListener3f(static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, std::span<float> value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListenerfv(static_cast<ALenum>(attribute), value.data());
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, std::int32_t &value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListeneri(static_cast<ALenum>(attribute), &value);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, glm::ivec3 &value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListener3i(static_cast<ALenum>(attribute), &value[0], &value[1], &value[2]);
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    Listener::get(Listener::Attribute attribute, std::span<std::int32_t> value) const {
        m_context_handle.set_current_thread_context().value();
        alGetListeneriv(static_cast<ALenum>(attribute), value.data());
        if (auto result = m_context_handle.get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }


    tl::expected<tl::monostate, AlErrorState> Listener::set_gain(float value) {
        return set(Attribute::Gain, value);
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_position(glm::vec3 value) {
        return set(Attribute::Position, value);
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_position(glm::ivec3 value) {
        return set(Attribute::Position, value);
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_velocity(glm::vec3 value) {
        return set(Attribute::Velocity, value);
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_velocity(glm::ivec3 value) {
        return set(Attribute::Velocity, value);
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_orientation(const AtUp<float> &value) {
        return set(Attribute::Velocity, value.as_span());
    }

    tl::expected<tl::monostate, AlErrorState> Listener::set_orientation(const AtUp<std::int32_t> &value) {
        return set(Attribute::Velocity, value.as_span());
    }

    tl::expected<float, AlErrorState> Listener::get_gain() const {
        float value;
        if (auto result = get(Attribute::Gain, value); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<glm::vec3, AlErrorState> Listener::get_position() const {
        glm::vec3 value;
        if (auto result = get(Attribute::Gain, value); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<glm::ivec3, AlErrorState> Listener::get_position_i() const {
        glm::ivec3 value;
        if (auto result = get(Attribute::Gain, value); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<glm::vec3, AlErrorState> Listener::get_velocity() const {
        glm::vec3 value;
        if (auto result = get(Attribute::Gain, value); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<glm::ivec3, AlErrorState> Listener::get_velocity_i() const {
        glm::ivec3 value;
        if (auto result = get(Attribute::Gain, value); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<AtUp<float>, AlErrorState> Listener::get_orientation() const {
        AtUp<float> value{};
        if (auto result = get(Attribute::Gain, value.as_span()); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<AtUp<std::int32_t>, AlErrorState> Listener::get_orientation_i() const {
        AtUp<std::int32_t> value{};
        if (auto result = get(Attribute::Gain, value.as_span()); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    Listener::Listener(ContextHandle context_handle) : m_context_handle(context_handle) {

    }
}