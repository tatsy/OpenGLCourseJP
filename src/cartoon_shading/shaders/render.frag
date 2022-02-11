#version 330

// Varying変数: 頂点シェーダから受け取った値
// Varying variables: received from a vertex shader.
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

// Varying変数: ディスプレイへの出力変数
// Varying variables: here for exporting colors to the display
out vec4 out_color;

// マテリアルのデータ
// Material parameters
uniform sampler2D u_texture;

void main() {
	// カメラ座標系を元にした局所座標系への変換
	// Local coordinate system in camera-space coordinates
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);

    // テクスチャから色を読み取る
    // Read shading color from texture
    float ndotl = max(0.0, dot(N, L));
    vec3 rgb = texture(u_texture, vec2(ndotl, 0.5)).rgb;

    // 描画色を代入 / Store pixel colors
    out_color = vec4(rgb, 1.0);
}
