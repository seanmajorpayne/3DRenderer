#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw;
    float pitch;
} camera_t;

float get_camera_yaw(void);
float get_camera_pitch(void);
void move_camera_forward(float delta_time);
void move_camera_backwards(float delta_time);
void increase_camera_yaw(float delta_time);
void decrease_camera_yaw(float delta_time);
void increase_camera_pitch(float delta_time);
void decrease_camera_pitch(float delta_time);
void increase_camera_y_pos(float delta_time);
void decrease_camera_y_yaw(float delta_time);
vec3_t update_target(mat4_t camera_yaw_rotation, mat4_t camera_pitch_rotation);
mat4_t get_view_matrix(vec3_t target, vec3_t up_direction);

#endif