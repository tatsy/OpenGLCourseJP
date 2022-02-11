#version 330

// Varying変数: 頂点シェーダから受け取った値
// Varying variables: received from a vertex shader.
in vec3 f_worldPos;
in vec3 f_fragColor;

// Varying変数: ディスプレイへの出力変数
// Varying variables: here for exporting colors to the display
out vec4 out_color;

void main() {
    // 描画色を代入 / Store pixel colors
    out_color = vec4(f_fragColor, 1.0);
}
