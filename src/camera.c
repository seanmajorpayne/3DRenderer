#include "camera.h"

static camera_t camera = {
    .position = {0, 0, 0},
    .direction = {0, 0, 1},
    .forward_velocity = {0, 0, 0},
    .yaw = 0.f,
    .pitch = 0.f
};

float get_camera_yaw(void) {
    return camera.yaw;
}

float get_camera_pitch(void) {
    return camera.pitch;
}

void move_camera_forward(float delta_time) {
    camera.forward_velocity = vec3_multiply(camera.direction, 5.0 * delta_time);
    camera.position = vec3_add(camera.position, camera.forward_velocity);
}

void move_camera_backwards(float delta_time) {
    camera.forward_velocity = vec3_multiply(camera.direction, 5.0 * delta_time);
    camera.position = vec3_subtract(camera.position, camera.forward_velocity);
}

void increase_camera_yaw(float delta_time) {
    camera.yaw += 1.0 * delta_time;
}

void decrease_camera_yaw(float delta_time) {
    camera.yaw -= 1.0 * delta_time;
}

void increase_camera_pitch(float delta_time) {
    camera.pitch += 1.0 * delta_time;
}

void decrease_camera_pitch(float delta_time) {
    camera.pitch -= 1.0 * delta_time;
}

void increase_camera_y_pos(float delta_time) {
    camera.position.y += 3.0 * delta_time;
}

void decrease_camera_y_yaw(float delta_time) {
    camera.position.y -= 3.0 * delta_time;
}

vec3_t update_target(mat4_t camera_yaw_rotation, mat4_t camera_pitch_rotation) {
    vec3_t target = vec3_new(0.0, 0.0, 1.0);
    camera.direction = vec3_from_vec4(mat4_vec4_multiply(camera_yaw_rotation, vec4_from_vec3(target)));
    vec3_t result = vec3_add(camera.position, camera.direction);
    camera.direction = vec3_from_vec4(mat4_vec4_multiply(camera_pitch_rotation, vec4_from_vec3(target)));
    return vec3_add(result, camera.direction);
}

mat4_t get_view_matrix(vec3_t target, vec3_t up_direction) 
{
    return mat4_look_at(camera.position, target, up_direction);   
}