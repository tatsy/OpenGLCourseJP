#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;

out vec4 out_color;

uniform sampler2D u_texture;

void main() {
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);

    float ndotl = max(0.0, dot(N, L));

    out_color = vec4(texture(u_texture, vec2(ndotl, 0.5)).rgb, 1.0);
}
