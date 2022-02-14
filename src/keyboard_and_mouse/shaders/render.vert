#version 330

// Attribute変数: 頂点の持つ属性情報(位置や色など)を表す
// Attribute variables: specifies vertex attributes, e.g., positions and colors.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

// Varying変数: シェーダ間の変数の受け渡しに用いる
// Varying variables: are used for sharing variables between shader stages.
out vec3 f_fragColor;

// Uniform変数: 全てのシェーダで同じ値を持つパラメータ半数
// Uniform variables: are parameters shared among all the shaders
uniform mat4 u_mvpMat;

void main() {
    // gl_Positionは頂点シェーダの組み込み変数．指定を忘れるとエラーになるので注意
    // You must specify "gl_Position", which is a build-in variable for vertex shader
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // Varying変数への代入
    // Store color data to varying variable
    f_fragColor = in_color;
}
