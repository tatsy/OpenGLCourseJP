#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 f_positionWorldSpace;
out vec3 f_cameraPosWorldSpace;
out vec3 f_normalWorldSpace;

uniform mat4 u_mvpMat;
uniform mat4 u_modelMat;
uniform vec3 u_cameraPos;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_positionWorldSpace = (u_modelMat * vec4(in_position, 1.0)).xyz;
    f_normalWorldSpace = (u_modelMat * vec4(in_normal, 0.0)).xyz;
    f_cameraPosWorldSpace = u_cameraPos;
}
