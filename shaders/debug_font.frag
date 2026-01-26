#version 330 core
out vec4 fragColor;

in vec2 UVs;

uniform sampler2D textureID;
uniform vec4 uniform_UVs;
uniform vec4 colour;

void main() {
    //vec4 colour = vec4(1.0, 0.0, 1.0, 1.0);
    vec2 new_UVs = mix(uniform_UVs.xy, uniform_UVs.zw, UVs);
    float a = 1.0 - texture(textureID, new_UVs).r;

    fragColor = vec4(colour.rgb, a);
}
