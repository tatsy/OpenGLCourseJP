#version 330

// Varying変数
in vec2 f_texcoord;

// ディスプレイへの出力変数
out vec4 out_color;

// テクスチャ
uniform sampler2D u_texture;
uniform bool u_useTexture;

// 針を使う場合の色
uniform vec3 u_needleColor;

void main() {
    // 描画色を代入
    if (u_useTexture) {
        vec2 texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
        out_color = vec4(texture(u_texture, texcoord).rgb, 1.0);
    } else {
        out_color = vec4(u_needleColor, 1.0);
    }
}
