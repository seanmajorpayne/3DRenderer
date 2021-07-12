#include "display.h"
#define GREATER_ABS_VALUE(x, y) (abs(x) >= abs(y) ? abs(x) : abs(y))

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* color_buffer = NULL;
static float* z_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static int window_width = 0;
static int window_height = 0;

int get_window_width(void) {
    return window_width;
}

int get_window_height(void) {
    return window_height;
}

float get_z_buffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return z_buffer[(window_width * y) + x];
}

void set_z_buffer(int x, int y, float interpolated_reciprocal_w) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    z_buffer[(window_width * y) + x] = interpolated_reciprocal_w;
}

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }

    // TODO: Create an SDL Renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) {
        puts("No memory.\n");
        return false;
    }
    z_buffer = (float*) malloc(sizeof(float) * window_width * window_height);
    if (!z_buffer) {
        puts("No memory.\n");
        return false;
    }
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    return true;
}

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        window_width * sizeof(uint32_t)
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void clear_color_buffer(uint32_t color) {
    for (int x = 0; x < window_height * window_width; x++) {
        color_buffer[x] = color;
    }
}

void clear_z_buffer() {
    for (int x = 0; x < window_height * window_width; x++) {
        z_buffer[x] = 1.0;
    }
}

void draw_grid(void) {
    for (int x = 0; x < window_width; x+=10) {
        for (int y = 0; y < window_height; y+=10) {
            if (y % 10 == 0 || x % 10 == 0) {
                color_buffer[(window_width * y) + x] = 0xFF333333;
            }
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    color_buffer[(window_width * y) + x] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            draw_pixel(i, j, color);
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_y = y1 - y0;
    int delta_x = x1 - x0;

    int distance = GREATER_ABS_VALUE(delta_x, delta_y);

    float x_inc = delta_x / (float) distance;
    float y_inc = delta_y / (float) distance;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= distance; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_triangle(triangle_t triangle, uint32_t color) {
    draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, color);
    draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, color);
    draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x, triangle.points[0].y, color);
}

void draw_vertices(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_rect(x0, y0, 5, 5, color);
    draw_rect(x1, y1, 5, 5, color);
    draw_rect(x2, y2, 5, 5, color);
}

void destroy_window(void) {
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}