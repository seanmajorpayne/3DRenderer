#include "light.h"
#include <inttypes.h>

static light_t light;

void init_light(vec3_t direction) {
    light.direction = direction;
}

vec3_t get_light_direction() {
    return light.direction;
}

uint32_t light_apply_intensity(uint32_t original_color, float percentage_intensity) {
    if (percentage_intensity < 0.0) { percentage_intensity = 0.0; }
    if (percentage_intensity > 1.0) { percentage_intensity = 1.0; }

    uint32_t a = 0xFF000000 & original_color;
    uint32_t r = (0x00FF0000 & original_color) * percentage_intensity;
    uint32_t g = (0x0000FF00 & original_color) * percentage_intensity;
    uint32_t b = (0x000000FF & original_color) * percentage_intensity;

    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
    return new_color; 
}