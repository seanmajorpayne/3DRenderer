#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},
    {.x = -1, .y =  1, .z = -1},
    {.x =  1, .y =  1, .z = -1},
    {.x =  1, .y = -1, .z = -1},
    {.x =  1, .y =  1, .z =  1},
    {.x =  1, .y = -1, .z =  1},
    {.x = -1, .y =  1, .z =  1},
    {.x = -1, .y = -1, .z =  1}
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},
    // right
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},
    // back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},
    // left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},
    // top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},
    // bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4}
};

void load_cube_mesh(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

void load_obj_file_data(char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        exit(EXIT_FAILURE);
    }
    char line[1000];
    
    while (fgets(line, 60, file)) {
        if (line[0] == 'v') {
            vec3_t vector;
            sscanf(line, "v %f %f %f\n", &vector.x, &vector.y, &vector.z);
            printf("%.2f\n", vector.x);
            printf("%.2f\n", vector.y);
            printf("%.2f\n", vector.z);
            array_push(mesh.vertices, vector);
        }

        else if (line[0] == 'f') {
            char* line_chunk;
            line_chunk = strtok(line, " ");

            face_t face;
            int face_vt;
            int face_vn;

            printf("%s\n", line_chunk);
            line_chunk = strtok(NULL, " ");
            printf("%s\n", line_chunk);       
            sscanf(line_chunk, "%d/%d/%d", &face.a, &face_vt, &face_vn);

            line_chunk = strtok(NULL, " ");
            printf("%s\n", line_chunk);
            sscanf(line_chunk, "%d/%d/%d", &face.b, &face_vt, &face_vn);

            line_chunk = strtok(NULL, " ");
            printf("%s\n", line_chunk);
            sscanf(line_chunk, "%d/%d/%d", &face.c, &face_vt, &face_vn);

            array_push(mesh.faces, face);
        }
    }
}

// TODO: Create functions for mesh