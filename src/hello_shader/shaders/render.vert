#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

out vec3 fragColor;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(inPosition, 1.0);
    fragColor = inColor;
}
