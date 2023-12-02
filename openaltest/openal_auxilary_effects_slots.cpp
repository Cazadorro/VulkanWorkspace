//
// Created by Bolt on 10/21/2023.
//

#include "openal_auxilary_effects_slots.h"
#include "openal_effect.h"
#include "openal_context.h"
#include <uul/assert.h>
#include <fmt/core.h>
namespace openal{

    bool AuxiliaryEffectSlotHandle::is_valid() const {
        return static_cast<bool>(alIsAuxiliaryEffectSlot(get_handle()));
    }

    tl::expected<tl::monostate, AlErrorState> AuxiliaryEffectSlotHandle::set(Attribute attribute, float value) {
        alAuxiliaryEffectSlotf(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> AuxiliaryEffectSlotHandle::set(Attribute attribute, std::int32_t value) {
        alAuxiliaryEffectSloti(get_handle(), static_cast<ALenum>(attribute), value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    AuxiliaryEffectSlotHandle::set(Attribute attribute, std::span<const float> value) {
        alAuxiliaryEffectSlotfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState>
    AuxiliaryEffectSlotHandle::set(Attribute attribute, std::span<const std::int32_t> value) {
        alAuxiliaryEffectSlotiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }



    template<>
    tl::expected<float, AlErrorState>
    AuxiliaryEffectSlotHandle::get(AuxiliaryEffectSlotHandle::Attribute attribute) const {
        float value;
        alGetAuxiliaryEffectSlotf(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    template<>
    tl::expected<std::int32_t, AlErrorState>
    AuxiliaryEffectSlotHandle::get(AuxiliaryEffectSlotHandle::Attribute attribute) const {
        std::int32_t value;
        alGetAuxiliaryEffectSloti(get_handle(), static_cast<ALenum>(attribute), &value);
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return value;
    }

    tl::expected<tl::monostate, AlErrorState>
    AuxiliaryEffectSlotHandle::get(Attribute attribute, std::span<float> value) const {
        alGetAuxiliaryEffectSlotfv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();


    }


    tl::expected<tl::monostate, AlErrorState>
    AuxiliaryEffectSlotHandle::get(Attribute attribute, std::span<std::int32_t> value) const {
        alGetAuxiliaryEffectSlotiv(get_handle(), static_cast<ALenum>(attribute), value.data());
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return tl::monostate();
    }

    tl::expected<tl::monostate, AlErrorState> AuxiliaryEffectSlotHandle::set_effect(UnTypedEffectHandle value) {
        return set(Attribute::Effect, static_cast<ALint>(value.get_handle()));
    }

    tl::expected<tl::monostate, AlErrorState> AuxiliaryEffectSlotHandle::set_gain(float value) {
        return set(Attribute::Gain, value);
    }

    tl::expected<tl::monostate, AlErrorState> AuxiliaryEffectSlotHandle::set_auxiliary_send_auto(bool value) {
        return set(Attribute::AuxiliarySendAuto, static_cast<std::int32_t>(value));
    }

    tl::expected<UnTypedEffectHandle, AlErrorState> AuxiliaryEffectSlotHandle::get_effect() const {
        return get<std::int32_t>(Attribute::Effect).map([](auto value){
            return UnTypedEffectHandle(static_cast<ALuint>(value));
        });
    }

    tl::expected<float, AlErrorState> AuxiliaryEffectSlotHandle::get_gain() const {
        return get<float>(Attribute::Gain);
    }

    tl::expected<bool, AlErrorState> AuxiliaryEffectSlotHandle::get_auxiliary_send_auto() const {
        return get<std::int32_t>(Attribute::AuxiliarySendAuto).map([](auto value){
            return static_cast<bool>(value);
        });
    }

    AuxiliaryEffectSlot::~AuxiliaryEffectSlot() {
        if (*this != null_auxiliary_effect_slot_handle) {
            Context::get_current_thread_context().clear_errors();
            alDeleteAuxiliaryEffectSlots(1, &get_handle());
            auto result = Context::get_current_thread_context().get_error();
            UUL_ASSERT(result.has_value(), fmt::format("{}", to_string(result.error())).c_str());
        }
    }

    AuxiliaryEffectSlot::AuxiliaryEffectSlot(AuxiliaryEffectSlot &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
    }

    AuxiliaryEffectSlot &AuxiliaryEffectSlot::operator=(AuxiliaryEffectSlot &&rhs) noexcept {
        using std::swap;
        std::swap(get_handle(), rhs.get_handle());
        return *this;
    }

    tl::expected<AuxiliaryEffectSlot, AlErrorState> AuxiliaryEffectSlot::generate() {
        Context::get_current_thread_context().clear_errors();
        ALuint auxiliary_effect_slot;
        alGenAuxiliaryEffectSlots(1, &auxiliary_effect_slot);
        if (auto result =  Context::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return AuxiliaryEffectSlot(auxiliary_effect_slot);
    }

    tl::expected<std::vector<AuxiliaryEffectSlot>, AlErrorState> AuxiliaryEffectSlot::generate_many(ALsizei count) {
        ContextHandle::get_current_thread_context().clear_errors();
        std::vector<AuxiliaryEffectSlot> auxiliary_effect_slots(count);
        alGenAuxiliaryEffectSlots(count, reinterpret_cast<ALuint *>(auxiliary_effect_slots.data()));
        if (auto result = ContextHandle::get_current_thread_context().get_error(); !result.has_value()) {
            return tl::make_unexpected(result.error());
        }
        return auxiliary_effect_slots;
    }


}