#version 330

// Attribute変数: 頂点の持つ属性情報(位置や色など)を表す
// Attribute variables: specifies vertex attributes, e.g., positions and colors.
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

// Varying変数: シェーダ間の変数の受け渡しに用いる
// Varying variables: are used for sharing variables between shader stages.
out vec3 f_fragColor;

// Uniform変数: 全てのシェーダで同じ値を持つパラメータ半数
// Uniform variables: are parameters shared among all the shaders
uniform mat4 u_mvpMat;

void main() {
    // gl_Positionは必ず指定する
    // You MUST specify gl_Position
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // 今回は描画色を法線情報から決定
    // Here, pixel colors are determined by normal vectors
    f_fragColor = in_normal * 0.5 + 0.5;
}
