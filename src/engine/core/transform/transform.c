#include "transform.h"

#include <cglm/cglm.h>

#include <stdbool.h>

void transform_matrix(Transform2D * t, mat4 out) {
    glm_mat4_identity(out);
    glm_translate(out, (vec3){t->position[0], t->position[1], 0.0f});
    glm_rotate_z(out, t->rotation, out);
    glm_scale(out, (vec3){t->scale[0], t->scale[1], 1.0f});
}

void transform_set_position(Transform2D * t, vec2 pos) {
    t->position[0] = pos[0];
    t->position[1] = pos[1];
    t->dirty = true;
}

void transform_set_scale(Transform2D * t, vec2 scale) {
    t->scale[0] = scale[0];
    t->scale[1] = scale[1];
    t->dirty = true;
}

void transform_set_rotation(Transform2D * t, float rot) {
    t->rotation = rot;
    t->dirty = true;
}

void transform_move(Transform2D * t, vec2 vel, float dt) {
    t->position[0]+= vel[0] * dt; 
    t->position[1] += vel[1] * dt;
    t->dirty = true;
}

void transform_scale(Transform2D * t, vec2 scale, float dt) {
    t->scale[0] += scale[0] * dt; 
    t->scale[1] += scale[1] * dt;
    t->dirty = true;
}

void transform_rotate(Transform2D * t, float rot, float dt) {
    t->rotation += rot * dt; 
    t->dirty = true;
}
