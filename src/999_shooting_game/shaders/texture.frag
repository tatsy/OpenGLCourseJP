#version 410

layout(location = 3) in vec2 f_texcoord;

layout(location = 0) out vec4 out_color;

uniform bool u_isTextured;
uniform sampler2D u_texture;

void main(void) {
    if (u_isTextured) {
        out_color = texture(u_texture, f_texcoord);
    }
}
