#version 330

layout(location = 0) in vec3 inPosition;

out float fragHeight;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(inPosition, 1.0);
    fragHeight = inPosition.z;
}
