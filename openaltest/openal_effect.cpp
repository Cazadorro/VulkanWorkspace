//
// Created by Bolt on 10/21/2023.
//

#include "openal_effect.h"
#include "openal_context.h"
#include <uul/assert.h>
#include <fmt/core.h>
namespace openal{

    bool UnTypedEffectHandle::is_valid() const {
        return alIsEffect(get_handle());
    }

    tl::expected<tl::monostate, AlErrorState> UnTypedEffectHandle::set(UnTypedEffectHandle::Attribute attribute, float value) {
        alEffectf(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> UnTypedEffectHandle::set(UnTypedEffectHandle::Attribute attribute, std::int32_t value) {
        alEffecti(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    UnTypedEffectHandle::set(UnTypedEffectHandle::Attribute attribute, std::span<const float> value) {
        alEffectfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    UnTypedEffectHandle::set(UnTypedEffectHandle::Attribute attribute, std::span<const std::int32_t> value) {
        alEffectiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    template<>
    tl::expected<float, AlErrorState>
    UnTypedEffectHandle::get(UnTypedEffectHandle::Attribute attribute) const {
        float value;
        alGetEffectf(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<std::int32_t, AlErrorState>
    UnTypedEffectHandle::get(UnTypedEffectHandle::Attribute attribute) const {
        std::int32_t value;
        alGetEffecti(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<tl::monostate, AlErrorState>
    UnTypedEffectHandle::get(UnTypedEffectHandle::Attribute attribute, std::span<float> value) const {
        alGetEffectfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();

    }

    tl::expected<tl::monostate, AlErrorState>
    UnTypedEffectHandle::get(UnTypedEffectHandle::Attribute attribute, std::span<std::int32_t> value) const {
        alGetEffectiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> UnTypedEffectHandle::set_type(UnTypedEffectHandle::Type value) {
        return set(Attribute::EffectType, static_cast<ALint>(value));
    }

    tl::expected<UnTypedEffectHandle::Type, AlErrorState> UnTypedEffectHandle::get_type() const {
        return get<std::int32_t>(Attribute::EffectType).map([](auto value){
            return static_cast<Type>(value);
        });
    }

    Effect::~Effect() {
        if (*this != null_effect_handle) {
            Context::get_current_thread_context().clear_errors();
            alDeleteEffects(1, &get_handle());
            auto result = Context::get_current_thread_context().get_error();
            UUL_ASSERT(result.has_value(), fmt::format("{}", to_string(result.error())).c_str());
        }
    }

    Effect::Effect(Effect &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
    }

    Effect &Effect::operator=(Effect &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }

    tl::expected<Effect, AlErrorState> Effect::generate() {
        Context::get_current_thread_context().clear_errors();
        ALuint auxiliary_effect_slot;
        alGenEffects(1, &auxiliary_effect_slot);
        if (auto result =  Context::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return Effect(auxiliary_effect_slot);
    }

    tl::expected<std::vector<Effect>, AlErrorState> Effect::generate_many(ALsizei count) {
        ContextHandle::get_current_thread_context().clear_errors();
        std::vector<Effect> auxiliary_effect_slots(count);
        alGenEffects(count, reinterpret_cast<ALuint *>(auxiliary_effect_slots.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return auxiliary_effect_slots;
    }
}