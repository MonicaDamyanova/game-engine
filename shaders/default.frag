#version 330 core
out vec4 fragColor;

in vec2 UVs;

uniform sampler2D u_textureID;
uniform vec4 u_UVs;

void main() {
    //vec2 new_UVs = mix(u_UVs.xy, u_UVs.zw, UVs);
    fragColor = texture(u_textureID, UVs);
}
