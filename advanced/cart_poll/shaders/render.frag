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
uniform vec3 u_diffColor = vec3(1.0, 0.0, 0.0);
uniform vec3 u_specColor = vec3(0.1, 0.1, 0.1);
uniform float u_shininess = 8.0;

void main() {
    // カメラ座標系を元にした局所座標系への変換
    // Local coordinate system in camera-space coordinates
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);

    // Blinn-Phongの反射モデル
    // Blinn-Phong shading
    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));
    vec3 diffuse = u_diffColor * ndotl;
    vec3 specular = u_specColor * pow(ndoth, u_shininess);
    vec3 ambient = u_diffColor * 0.1;

    // 描画色を代入 / Store pixel colors
    out_color = vec4(diffuse + specular + ambient, 1.0);
}
