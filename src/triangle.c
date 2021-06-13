#include "triangle.h"
#include "display.h"

/*
* Swaps two integer values
*/

void swap(int* a, int* b) {
    int t = *a;
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