#version 330 core
out vec4 fragColor;

in vec2 UVs;

uniform sampler2D u_textureID;
uniform vec4 u_UVs;
uniform vec4 u_colour;

void main() {
    vec2 new_UVs = mix(u_UVs.xy, u_UVs.zw, UVs);
    float a = 1.0 - texture(u_textureID, new_UVs).r;

    fragColor = vec4(u_colour.rgb, a);
}
