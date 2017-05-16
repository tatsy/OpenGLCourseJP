#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_binormal;

out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_tangentCameraSpace;
out vec3 f_binormalCameraSpace;
out vec3 f_lightPosCameraSpace;
out vec2 f_texcoord;

uniform vec3 u_lightPos;
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_tangentCameraSpace = (u_normMat * vec4(in_tangent, 0.0)).xyz;
    f_binormalCameraSpace = (u_normMat * vec4(in_binormal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    f_texcoord = in_texcoord;
}
