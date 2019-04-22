#version 330

layout(location = 0) in vec3 in_position;

out vec3 f_positionOnCube;

uniform mat4 u_lightMat;

void main(void) {
    gl_Position = u_lightMat * vec4(in_position, 1.0);

    f_positionOnCube = in_position;
}
