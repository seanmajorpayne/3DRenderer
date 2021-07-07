#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
    float m[4][4];
} mat4_t;

mat4_t mat4_identity(void);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_rotate_z(float angle);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t get_world_matrix(mat4_t scale_matrix, mat4_t  translation_matrix,
                        mat4_t  rotation_matrix_x, mat4_t rotation_matrix_y,
                        mat4_t rotation_matrix_z);
mat4_t mat4_make_projection(float fov, float aspect, float zfar, float znear);
vec4_t mat4_mult_vec4_project(mat4_t m, vec4_t v);
vec4_t mat4_vec4_multiply(mat4_t m, vec4_t v);
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);

#endif