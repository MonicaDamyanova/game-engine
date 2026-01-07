#pragma once

#include <cglm/cglm.h>

#include <stdbool.h>

typedef struct {
    vec2 position;
    vec2 scale;
    float rotation;
    bool dirty;
} Transform2D;
        

void transform_matrix(Transform2D * t, mat4 out);

void transform_set_position(Transform2D * t, vec2 pos);
void transform_set_scale(Transform2D * t, vec2 scale);
void transform_set_rotation(Transform2D * t, float rot);

void transform_move(Transform2D * t, vec2 vel, float dt);
void transform_scale(Transform2D * t, vec2 scale, float dt);
void transform_rotate(Transform2D * t, float rot, float dt);
