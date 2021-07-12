#ifndef DISPLAY_H
#define DISPLAY_H

#include "triangle.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

//////////////////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////////////////

bool initialize_window(void);
int get_window_width(void);
int get_window_height(void);
float get_z_buffer_at(int x, int y);
void set_z_buffer(int x, int y, float interpolated_reciprocal_w);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer();
void draw_grid(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(triangle_t triangle, uint32_t);
void draw_vertices(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void destroy_window(void);

#endif