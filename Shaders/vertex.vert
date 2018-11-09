#version 450
layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 frag_position;

void main() {
    frag_position = in_position;
}