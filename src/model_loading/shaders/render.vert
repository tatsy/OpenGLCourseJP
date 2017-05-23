#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 f_fragColor;

uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // 今回は描画色を法線情報から決定
    f_fragColor = in_normal * 0.5 + 0.5;
}
