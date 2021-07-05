#include <math.h>
#include "vector.h"

//////////////////////////////////////////////////////////////////////
// Vector 2D Functions
//////////////////////////////////////////////////////////////////////

// Pythagorean theorem for 2D Vector
float vec2_get_magnitude(vec2_t* v) {
    return sqrt(pow(v->x, 2) + pow(v->y, 2));
}

vec2_t vec2_add(vec2_t v_one, vec2_t v_two) {
    vec2_t result = {
        .x = v_one.x + v_two.x,
        .y = v_one.y + v_two.y
    };
    return result;
}

vec2_t vec2_subtract(vec2_t v_one, vec2_t v_two) {
    vec2_t result = {
        .x = v_one.x - v_two.x,
        .y = v_one.y - v_two.y
    };
    return result;  
}

void vec2_multiply(vec2_t* v, float scalar) {
    v->x *= scalar;
    v->y *= scalar;
}

void vec2_divide(vec2_t* v, float scalar) {
    v->x /= scalar;
    v->y /= scalar; 
}

float vec2_dot_product(vec2_t v_one, vec2_t v_two) {
    return (v_one.x * v_two.x) + (v_one.y * v_two.y);
}

void vec2_normalize(vec2_t* v) 
{
    float magnitude = vec2_get_magnitude(v);
    vec2_divide(v, magnitude);
}

//////////////////////////////////////////////////////////////////////
// Vector 3D Functions
//////////////////////////////////////////////////////////////////////

// Pythagorean theorem for 3D Vector
float vec3_get_magnitude(vec3_t* v) {
    return sqrt(pow(v->x, 2) + pow(v->y, 2) + pow(v->z, 2));
}

vec3_t vec3_add(vec3_t v_one, vec3_t v_two) {
    vec3_t result = {
        .x = v_one.x + v_two.x,
        .y = v_one.y + v_two.y,
        .z = v_one.z + v_two.z
    };
    return result;
}

vec3_t vec3_subtract(vec3_t v_one, vec3_t v_two) {
    vec3_t result = {
        .x = v_one.x - v_two.x,
        .y = v_one.y - v_two.y,
        .z = v_one.z - v_two.z
    };
    return result;    
}

void vec3_multiply(vec3_t* v, float scalar) {
    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;    
}

float vec3_dot_product(vec3_t v_one, vec3_t v_two) {
    return (v_one.x * v_two.x) + (v_one.y * v_two.y) + (v_one.z * v_two.z);
}

void vec3_divide(vec3_t* v, float scalar) {
    v->x /= scalar;
    v->y /= scalar;
    v->z /= scalar;      
}

void vec3_normalize(vec3_t* v) {
    float magnitude = vec3_get_magnitude(v);
    vec3_divide(v, magnitude);
}

vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t vecfour = {v.x, v.y, v.z, 1.0};
    return vecfour;
}

vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t vecthree = {v.x, v.y, v.z};
    return vecthree;
}

vec2_t vec2_from_vec4(vec4_t v) {
    vec2_t vectwo = {v.x, v.y};
    return vectwo;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;  
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;    
}

vec3_t vec3_cross_product(vec3_t v_one, vec3_t v_two) 
{
    vec3_t cross_product = {
        .x = (v_one.y * v_two.z) - (v_one.z * v_two.y),
        .y = (v_one.z * v_two.x) - (v_one.x * v_two.z),
        .z = (v_one.x * v_two.y) - (v_one.y * v_two.x)
    };
    return cross_product;
}

//////////////////////////////////////////////////////////////////////
// Convert Vectors of Different Dimensions
//////////////////////////////////////////////////////////////////////

float fov_factor = 640;

/*
* Orthogonal projection which converts a 3D vector to a 2D Vector
*/
vec2_t project(vec3_t point) {
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}