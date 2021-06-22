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
    vec2_t a, vec2_t b, vec2_t c,
    float u0, float v0, float u1, float v1, float u2, float v2
) {
    vec2_t point_p = {x, y};
    vec3_t weights = barycentric_weights(a, b, c, point_p);

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u = u0 * alpha + u1 * beta + u2 * gamma;
    float interpolated_v = v0 * alpha + v1 * beta + v2 * gamma;

    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_height));
    draw_pixel(x, y, texture[texture_width * tex_y + tex_x]);
}

void draw_textured_triangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t* texture) {

    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }
    if (y1 > y2) {
        swap(&y1, &y2);
        swap(&x1, &x2);
        swap_float(&u1, &u2);
        swap_float(&v1, &v2);
    }
    if (y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_float(&u0, &u1);
        swap_float(&v0, &v1);
    }

    // Create vector points
    vec2_t point_a = {x0, y0};
    vec2_t point_b = {x1, y1};
    vec2_t point_c = {x2, y2};

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
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
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
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
            }
        }   

    }  
}