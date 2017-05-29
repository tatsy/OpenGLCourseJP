#version 330

// Attribute変数
layout(location = 0) in vec3 in_position;

// Varying変数
out vec4 f_positionLightSpace;

// 各種変換行列
uniform mat4 u_mvpMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // ライトから見たときの座標
    f_positionLightSpace = gl_Position;
}
