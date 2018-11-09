#version 450
layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 frag_position;

void main() {
    out_color = vec4(frag_position, 1.0);
}