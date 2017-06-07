#version 330

// Attribute変数
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Varying変数
out vec3 f_fragColor;

// Uniform変数
uniform mat4 u_mvpMat;

void main() {
    // gl_Positionは頂点シェーダの組み込み変数
    // 指定を忘れるとエラーになるので注意
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying変数への代入
    f_fragColor = in_color;
}
