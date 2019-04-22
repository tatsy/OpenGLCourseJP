#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0) out vec3 f_posViewSpace;
layout(location = 1) out vec3 f_normViewSpace;
layout(location = 2) out vec3 f_lightPosViewSpace;
layout(location = 3) out vec2 f_texcoord;

uniform mat4 u_lightMat;
uniform mat4 u_mvpMat;
uniform mat4 u_mvMat;
uniform mat4 u_normMat;
uniform vec3 u_lightPos;

void main(void) {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    f_posViewSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normViewSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosViewSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    f_texcoord = in_texcoord;
}
