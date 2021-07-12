
#include "clipping.h"
#include <math.h>

#define NUM_PLANES 6

plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov_x, float fov_y, float z_near, float z_far) {
    float cos_half_fov_x = cos(fov_x / 2);
    float sin_half_fov_x = sin(fov_x / 2);    
    float cos_half_fov_y = cos(fov_y / 2);
    float sin_half_fov_y = sin(fov_y / 2);

    vec3_t origin = {0, 0, 0};

    frustum_planes[LEFT_FRUSTRUM_PLANE].point = origin;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.x = cos_half_fov_x;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTRUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[RIGHT_FRUSTRUM_PLANE].point = origin;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.x = -cos_half_fov_x;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTRUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[TOP_FRUSTRUM_PLANE].point = origin;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.y = -cos_half_fov_y;
    frustum_planes[TOP_FRUSTRUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[BOTTOM_FRUSTRUM_PLANE].point = origin;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.y = cos_half_fov_y;
    frustum_planes[BOTTOM_FRUSTRUM_PLANE].normal.z = sin_half_fov_y;

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

polygon_t create_polygon_from_triangle(vec3_t a, vec3_t b, vec3_t c, tex2_t at, tex2_t bt, tex2_t ct) {
    polygon_t polygon = {
        .vertices = {a, b, c},
        .tex_coords = {at, bt, ct},
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

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertices[MAX_POLY_VERTICES];
    tex2_t inside_tex_coords[MAX_POLY_VERTICES];
    int num_inside_vertices = 0;

    vec3_t* current_vertex = &polygon->vertices[0];
    tex2_t* current_tex_coord = &polygon->tex_coords[0];
    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
    tex2_t* previous_tex_coord = &polygon->tex_coords[polygon->num_vertices - 1];

    float current_dot = 0;
    float previous_dot = vec3_dot_product(vec3_subtract(*previous_vertex, plane_point), plane_normal);

    while(current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3_dot_product(vec3_subtract(*current_vertex, plane_point), plane_normal);

        if (current_dot * previous_dot < 0) {
            //Calculate interpolation factor
            float t = previous_dot / (previous_dot - current_dot);
            //Calculate the intersection point

            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t)
            }; 
            
            //= vec3_clone(current_vertex);
            //intersection_point = vec3_subtract(intersection_point, *previous_vertex);
            //intersection_point = vec3_multiply(intersection_point, t);
            //intersection_point = vec3_add(intersection_point, *previous_vertex);

            tex2_t interpolated_tex_coord = {
                .u = float_lerp(previous_tex_coord->u, current_tex_coord->u, t),
                .v = float_lerp(previous_tex_coord->v, current_tex_coord->v, t)
            };
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_tex_coords[num_inside_vertices] = tex2_clone(&interpolated_tex_coord);
            num_inside_vertices++;
        }

        // Inside Plane
        if (current_dot > 0) {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_tex_coords[num_inside_vertices] = tex2_clone(current_tex_coord);
            num_inside_vertices++;
        }
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_tex_coord = current_tex_coord;
        current_vertex++;
        current_tex_coord++;
    }
    
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->tex_coords[i] = tex2_clone(&inside_tex_coords[i]);
    }
    polygon->num_vertices = num_inside_vertices;
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles) {
    for (int i = 1; i < polygon->num_vertices - 1; i++) {
        triangles[i - 1].points[0] = vec4_from_vec3(polygon->vertices[0]);
        triangles[i - 1].points[1] = vec4_from_vec3(polygon->vertices[i]);
        triangles[i - 1].points[2] = vec4_from_vec3(polygon->vertices[i + 1]);
        triangles[i - 1].tex_coords[0] = polygon->tex_coords[0];
        triangles[i - 1].tex_coords[1] = polygon->tex_coords[i];
        triangles[i - 1].tex_coords[2] = polygon->tex_coords[i + 1];
    }
    *num_triangles = polygon->num_vertices - 2;
}