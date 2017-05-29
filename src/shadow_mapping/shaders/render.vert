#version 330

// Attribute変数
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

// Varuing変数
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;
out vec4 f_positionLightSpace;

// 光源の情報
uniform vec3 u_lightPos;

// 各種変換行列
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;
uniform mat4 u_lightBiasMVP;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // カメラ座標系への変換
	f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
	f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
	f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
    f_positionLightSpace = u_lightBiasMVP * vec4(in_position, 1.0);
}
