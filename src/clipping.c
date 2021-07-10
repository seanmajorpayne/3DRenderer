
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
