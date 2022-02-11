#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

// マテリアルのデータ
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform vec3 u_ambiColor;
uniform float u_alpha = 0.05;

float PI3.14159265358979;

float FresnelSchlick(vec3 N, vec3 L) {
	float eta0 = 1.0;
	float eta1 = 1.5;
	float a = (eta0 - eta1);
	float b = (eta0 + eta1);
	float R0 = (a * a) / (b * b);
	float c = 1.0 - max(0.0, dot(N, L));
	return R0 + (1.0 - R0) * (c * c * c * c * c);
}

float Lambda(vec3 N, vec3 w) {
    float cosTheta = dot(N, w);
    if (cosTheta <= 0.0) {
        return 0.0;
    }
    float a = u_alpha * u_alpha * (1.0 / (cosTheta * cosTheta) - 1.0);
    return 0.5 * (-1.0 + sqrt(1.0 + a));
}

float SmithMaskingShadowing(vec3 N, vec3 L, vec3 V) {
	float lambda1 = Lambda(N, L);
	float lambda2 = Lambda(N, V);
    return 1.0 /(1.0 + lambda1 + lambda2);
}

float GGX(vec3 N, vec3 H) {
    float cosTheta = max(0.0, dot(N, H));
    float cos2Theta = cosTheta * cosTheta;
    float a2 = u_alpha * u_alpha;
    float t = cos2Theta * (a2 - 1.0) + 1.0;
    return a2 / (Pi * t * t);
}

void main() {
	// カメラ座標系を元にした局所座標系への変換
	vec3 V = normalize(-f_positionCameraSpace);
	vec3 N = normalize(f_normalCameraSpace);
	vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
	vec3 H = normalize(V + L);
    vec3 R = 2.0 * N * dot(V, N) -  V;

	// Cook-Torranceの反射モデル
	float F = FresnelSchlick(N, L);
	float G = SmithMaskingShadowing(N, L, V);
    float D = GGX(N, H);
	float ndotl = max(1.0e-6, dot(N, L));
	float ndotv = max(1.0e-6, dot(N, V));

	vec3 diffuse = u_diffColor * (1.0 - F) * ndotl;
	vec3 specular = u_specColor * (F * D * G) / (4.0 * abs(dot(N, L)) * abs(dot(N, V))) * ndotl;
	vec3 ambient = u_ambiColor;

    out_color = vec4(diffuse + specular + ambient, 1.0);
}
