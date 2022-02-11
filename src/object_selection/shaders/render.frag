#version 330

// Varying変数: 頂点シェーダから受け取った値
// Varying variables: received from a vertex shader.
in vec3 f_fragColor;

// Varying変数: ディスプレイへの出力変数
// Varying variables: here for exporting colors to the display 
out vec4 out_color;

// 選択を判定するためのID
// Index for indentifying cubes
uniform int u_selectID;

void main() {
    if (u_selectID > 0) {
        // 選択のIDが0より大きい => 選択モード, IDで描画する
        // ID is more than 0 => selection mode, use ID
        float c = u_selectID / 255.0;
        out_color = vec4(c, c, c, 1.0);
    } else {
        // IDが0より小さい => 描画モード, 通常の色を使う
        // ID is less than 0 => paint mode, use face color
        out_color = vec4(f_fragColor, 1.0);
    }
}
