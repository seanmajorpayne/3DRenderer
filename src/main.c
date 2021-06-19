#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <math.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#ifndef M_PI
#define M_PI (3.141592)
#endif

bool is_running = false;
bool display_wireframe = true;
bool fill_triangles = false;
bool enable_culling = true;
bool display_vertices = false;

triangle_t* triangles_to_render = NULL;
vec3_t camera_position = {.x = 0, .y = 0, .z = 0};
mat4_t projection_matrix;
int previous_frame_time = 0;

bool setup(void) {
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) {
        puts("No memory.\n");
        return false;
    }
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    float fov = M_PI / 3.0;                     // 60 Degrees
    float aspect = (float) window_height / (float) window_width;
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_make_projection(fov, aspect, zfar, znear);

    load_cube_mesh();
    //load_obj_file_data("./assets/f22.obj");
    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_running = false;
                    break;
                case SDLK_1:
                    display_wireframe = !display_wireframe;
                    break;
                case SDLK_2:
                    fill_triangles = !fill_triangles;
                    break;
                case SDLK_c:
                    enable_culling = !enable_culling;
                    break;
                case SDLK_3:
                    display_vertices = !display_vertices;
                    break;
            }
            break;
    }
}

void merge(int low, int mid, int high)
{
    int merge_size = high - low + 1;
    triangle_t *temp = malloc(merge_size * sizeof(triangle_t));
    int merge_pos = 0;
    int left_pos = low;
    int right_pos = mid + 1;

    while (left_pos <= mid && right_pos <= high)
    {

        if (triangles_to_render[left_pos].avg_depth > triangles_to_render[right_pos].avg_depth)
        {
            temp[merge_pos++] = triangles_to_render[left_pos++];
        }
        else
        {
            temp[merge_pos++] = triangles_to_render[right_pos++];
        }
    }

    while (left_pos <= mid)
    {
        temp[merge_pos++] = triangles_to_render[left_pos++];
    }

    while (right_pos <= high)
    {
        temp[merge_pos++] = triangles_to_render[right_pos++];
    }

    for (merge_pos = 0; merge_pos < merge_size; ++merge_pos)
        triangles_to_render[low + merge_pos] = temp[merge_pos];

    free(temp);
}

void merge_sort(int low, int high)
{
    if (low < high)
    {
        int mid = (low + high) / 2;

        merge_sort(low, mid);
        merge_sort(mid + 1, high);

        merge(low, mid, high);
    }
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    // Initialize the array of triangles to render
    triangles_to_render = NULL;

    mesh.rotation.x += 0.001;
    //mesh.rotation.y += 0.001;
    //mesh.rotation.z += 0.001;

    // Move object away from camera
    mesh.translation.z = 5.0;

    //mesh.scale.x += 0.002;

    //mesh.translation.x += 0.005;

    // Create scale matrix to multiply mesh vertices
    mat4_t scale_matrix = mat4_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_translate(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_z = mat4_rotate_z(mesh.rotation.z);
    mat4_t rotation_matrix_x = mat4_rotate_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_rotate_y(mesh.rotation.y);

    // Create world matrix containing all scaling, rotation and translation
    mat4_t world_matrix = get_world_matrix(scale_matrix, translation_matrix, rotation_matrix_x, rotation_matrix_y, rotation_matrix_z);

    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            transformed_vertex = mat4_vec4_multiply(world_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }
        
        float visible = 1.f;
        if (enable_culling) {
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
            vec3_t vector_ab = vec3_subtract(vector_b, vector_a);
            vec3_t vector_ac = vec3_subtract(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);
            vec3_t normal_vector = vec3_cross_product(vector_ab, vector_ac);
            vec3_normalize(&normal_vector);
            vec3_t camera_vector = vec3_subtract(camera_position, vector_a);
            visible = vec3_dot_product(normal_vector, camera_vector);
        }
        

        if (visible < 0) {
            continue;
        }

        vec4_t projected_points[3];

        for (int j = 0; j < 3; j++) {
            projected_points[j] = mat4_mult_vec4_project(projection_matrix, transformed_vertices[j]);
        
            // Scale point to middle of screen
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // Translate point to middle of screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calculate average depth after tranformed vertices

        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

        triangle_t projected_triangle = {
            .points = {
                {projected_points[0].x, projected_points[0].y},
                {projected_points[1].x, projected_points[1].y},
                {projected_points[2].x, projected_points[2].y},
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };
        array_push(triangles_to_render, projected_triangle);
    }

    int num_triangles = array_length(triangles_to_render) - 1;
    merge_sort(0, num_triangles);
}

void render(void) {
    draw_grid();
    
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (fill_triangles) {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.color
            );
        }

        if (display_wireframe) {
            draw_triangle(triangle, 0xFF0000FF);
        }

        if (display_vertices) {
            draw_vertices(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFF0000
            );
        }
    }

    // Clear the array of triangles
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main(void) {
    /* TODO: Create SDL Window */
    is_running = initialize_window();

    if(!setup()) {
        return -1;
    };

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}