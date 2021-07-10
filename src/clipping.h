#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"

#define MAX_POLY_VERTICES (12)

enum {
    LEFT_FRUSTRUM_PLANE,
    RIGHT_FRUSTRUM_PLANE,
    TOP_FRUSTRUM_PLANE,
    BOTTOM_FRUSTRUM_PLANE,
    NEAR_FRUSTRUM_PLANE,
    FAR_FRUSTRUM_PLANE
};

typedef struct {
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct {
    vec3_t vertices[MAX_POLY_VERTICES];
    int num_vertices;
} polygon_t;

void init_frustum_planes(float fov, float z_near, float z_far);
polygon_t create_polygon_from_triangle(vec3_t a, vec3_t b, vec3_t c);
void clip_polygon(polygon_t* polygon);

#endif