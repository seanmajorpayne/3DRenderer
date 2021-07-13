#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

//////////////////////////////////////////////////////////////////////
// Mesh
//////////////////////////////////////////////////////////////////////

typedef struct {
    vec3_t* vertices;
    face_t* faces;
    upng_t* texture;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;
} mesh_t;

void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_png_texture_data(mesh_t* mesh, char* png_filename);
void load_obj_file_data(mesh_t* mesh, char* filename);
int get_mesh_count();
mesh_t* get_mesh_at(int mesh_index);
void free_meshes();

#endif
