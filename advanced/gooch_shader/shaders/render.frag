#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

// NPR info
const float b = 0.8;
const float y = 0.4;
const float alpha = 0.2;
const float beta = 0.7;
const vec4 k_blue = vec4(0.0, 0.0, b, 1.0);
const vec4 k_yell = vec4(y, y, 0.0, 1.0);
const vec4 k_diff = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 k_cool = k_blue + alpha * k_diff;
const vec4 k_warm = k_yell + beta * k_diff;

// Material info
const vec4 ambMaterial = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 diffMaterial = vec4(0.8, 0.2, 0.2, 1.0);
const vec4 specMaterial = vec4(0.2, 0.2, 0.2, 1.0);
const float shinMaterial = 64.0;

// uniform variables
uniform float lightStrength = 0.2;
uniform int drawHighlight = 1;

void main() {
	// カメラ座標系を元にした局所座標系への変換
	vec3 V = normalize(-f_positionCameraSpace);
	vec3 N = normalize(f_normalCameraSpace);
	vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
	vec3 H = normalize(V + L);

	// npr
	float LdotN = dot(L, N);
	vec4 coolColor = (1.0 - (1.0 + LdotN) * 0.5) * k_cool;
	vec4 warmColor = (1.0 + LdotN) * 0.5 * k_warm;

	// ambient
	vec4 ambient = ambMaterial * lightStrength;

	// diffuse
	float NdotL = dot(N, L);
	vec4 diffuse = vec4(max(0.0, NdotL)) * lightStrength * diffMaterial;

	// specular
	float NdotH = dot(N, H);
	float spec = pow(max(0.0, NdotH), shinMaterial);
	if (NdotL <= 0.0) {
		spec = 0.0;
	}
	vec4 specular = spec * specMaterial;
	vec4 shading = ambient + diffuse + specular;
	vec4 npr = coolColor + warmColor;
	out_color = shading + npr;

	if (drawHighlight == 1) {
		if (NdotH > 0.98) {
			out_color = vec4(1.0, 1.0, 1.0, 1.0);
		}
	}

	float NdotV = max(0.0, dot(N, V));
	float boundary = clamp(NdotV * NdotV * 10.0, 0.0, 1.0);
	out_color *= (0.1 + 0.9 * boundary);
}
