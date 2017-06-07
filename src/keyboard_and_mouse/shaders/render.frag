#version 330

// Varying変数
in vec3 f_fragColor;

// ディスプレイへの出力変数
out vec4 out_color;

void main() {
    // 描画色を代入
    out_color = vec4(f_fragColor, 1.0);
}
