#version 410

layout(location = 0) in vec3 f_posViewSpace;
layout(location = 1) in vec3 f_normViewSpace;
layout(location = 2) in vec3 f_lightPosViewSpace;
layout(location = 3) in vec2 f_texcoord;

layout(location = 0) out vec4 out_color;

uniform vec3 u_ambiColor;
uniform vec3 u_diffColor;
uniform vec3 u_specColor;
uniform float u_shininess;

uniform bool u_isTextured;
uniform sampler2D u_texture;

void main(void) {
    vec3 V = normalize(-f_posViewSpace);
    vec3 N = normalize(f_normViewSpace);
    vec3 L = normalize(f_lightPosViewSpace - f_posViewSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffuse = u_diffColor;
    if (u_isTextured) {
        diffuse *= texture(u_texture, f_texcoord).rgb;
    }

    vec3 specular = u_specColor;
    vec3 ambient = u_ambiColor;

    out_color.rgb = ambient + diffuse * ndotl + specular * pow(ndoth, u_shininess);
    out_color.a = 1.0;
}
