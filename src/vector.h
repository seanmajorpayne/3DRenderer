#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x, y;
} vec2_t;

typedef struct {
    float x, y, z;
} vec3_t;

typedef struct {
    float x, y, z, w;
} vec4_t;

//////////////////////////////////////////////////////////////////////
// Vector 2D Functions
//////////////////////////////////////////////////////////////////////

float vec2_get_magnitude(vec2_t* v);
vec2_t vec2_add(vec2_t v_one, vec2_t v_two);
vec2_t vec2_subtract(vec2_t v_one, vec2_t v_two);
void vec2_multiply(vec2_t* v, float scalar);
void vec2_divide(vec2_t* v, float scalar);
float vec2_dot_product(vec2_t v_one, vec2_t v_two);
void vec2_normalize(vec2_t* v);

//////////////////////////////////////////////////////////////////////
// Vector 3D Functions
//////////////////////////////////////////////////////////////////////

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);
vec3_t vec3_cross_product(vec3_t v_one, vec3_t v_two);
float vec3_get_magnitude(vec3_t* v);
vec3_t vec3_add(vec3_t v_one, vec3_t v_two);
vec3_t vec3_subtract(vec3_t v_one, vec3_t v_two);
vec3_t vec3_multiply(vec3_t v, float scalar);
float vec3_dot_product(vec3_t v_one, vec3_t v_two);
void vec3_divide(vec3_t* v, float scalar);
void vec3_normalize(vec3_t* v);
vec3_t vec3_clone(vec3_t* v);

//////////////////////////////////////////////////////////////////////
// Vector Conversion Functions
//////////////////////////////////////////////////////////////////////

vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);
vec2_t vec2_from_vec4(vec4_t v);

//////////////////////////////////////////////////////////////////////
// Convert Vectors of Different Dimensions
//////////////////////////////////////////////////////////////////////

vec2_t project(vec3_t point);

#endif