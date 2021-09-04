//
// Created by Shae Bolt on 9/3/2021.
//

#ifndef VULKANWORKSPACE_FIRSTPERSONCAMERA_H
#define VULKANWORKSPACE_FIRSTPERSONCAMERA_H
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED
//#define GLM_FORCE_RIGHT_HANDED
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gul {
    class FirstPersonCamera {
    public:

        explicit FirstPersonCamera(
                const glm::vec3 &position = glm::vec3(0.0, 0.0, 0.0),
                const glm::vec3 &pitch_yaw_roll = glm::vec3(0.0, 0.0, 0.0),
                const glm::vec3 &world_up = glm::vec3(0.0, 1.0, 0.0));

        [[nodiscard]]
        glm::mat4 calcViewMatrix() const;
        [[nodiscard]]
        glm::mat4 calcRotationMatrix() const;


        [[nodiscard]]
        glm::mat4 calcTranslationMatrix() const;

        void setPosition(const glm::vec3 &position);

        [[nodiscard]]
        const glm::vec3 &getPosition() const;

        void translate(const glm::vec3 &translation);

        void setRotation(const glm::vec3 &pitch_yaw_roll);

        void offsetRotation(const glm::vec3 &pitch_yaw_roll_offset);

        [[nodiscard]]
        const glm::vec3 getRotation() const;

        void lookAt(const glm::vec3& center);

        [[nodiscard]]
        glm::vec3
        getMoveVelocity(bool forward, bool back, bool left, bool right,
                        float speed = 2.0f);
        void
        move(bool forward, bool back, bool left, bool right, float delta_time,
             float speed = 2.0f);

        void
        move(bool forward, bool back, bool left, bool right, bool up, bool down, float delta_time,
             float speed = 2.0f);

        void rotate(bool up, bool down, bool left, bool right, float delta_time,
                    float speed = 0.75f);

    private:
        glm::vec3 m_position;
        glm::vec3 m_pitch_yaw_roll;
        glm::vec3 m_world_up;
        glm::vec3 m_up;
        glm::vec3 m_front;
        glm::vec3 m_right;

        void updateFront(const glm::vec3 &pitch_yaw_roll);

        void updateRight(const glm::vec3 &front);

        void updateUp(const glm::vec3 &front, const glm::vec3 &right);

        void updateFrontRightUp(const glm::vec3 &pitch_yaw_roll);

    };
}

#endif //VULKANWORKSPACE_FIRSTPERSONCAMERA_H
