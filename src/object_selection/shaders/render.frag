#version 330

// Varying変数
in vec3 f_fragColor;

// ディスプレイへの出力変数
out vec4 out_color;

// 選択を判定するためのID
uniform int u_selectID;

void main() {
    if (u_selectID > 0) {
        // 選択のIDが0より大きければIDで描画する
        float c = u_selectID / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {
        // 描画色を代入
        out_color = vec4(f_fragColor, 1.0);
    }
}
