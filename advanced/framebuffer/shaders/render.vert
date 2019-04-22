#version 330

// Attribute変数
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

// Varying変数
out vec2 f_texcoord;

// Uniform変数
uniform mat4 u_mvpMat;

void main() {
    // gl_Positionは頂点シェーダの組み込み変数
    // 指定を忘れるとエラーになるので注意
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying変数への代入
    f_texcoord = in_texcoord;
}
