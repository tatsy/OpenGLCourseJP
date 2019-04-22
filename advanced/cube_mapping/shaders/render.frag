#version 330

in vec3 f_positionWorldSpace;
in vec3 f_cameraPosWorldSpace;
in vec3 f_normalWorldSpace;

out vec4 out_color;

uniform samplerCube u_texture;

void main() {
    vec3 V = normalize(f_cameraPosWorldSpace - f_positionWorldSpace);
    vec3 N = normalize(f_normalWorldSpace);
    vec3 R = -V + 2.0 * N * dot(V, N);
    out_color = texture(u_texture, R);
}
