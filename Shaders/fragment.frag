#version 450

layout(location = 0) out vec4 out_color;
layout(location = 0) in vec4 frag_color;
layout(location = 1) in vec2 frag_uv;
layout(binding = 1) uniform sampler2D diffuseTexture;

void main() {
    out_color = texture(diffuseTexture, frag_uv);
}
