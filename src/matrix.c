#include "matrix.h"
#include "vector.h"
#include <math.h>

/*
* | 1 0 0 0 |
* | 0 1 0 0 |
* | 0 0 1 0 |
* | 0 0 0 1 |
*/
mat4_t mat4_identity(void) {
    mat4_t mat = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return mat;
}

/*
* |sx  0  0 0 |
* | 0 sy  0 0 |
* | 0  0 sz 0 |
* | 0  0  0 1 |
*/
mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t mat = mat4_identity();
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

/*
* | 1 0 0 tx |
* | 0 1 0 ty |
* | 0 0 1 tz |
* | 0 0 0 1  |
*/
mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t mat = mat4_identity();
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

mat4_t mat4_rotate_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t mat = mat4_identity();
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

mat4_t mat4_rotate_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t mat = mat4_identity();
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;    
}

mat4_t mat4_rotate_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t mat = mat4_identity();
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;    
}

/*
* Performs matrix multiplication on scale, translation and rotation matrices
* to calculate the final position of an object after all transformations.
* Tranformation order matters scale -> rotation -> translation.
* Multiplication order matters [[modifier]] * [[world]]
*/
mat4_t get_world_matrix(mat4_t scale_matrix, mat4_t  translation_matrix,
                        mat4_t  rotation_matrix_x, mat4_t rotation_matrix_y,
                        mat4_t rotation_matrix_z) {

    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
    return world_matrix;
}

/*
* Projection Matrix
* | aspect * (1 / tan(fov/2))    0              0                        0                                |
* | 0                            1/tan(fov/2)   0                        0                                |
* | 0                            0              zfar / (zfar - znear)    -(zfar * znear) / (zfar - znear) |
* | 0                            0              1                        0                                |
* Used to calculate the perspective of an object (visualizing 3D in 2D).
*/
mat4_t mat4_make_projection(float fov, float aspect, float zfar, float znear) {
    mat4_t m = {{{0}}};
    m.m[0][0] = aspect * (1 / tan(fov/2));
    m.m[1][1] = 1 / tan(fov/2);
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = -(zfar * znear) / (zfar - znear);
    m.m[3][2] = 1.0;
    return m;
}


/*
* Multiplies a 4x4 Projection Matrix by a 4x1 vector.
* The resulting 4x1 vector contains the positon of the
* object in 3D space relative to the field of view, aspect ratio,
* and distance from monitor to the end of the rendering plane.
* The final vector contains the normalized device coordinates.
*/
vec4_t mat4_mult_vec4_project(mat4_t m, vec4_t v) {
    vec4_t result = mat4_vec4_multiply(m, v);

    if (result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return result;
}

/*
* Multiplies a 4x4 Matrix by a 4x1 vector
*/
vec4_t mat4_vec4_multiply(mat4_t m, vec4_t v) {
    vec4_t r;
    r.x = (m.m[0][0] * v.x) + (m.m[0][1] * v.y) + (m.m[0][2] * v.z) + (m.m[0][3] * v.w);
    r.y = (m.m[1][0] * v.x) + (m.m[1][1] * v.y) + (m.m[1][2] * v.z) + (m.m[1][3] * v.w);
    r.z = (m.m[2][0] * v.x) + (m.m[2][1] * v.y) + (m.m[2][2] * v.z) + (m.m[2][3] * v.w);
    r.w = (m.m[3][0] * v.x) + (m.m[3][1] * v.y) + (m.m[3][2] * v.z) + (m.m[3][3] * v.w);
    return r;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t result = mat4_identity();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = (a.m[i][0] * b.m[0][j]) + (a.m[i][1] * b.m[1][j]) + (a.m[i][2] * b.m[2][j]) + (a.m[i][3] * b.m[3][j]);
        }
    }
    
    return result;
}