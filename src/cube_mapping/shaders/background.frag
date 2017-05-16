#version 330

in vec3 f_positionOnCube;

out vec4 out_color;

uniform samplerCube u_texture;

void main(void) {
    vec3 V = normalize(f_positionOnCube);
    out_color = texture(u_texture, V);
}
