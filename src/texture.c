#include "texture.h"

tex2_t tex2_clone(tex2_t* texture) {
    tex2_t result = {texture->u, texture->v};
    return result;
}
