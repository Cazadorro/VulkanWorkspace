//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_AUXILARY_EFFECTS_SLOTS_H
#define VULKANWORKSPACE_OPENAL_AUXILARY_EFFECTS_SLOTS_H

#include "openal_util.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>

namespace openal {

    class UnTypedEffectHandle;
    class AuxiliaryEffectSlotHandle : public Handle<ALuint, AuxiliaryEffectSlotHandle> {
    public:
        using Handle::Handle;

        enum class Attribute : ALenum {
           Effect = AL_EFFECTSLOT_EFFECT,
           Gain = AL_EFFECTSLOT_GAIN,
           AuxiliarySendAuto = AL_EFFECTSLOT_AUXILIARY_SEND_AUTO,
        };

        [[nodiscard]]
        bool is_valid() const;

        template<typename T>
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, T value) = delete;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::int32_t value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set(Attribute attribute, std::span<const float> value);

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

        //TODO Effect Object ID used in set_effect.
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_effect(UnTypedEffectHandle value);
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_gain(float value);
        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_auxiliary_send_auto(bool value);

        [[nodiscard]]
        tl::expected<UnTypedEffectHandle, AlErrorState> get_effect() const;
        [[nodiscard]]
        tl::expected<float, AlErrorState> get_gain() const;
        [[nodiscard]]
        tl::expected<bool, AlErrorState> get_auxiliary_send_auto() const;
    private:
    };

    static constexpr auto null_auxiliary_effect_slot_handle = AuxiliaryEffectSlotHandle();

    class AuxiliaryEffectSlot : public AuxiliaryEffectSlotHandle {
    public:
        using AuxiliaryEffectSlotHandle::AuxiliaryEffectSlotHandle;

        ~AuxiliaryEffectSlot();

        AuxiliaryEffectSlot(AuxiliaryEffectSlot &&rhs) noexcept;

        AuxiliaryEffectSlot &operator=(AuxiliaryEffectSlot &&rhs) noexcept;

        AuxiliaryEffectSlot(const AuxiliaryEffectSlot &rhs) = delete;

        AuxiliaryEffectSlot &operator=(const AuxiliaryEffectSlot &rhs) = delete;

        [[nodiscard]]
        static tl::expected<AuxiliaryEffectSlot, AlErrorState> generate();

        [[nodiscard]]
        static tl::expected<std::vector<AuxiliaryEffectSlot>, AlErrorState> generate_many(ALsizei count);
    };
}
#endif //VULKANWORKSPACE_OPENAL_AUXILARY_EFFECTS_SLOTS_H
