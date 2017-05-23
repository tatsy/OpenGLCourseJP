#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

out vec3 f_fragColor;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    f_fragColor = in_color;
}
