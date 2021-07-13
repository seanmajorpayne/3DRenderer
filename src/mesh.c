#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_obj_file_data(mesh_t* mesh, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        exit(EXIT_FAILURE);
    }
    char line[1000];

    tex2_t* texcoords = NULL;
    
    while (fgets(line, 1024, file)) {
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vector;
            sscanf(line, "v %f %f %f\n", &vector.x, &vector.y, &vector.z);
            array_push(mesh->vertices, vector);
        }

        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }

        if (strncmp(line, "f ", 2) == 0) {
            face_t face;
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];

            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );

            face.color = 0xFFFFFFFF;
            face.a = vertex_indices[0] - 1;
            face.b = vertex_indices[1] - 1;
            face.c = vertex_indices[2] - 1;
            face.a_uv = texcoords[texture_indices[0] - 1];
            face.b_uv = texcoords[texture_indices[1] - 1];
            face.c_uv = texcoords[texture_indices[2] - 1];
            array_push(mesh->faces, face);
        }
    }
    array_free(texcoords);
}

int get_mesh_count() {
    return mesh_count;
}

mesh_t* get_mesh_at(int mesh_index) {
    return &meshes[mesh_index];
}

void free_meshes() {
    for (int i = 0; i < mesh_count; i++) {
        array_free(meshes[i].faces);
        array_free(meshes[i].vertices);
        upng_free(meshes[i].texture);
    }
}

void load_png_texture_data(mesh_t* mesh, char* filename) {
    upng_t* png_image = upng_new_from_file(filename);
    if (png_image != NULL) {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK) {
            mesh->texture = png_image;
        }
    }
}

void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation) {
    load_obj_file_data(&meshes[mesh_count], obj_filename);
    load_png_texture_data(&meshes[mesh_count], png_filename);
    meshes[mesh_count].scale = scale;
    meshes[mesh_count].translation = translation;
    meshes[mesh_count].rotation = rotation;
    mesh_count++;
}
