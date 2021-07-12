#include "triangle.h"
#include "display.h"
#include <math.h>

/*
* Swaps two integer values
*/

void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void swap_float(float* a, float* b) {
    float t = *a;
    *a = *b;
    *b = t;
}

/*
* Vertices need to be sorted for flat-bottom flat-top
* methods of drawing triangles. Sorted by Y position
*/

void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2, uint32_t color) {
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
    }
    if (y1 > y2) {
        swap(&y1, &y2);
        swap(&x1, &x2);
        swap_float(&z1, &z2);
        swap_float(&w1, &w2);
    }
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
    }

    vec2_t a = {x0, y0};
    vec2_t b = {x1, y1};
    vec2_t c = {x2, y2};

    // Flat Bottom Triangle

    float slope_one = 0;
    float slope_two = 0;

    if (y1 - y0 != 0) slope_one = (float) (x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) slope_two = (float) (x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * slope_one;
            int x_end = x0 + (y - y0) * slope_two;

            if (x_start > x_end) {
                swap(&x_start, &x_end);
            }
            
            for (int x = x_start; x < x_end; x++) {
                vec2_t p = {x, y};
                vec3_t weights = barycentric_weights(a, b, c, p);
                float alpha = weights.x;
                float beta = weights.y;
                float gamma = weights.z;
                float interpolated_reciprocal_w = (1 / w0) * alpha + (1 / w1) * beta + (1 / w2) * gamma;
                interpolated_reciprocal_w = 1 - interpolated_reciprocal_w;
                if (interpolated_reciprocal_w < get_z_buffer_at(x, y)){
                    draw_pixel(x, y, color);

                    set_z_buffer(x, y, interpolated_reciprocal_w);
                }
            }
        }
    }

    // Flat Top Triangle

    slope_one = 0;
    slope_two = 0;

    if (y2 - y1 != 0) slope_one = (float) (x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) slope_two = (float) (x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {

        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * slope_one;
            int x_end = x0 + (y - y0) * slope_two;

            if (x_start > x_end) {
                swap(&x_start, &x_end);
            }
            
            for (int x = x_start; x < x_end; x++) {
                vec2_t p = {x, y};
                vec3_t weights = barycentric_weights(a, b, c, p);
                float alpha = weights.x;
                float beta = weights.y;
                float gamma = weights.z;
                float interpolated_reciprocal_w = (1 / w0) * alpha + (1 / w1) * beta + (1 / w2) * gamma;
                interpolated_reciprocal_w = 1 - interpolated_reciprocal_w;
                if (interpolated_reciprocal_w < get_z_buffer_at(x, y)){
                    draw_pixel(x, y, color);

                    set_z_buffer(x, y, interpolated_reciprocal_w);
                }
            }
        }   

    }  
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    vec2_t ab = vec2_subtract(b, a);
    vec2_t bc = vec2_subtract(c, b);
    vec2_t ac = vec2_subtract(c, a);
    vec2_t ap = vec2_subtract(p, a);
    vec2_t bp = vec2_subtract(p, b);

    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;
    float gamma = 1 - alpha - beta;
    vec3_t weights = {alpha, beta, gamma};
    return weights;
}

void draw_texel(
    int x, int y, uint32_t* texture,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
    //float u0, float v0, float u1, float v1, float u2, float v2
) {
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);
    vec3_t weights = barycentric_weights(a, b, c, p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w; // 1/w

    // interpolation of u/w and v/w using barycentric weights and factor 1/w
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Divide back both interpolated values
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    // Must invert so smaller values are closer to the viewer
    interpolated_reciprocal_w = 1 - interpolated_reciprocal_w;

    // Only render pixels that are in the front of the z-plane
    if (interpolated_reciprocal_w < get_z_buffer_at(x, y)) {
        draw_pixel(x, y, texture[texture_width * tex_y + tex_x]);
        
        // Update z-buffer with 1/w of current pixel
        set_z_buffer(x, y, interpolated_reciprocal_w);
    }
}

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t* texture) {

    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }
    if (y1 > y2) {
        swap(&y1, &y2);
        swap(&x1, &x2);
        swap_float(&z1, &z2);
        swap_float(&w1, &w2);
        swap_float(&u1, &u2);
        swap_float(&v1, &v2);
    }
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&z0, &z1);
        swap_float(&w0, &w1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }

    // Create vector points and texture coords for each vertices
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

    // Invert V to accomodate for obj file format
    tex2_t a_uv = {u0, 1.0 - v0};
    tex2_t b_uv = {u1, 1.0 - v1};
    tex2_t c_uv = {u2, 1.0 - v2};

    // Flat Bottom Triangle

    float slope_one = 0;
    float slope_two = 0;

    if (y1 - y0 != 0) slope_one = (float) (x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) slope_two = (float) (x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * slope_one;
            int x_end = x0 + (y - y0) * slope_two;

            if (x_start > x_end) {
                swap(&x_start, &x_end);
            }
            
            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    } 

    // Flat Top Triangle

    slope_one = 0;
    slope_two = 0;

    if (y2 - y1 != 0) slope_one = (float) (x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) slope_two = (float) (x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {

        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * slope_one;
            int x_end = x0 + (y - y0) * slope_two;

            if (x_start > x_end) {
                swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++) {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }   

    }  
}