#version 330

// Attribute変数 / Attribute variables
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

// Varuing変数 / Varying variables
out vec3 f_positionCameraSpace;
out vec3 f_normalCameraSpace;
out vec3 f_lightPosCameraSpace;

// 光源の情報 / Light parameters
uniform vec3 u_lightPos;

// 各種変換行列 / Transformation matrices
uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;
uniform mat4 u_lightMat;

void main() {
    // gl_Positionは必ず指定する
    // You MUST specify gl_Position
    gl_Position = u_mvpMat * vec4(in_position, 1.0);

    // カメラ座標系への変換
    // Transform world-space parameters to those in the camera-space
    f_positionCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (u_lightMat * vec4(u_lightPos, 1.0)).xyz;
}
