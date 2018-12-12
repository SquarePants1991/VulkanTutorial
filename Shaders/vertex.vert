#version 450
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 0) out vec4 frag_color;

layout(binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
    mat4 model;
} ubo;

void main() {
    frag_color = vec4(in_color, 1.0);
    vec4 pos = ubo.projection * ubo.view * ubo.model * vec4(in_position, 1.0);
    pos.y = -pos.y;
    pos.z = (pos.z + pos.w) / 2.0;
    gl_Position = pos;
}
