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
#include "light.h"
#include "texture.h"
#include "upng.h"
#include "camera.h"
#include "clipping.h"
#ifndef M_PI
#define M_PI (3.141592)
#endif

#define MAX_TRIANGLES_PER_MESH (10000)

bool is_running = false;
bool display_wireframe = true;
bool fill_triangles = false;
bool enable_culling = true;
bool display_vertices = false;
bool render_textures = false;

triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;
mat4_t projection_matrix;
int previous_frame_time = 0;

float delta_time = 0;

bool setup(void) {
    float aspect_x = (float) get_window_width() / (float) get_window_height();
    float fov_y = M_PI / 3.0; // 60 Degrees
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float aspect_y = (float) get_window_height() / (float) get_window_width();
    float z_near = 0.1;
    float z_far = 100.0;
    projection_matrix = mat4_make_projection(fov_y, aspect_y, z_far, z_near);

    init_light(vec3_new(0.0, 0.0, 1.0));
    // Initialize frustum planes with a point and normal

    init_frustum_planes(fov_x, fov_y, z_near, z_far);

    load_mesh(
        "./assets/f22.obj", 
        "./assets/f22.png",
        vec3_new(1.0, 1.0, 1.0), 
        vec3_new(-3.0, 0.0, 0.0),
        vec3_new(0.0, 0.0, 0.0)
    );

    load_mesh(
        "./assets/cube.obj", 
        "./assets/cube.png",
        vec3_new(1.0, 1.0, 1.0), 
        vec3_new(3.0, 0.0, 0.0),
        vec3_new(0.0, 0.0, 0.0)
    );
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
                case SDLK_4:
                    render_textures = !render_textures;
                    break;
                case SDLK_w:
                    move_camera_forward(delta_time);
                    break;
                case SDLK_s:
                    move_camera_backwards(delta_time);
                    break;
                case SDLK_a:
                    increase_camera_yaw(delta_time);
                    break;
                case SDLK_d:
                    decrease_camera_yaw(delta_time);
                    break;
                case SDLK_UP:
                    increase_camera_pitch(delta_time);
                    break;
                case SDLK_DOWN:
                    decrease_camera_pitch(delta_time);
                    break;
                case SDLK_q:
                    increase_camera_y_pos(delta_time);
                    break;
                case SDLK_e:
                    decrease_camera_y_yaw(delta_time);
                    break;
            }
            break;
    }
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Get delta time in seconds
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;
    previous_frame_time = SDL_GetTicks();

    // Reset triangles to render per frame
    num_triangles_to_render = 0;

    // Create the view matrix
    mat4_t camera_yaw_rotation = mat4_rotate_y(get_camera_yaw());
    mat4_t camera_pitch_rotation = mat4_rotate_x(get_camera_pitch());
    vec3_t target = update_target(camera_yaw_rotation, camera_pitch_rotation);
    vec3_t up_direction = {0, 1, 0};
    mat4_t view_matrix = get_view_matrix(target, up_direction);

    for (int mesh_index = 0; mesh_index < get_mesh_count(); mesh_index++) {
        // Create scale matrix to multiply mesh vertices
        mesh_t* mesh = get_mesh_at(mesh_index);

        // Move object away from camera
        mesh->translation.y = -0.3;
        mesh->translation.z = 5.0;


        mat4_t scale_matrix = mat4_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
        mat4_t translation_matrix = mat4_translate(mesh->translation.x, mesh->translation.y, mesh->translation.z);
        mat4_t rotation_matrix_z = mat4_rotate_z(mesh->rotation.z);
        mat4_t rotation_matrix_x = mat4_rotate_x(mesh->rotation.x);
        mat4_t rotation_matrix_y = mat4_rotate_y(mesh->rotation.y);

        // Create world matrix containing all scaling, rotation and translation
        mat4_t world_matrix = get_world_matrix(scale_matrix, translation_matrix, rotation_matrix_x, rotation_matrix_y, rotation_matrix_z);

        int num_faces = array_length(mesh->faces);
        for (int i = 0; i < num_faces; i++) {
            face_t mesh_face = mesh->faces[i];
            vec3_t face_vertices[3];
            face_vertices[0] = mesh->vertices[mesh_face.a];
            face_vertices[1] = mesh->vertices[mesh_face.b];
            face_vertices[2] = mesh->vertices[mesh_face.c];

            vec4_t transformed_vertices[3];

            for (int j = 0; j < 3; j++) {
                vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

                transformed_vertex = mat4_vec4_multiply(world_matrix, transformed_vertex);

                transformed_vertex = mat4_vec4_multiply(view_matrix, transformed_vertex);

                transformed_vertices[j] = transformed_vertex;
            }
            
            // Calculate the normal vector for the face
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
            vec3_t vector_ab = vec3_subtract(vector_b, vector_a);
            vec3_t vector_ac = vec3_subtract(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);
            vec3_t normal_vector = vec3_cross_product(vector_ab, vector_ac);
            vec3_normalize(&normal_vector);
            
            /* If the user has backface culling enabled, calculate the angle of the camera
            * and the face's normal vector. If this is non-negative, the face should be
            * rendered.
            */
            if (enable_culling) {
                vec3_t origin = {0, 0, 0};
                vec3_t camera_vector = vec3_subtract(origin, vector_a);
                float visible = vec3_dot_product(normal_vector, camera_vector);
                if (visible < 0) {
                    continue;
                }
            }

            // Clipping
            polygon_t polygon = create_polygon_from_triangle(
                vec3_from_vec4(transformed_vertices[0]), 
                vec3_from_vec4(transformed_vertices[1]), 
                vec3_from_vec4(transformed_vertices[2]),
                mesh_face.a_uv,
                mesh_face.b_uv,
                mesh_face.c_uv
            );

            clip_polygon(&polygon);

            triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
            int num_triangles_after_clipping = 0;

            triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

            // Loop all assembled triangles after clipping
            for (int t = 0; t < num_triangles_after_clipping; t++) {
                triangle_t triangle_after_clipping = triangles_after_clipping[t];
                vec4_t projected_points[3];

                for (int j = 0; j < 3; j++) {
                    projected_points[j] = mat4_mult_vec4_project(projection_matrix, triangle_after_clipping.points[j]);

                    // Invert Y Values to account for flipped screen y coordinate

                    projected_points[j].y *= -1.0;

                    // Scale point to middle of screen
                    projected_points[j].x *= (get_window_width() / 2.0);
                    projected_points[j].y *= (get_window_height() / 2.0);

                    // Translate point to middle of screen
                    projected_points[j].x += (get_window_width() / 2.0);
                    projected_points[j].y += (get_window_height() / 2.0);
                }
                /* Calculate the alignment of the sun with the face
                *  and use the alignment to determine the amount of shading
                *  to apply to the face.
                */
                float face_light_alignment = -vec3_dot_product(normal_vector, get_light_direction());
                uint32_t triangle_color = light_apply_intensity(mesh_face.color, face_light_alignment);

                triangle_t triangle_to_render = {
                    .points = {
                        {projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
                        {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
                        {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w},
                    },
                    .tex_coords = {
                        {triangle_after_clipping.tex_coords[0].u, triangle_after_clipping.tex_coords[0].v},
                        {triangle_after_clipping.tex_coords[1].u, triangle_after_clipping.tex_coords[1].v},
                        {triangle_after_clipping.tex_coords[2].u, triangle_after_clipping.tex_coords[2].v}
                    },
                    .color = triangle_color,
                    .texture = mesh->texture
                };
                if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                    triangles_to_render[num_triangles_to_render++] = triangle_to_render;
                }
            }
        }
    }
}

void render(void) {

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    draw_grid();
    
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (fill_triangles) {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.color
            );
        }

        if (render_textures) {
            draw_textured_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.tex_coords[0].u,
                triangle.tex_coords[0].v,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.tex_coords[1].u,
                triangle.tex_coords[1].v,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.tex_coords[2].u,
                triangle.tex_coords[2].v,
                triangle.texture
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

    render_color_buffer();
}

void free_resources(void) {
    free_meshes();
}

int main(void) {
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