#include "vector.h"
#include <inttypes.h>

typedef struct {
    vec3_t direction;
} light_t;

void init_light(vec3_t direction);
vec3_t get_light_direction();
uint32_t light_apply_intensity(uint32_t original_color, float percentage_intensity);