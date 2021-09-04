//
// Created by Shae Bolt on 9/3/2021.
//

#include "firstpersoncamera.h"

#include "firstpersoncamera.h"

gul::FirstPersonCamera::FirstPersonCamera(const glm::vec3 &position,
                                          const glm::vec3 &pitch_yaw_roll,
                                          const glm::vec3 &world_up) :
        m_position(position), m_pitch_yaw_roll(pitch_yaw_roll),
        m_world_up(world_up){
    updateFrontRightUp(m_pitch_yaw_roll);
}

void gul::FirstPersonCamera::updateFront(const glm::vec3 &pitch_yaw_roll) {
    float pitch = pitch_yaw_roll.x;
    float yaw = pitch_yaw_roll.y;
    m_front.x = glm::cos(pitch) * glm::sin(yaw);
    m_front.y = glm::sin(pitch);
    m_front.z = glm::cos(yaw) * glm::cos(pitch);
    //shouldn't need to normalize? norm(m_front);
    m_front = glm::normalize(m_front);

}

void gul::FirstPersonCamera::updateRight(const glm::vec3 &front) {
    m_right = glm::normalize(glm::cross(m_world_up, front));
}
//				x.y * y.z - y.y * x.z,
//				x.z * y.x - y.z * x.x,
//				x.x * y.y - y.x * x.y

void gul::FirstPersonCamera::updateUp(const glm::vec3 &front,
                                      const glm::vec3 &right) {
    m_up = glm::normalize(glm::cross(front,right));
}

void gul::FirstPersonCamera::updateFrontRightUp(const glm::vec3 &pitch_yaw_roll) {
    updateFront(pitch_yaw_roll);
    updateRight(m_front);
    updateUp(m_front, m_right);
}

glm::mat4 gul::FirstPersonCamera::calcViewMatrix() const{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 gul::FirstPersonCamera::calcRotationMatrix() const {
    //https://gamedev.stackexchange.com/a/120504/99481
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = m_right.x;
    rotation[0][1] = m_right.y;
    rotation[0][2] = m_right.z;

    rotation[1][0] = m_up.x;
    rotation[1][1] = m_up.y;
    rotation[1][2] = m_up.z;

    rotation[2][0] = m_front.x;
    rotation[2][1] = m_front.y;
    rotation[2][2] = m_front.z;

    return rotation;
}


void gul::FirstPersonCamera::setPosition(const glm::vec3 &position) {
    m_position = position;
}

const glm::vec3 &gul::FirstPersonCamera::getPosition() const {
    return m_position;
}

void gul::FirstPersonCamera::translate(const glm::vec3 &translation) {
    m_position += translation;
}

void gul::FirstPersonCamera::setRotation(const glm::vec3 &pitch_yaw_roll) {
    m_pitch_yaw_roll = pitch_yaw_roll;
    updateFrontRightUp(pitch_yaw_roll);
}

void
gul::FirstPersonCamera::offsetRotation(const glm::vec3 &pitch_yaw_roll_offset) {
    m_pitch_yaw_roll += pitch_yaw_roll_offset;
    updateFrontRightUp(m_pitch_yaw_roll);
}

const glm::vec3 gul::FirstPersonCamera::getRotation() const {
    return m_pitch_yaw_roll;
}

void
gul::FirstPersonCamera::move(bool forward, bool back, bool left, bool right,
                             float delta_time, float speed) {
    move(forward, back, left, right, false, false, delta_time,speed);
}

void
gul::FirstPersonCamera::move(bool forward, bool back, bool left, bool right,
                             bool up, bool down, float delta_time,
                             float speed) {
    float move_speed = delta_time * speed;
    if (forward) {
        m_position += m_front * move_speed;
    }
    if (back) {
        m_position -= m_front * move_speed;
    }
    if (left) {
        m_position -= m_right * move_speed;
    }
    if (right) {
        m_position += m_right * move_speed;
    }
    if (up){
        m_position += m_up * move_speed;
    }
    if (down){
        m_position -= m_up * move_speed;
    }
}


void gul::FirstPersonCamera::rotate(bool up, bool down, bool left, bool right,
                                    float delta_time, float speed) {
    float rotate_speed = delta_time * speed;
    if (up) {
        m_pitch_yaw_roll.x += rotate_speed;
        m_pitch_yaw_roll.x = glm::min(m_pitch_yaw_roll.x, glm::radians(89.0f));
    }
    if (down) {
        m_pitch_yaw_roll.x -= rotate_speed;
        m_pitch_yaw_roll.x = glm::max(m_pitch_yaw_roll.x, glm::radians(-89.0f));
    }
    if (left) {
        m_pitch_yaw_roll.y -= rotate_speed;
    }
    if (right) {
        m_pitch_yaw_roll.y += rotate_speed;

    }
    updateFrontRightUp(m_pitch_yaw_roll);
}

glm::vec3 gul::FirstPersonCamera::getMoveVelocity(bool forward, bool back, bool left,
                                                  bool right, float speed) {
    auto velocity = glm::vec3(0.0f);
    if (forward) {
        velocity += m_front * speed;
    }
    if (back) {
        velocity -= m_front * speed;
    }
    if (left) {
        velocity -= m_right * speed;
    }
    if (right) {
        velocity += m_right * speed;
    }
    return velocity;
}

glm::mat4 gul::FirstPersonCamera::calcTranslationMatrix() const {
    return glm::translate(glm::mat4(1.0f), m_position);
}

void gul::FirstPersonCamera::lookAt(const glm::vec3 &center) {
    glm::mat4x4 look_at_matrix = glm::lookAt(m_position, center, m_world_up);
    glm::vec3 euler_angles = glm::eulerAngles(glm::quat_cast(look_at_matrix));
    //   euler_angles.x *= -1;
    //   euler_angles.y *= -1;
//    euler_angles.z *= -1;
    setRotation(euler_angles);
}