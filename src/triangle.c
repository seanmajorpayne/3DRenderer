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

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
    }
    if (y1 > y2) {
        swap(&y1, &y2);
        swap(&x1, &x2);
    }
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
    }

    if (y1 == y2) {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    }

    else if (y0 == y1) {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }

    else {
        int mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;
        int my = y1;

        // Draw flat-bottom
        fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, color);

        // Draw flat-top
        fill_flat_top_triangle(x1, y1, mx, my, x2, y2, color);
    }
}

/*
* Find the inverse slope of both sides of the triangle.
* Use the slopes to calculate an x_start and x_end and fill in
* the pixels between them.
*/
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float slope_one = (float) (x1 - x0) / (y1 - y0);
    float slope_two = (float) (x2 - x0) / (y2 - y0);

    float x_start = x0;
    float x_end = x0;
    for (int y = y0; y <= y2; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += slope_one;
        x_end += slope_two;
    }

}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    float slope_one = (float) (x2 - x0) / (y2 - y0);
    float slope_two = (float) (x2 - x1) / (y2 - y1);

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y0; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= slope_one;
        x_end -= slope_two;
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

    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));
    draw_pixel(x, y, texture[texture_width * tex_y + tex_x]);
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

    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

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