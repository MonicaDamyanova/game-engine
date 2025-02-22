#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUVs;

out vec2 UVs;

uniform mat4 projection;
uniform mat4 model; // Moving and orientation
// uniform mat4 view; // Moving the camera

void main()
{
    UVs = aUVs;
    gl_Position = projection * model * vec4(aPos, 1.0);
}
