#include "camera.h"

// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : forward(glm::vec3(0.0f, 0.0f, -1.0f)), speed(50.f), sens(0.2f)
{
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;
    update_vectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::get_view_matrix() const
{
    return glm::lookAt(position, position + forward, up);
}

void Camera::process_key_input(Camera::Direction direction, float deltaTime, bool onlyXZ)
{
    auto move_dir = glm::vec3(0.f);
    auto local_forward = onlyXZ ? glm::normalize(glm::vec3(forward.x, 0, forward.z)) : forward;
    auto local_right = onlyXZ ? glm::normalize(glm::vec3(right.x, 0, right.z)) : right;
    float move_speed = speed * deltaTime;
    if (direction == FORWARD)
        move_dir += local_forward;
    if (direction == BACKWARD)
        move_dir -= local_forward;
    if (direction == LEFT)
        move_dir -= local_right;
    if (direction == RIGHT)
        move_dir += local_right;
    position += move_dir * move_speed;
}

void Camera::process_cursor_input(float x_offset, float y_offset)
{
    x_offset *= sens;
    y_offset *= sens;

    yaw += x_offset;
    pitch += y_offset;

    // looking up/down constraints
    if (pitch > 89.f)
        pitch = 89.f;
    if (pitch < -89.f)
        pitch = -89.f;

    // update vectors
    update_vectors();
}

void Camera::update_vectors()
{
    auto direction = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    forward = glm::normalize(direction);
    right = glm::normalize(glm::cross(forward, world_up));
    up = glm::normalize(glm::cross(right, forward));
}