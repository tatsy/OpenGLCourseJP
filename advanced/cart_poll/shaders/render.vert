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
uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

void main() {
    mat4 mvMat = u_viewMat * u_modelMat;
    mat4 mvpMat = u_projMat * mvMat;
    mat4 normMat = transpose(inverse(mvMat));
    mat4 lightMat = u_viewMat;

    // gl_Positionは必ず指定する
    // You MUST specify gl_Position
    gl_Position = mvpMat * vec4(in_position, 1.0);

    // カメラ座標系への変換
    // Transform world-space parameters to those in the camera-space
    f_positionCameraSpace = (mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (normMat * vec4(in_normal, 0.0)).xyz;
    f_lightPosCameraSpace = (lightMat * vec4(u_lightPos, 1.0)).xyz;
}
