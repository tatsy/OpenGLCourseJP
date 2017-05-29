#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;
in vec4 f_positionLightSpace;

out vec4 out_color;

// マテリアルのデータ
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_shininess;

// シャドウ・マップのための深度テクスチャ
uniform sampler2D u_depthTex;
uniform bool u_enableSM;

void main() {
	// 影の計算
	float visibility = 1.0;
	if (u_enableSM) {
		if (f_positionLightSpace.w >= 0.0) {
			vec2 texcoord = (f_positionLightSpace.xy / f_positionLightSpace.w) * 0.5 + 0.5;
			float zValue = f_positionLightSpace.z / f_positionLightSpace.w;
			float dValue = texture(u_depthTex, texcoord).r;
			if (dValue - 0.000005 < zValue) {
				visibility = 0.5;
			}
		}
	}

	// カメラ座標系を元にした局所座標系への変換
	vec3 V = normalize(-f_positionCameraSpace);
	vec3 N = normalize(f_normalCameraSpace);
	vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
	vec3 H = normalize(V + L);

	// Blinn-Phongの反射モデル
	float ndotl = max(0.0, dot(N, L));
	float ndoth = max(0.0, dot(N, H));
	vec3 diffuse = u_diffColor * ndotl;
	vec3 specular = u_specColor * pow(ndoth, u_shininess);
	vec3 ambient = u_ambiColor;

    out_color = vec4(visibility * (diffuse + specular + ambient), 1.0);
}
