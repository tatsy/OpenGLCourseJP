#version 330

in vec3 f_fragColor;

out vec4 out_color;

void main() {
    out_color = vec4(f_fragColor, 1.0);
}
