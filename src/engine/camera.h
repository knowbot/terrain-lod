#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
    public:
        // Options for camera movement
        enum Direction {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT
        };

        glm::vec3 position{};
        glm::vec3 forward{};
        glm::vec3 up{};
        glm::vec3 right{};
        glm::vec3 world_up{};

        float yaw = -90.f, pitch = 0.f;
        float speed = 500.f, sens = 0.2f;

        // constructor with vectors
        explicit Camera(glm::vec3 position = glm::vec3(0, 0, 0), glm::vec3 up = glm::vec3(0, 1, 0), float yaw = -90.f, float pitch = 0.f);

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 get_view_matrix() const;

        void process_key_input(Direction direction, float deltaTime, bool onlyXZ = false);

        void process_cursor_input(float x_offset, float y_offset);

    private:
        void update_vectors();
    };
#endif