//
// Created by Bolt on 10/21/2023.
//

#ifndef VULKANWORKSPACE_OPENAL_LISTENER_H
#define VULKANWORKSPACE_OPENAL_LISTENER_H

#include "openal_util.h"
#include "openal_context.h"
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#include <tl/expected.hpp>
#include <span>


namespace openal{

    class Listener {
    public:

        //TODO make protected friend of Context handle?
        Listener(ContextHandle context_handle);

        enum class Attribute : ALenum {
            Gain = AL_GAIN,
            Position = AL_POSITION,
            Velocity = AL_VELOCITY,
            Orientation = AL_ORIENTATION
        };

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
        tl::expected<tl::monostate, AlErrorState>
        set(Attribute attribute, std::span<const std::int32_t> value);


        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, float &value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, glm::vec3 &value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::span<float> value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, std::int32_t &value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> get(Attribute attribute, glm::ivec3 &value) const;

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState>
        get(Attribute attribute, std::span<std::int32_t> value) const;


        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_gain(float value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_position(glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_position(glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_velocity(glm::vec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_velocity(glm::ivec3 value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_orientation(const AtUp<float> &value);

        [[nodiscard]]
        tl::expected<tl::monostate, AlErrorState> set_orientation(const AtUp<std::int32_t> &value);

        [[nodiscard]]
        tl::expected<float, AlErrorState> get_gain() const;

        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get_position() const;

        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get_position_i() const;

        [[nodiscard]]
        tl::expected<glm::vec3, AlErrorState> get_velocity() const;

        [[nodiscard]]
        tl::expected<glm::ivec3, AlErrorState> get_velocity_i() const;

        [[nodiscard]]
        tl::expected<AtUp<float>, AlErrorState> get_orientation() const;

        [[nodiscard]]
        tl::expected<AtUp<std::int32_t>, AlErrorState> get_orientation_i() const;

    private:
        ContextHandle m_context_handle;
    };
}
#endif //VULKANWORKSPACE_OPENAL_LISTENER_H
