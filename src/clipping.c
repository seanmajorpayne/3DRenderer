
#include "clipping.h"

#define NUM_PLANES 6

plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov, float z_near, float z_far) {
    float cos_half_fov = cos(fov / 2);
    float sin_half_fov = sin(fov / 2);

    vec3_t origin = {0, 0, 0};

    frustum_planes[LEFT_FRUSTRUM_PLANE].point = origin;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[RIGHT_FRUSTRUM_PLANE].point = origin;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.x = -cos_half_fov;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[TOP_FRUSTRUM_PLANE].point = origin;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.y = -cos_half_fov;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[BOTTOM_FRUSTRUM_PLANE].point = origin;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.y = cos_half_fov;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[NEAR_FRUSTRUM_PLANE].point.x = 0;
    frustum_planes[NEAR_FRUSTRUM_PLANE].point.y = 0;
    frustum_planes[NEAR_FRUSTRUM_PLANE].point.z = z_near;
    frustum_planes[NEAR_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTRUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTRUM_PLANE].point.x = 0;
    frustum_planes[FAR_FRUSTRUM_PLANE].point.y = 0;
    frustum_planes[FAR_FRUSTRUM_PLANE].point.z = z_far;
    frustum_planes[FAR_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTRUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t a, vec3_t b, vec3_t c) {
    polygon_t polygon = {
        .vertices = {a, b, c},
        .num_vertices = 3
    };
    return polygon;
}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTRUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTRUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTRUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTRUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTRUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTRUM_PLANE);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertices[MAX_POLY_VERTICES];
    int num_inside_vertices = 0;

    vec3_t* current_vertex = &polygon->vertices[0];
    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];

    float current_dot = vec3_dot_product(vec3_subtract(*current_vertex, plane_point), plane_normal);
    float previous_dot = vec3_dot_product(vec3_subtract(*previous_vertex, plane_point), plane_normal);

    while(current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3_dot_product(vec3_subtract(*current_vertex, plane_point), plane_normal);

        if (current_dot * previous_dot < 0) {
            //Calculate interpolation factor
            //Calculate the intersection point
            vec3_t intersection_point;

            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            num_inside_vertices++;
        }

        // Inside Plane
        if (current_dot > 0) {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            num_inside_vertices++;
        }
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        current_vertex++;
    }
}
