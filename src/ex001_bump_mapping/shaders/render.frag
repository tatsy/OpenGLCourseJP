#version 330

in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_tangentCameraSpace;
in vec3 f_binormalCameraSpace;
in vec3 f_lightPosCameraSpace;
in vec2 f_texcoord;

out vec4 out_color;

uniform sampler2D u_diffTexture;
uniform sampler2D u_bumpTexture;
uniform int u_enableBump;

void main() {
    vec2 texcoord = vec2(f_texcoord.x, 1.0 - f_texcoord.y);
    vec3 rhoDiff = texture(u_diffTexture, texcoord).rgb;

    vec2 size = vec2(1.0 / 4096.0, 0.0);
    ivec3 off = ivec3(-1, 0, 1);
    float height = texture(u_bumpTexture, texcoord).x;

    vec3 N = normalize(f_normalCameraSpace);
    if (u_enableBump != 0) {
        vec3 localNormal = normalize(texture(u_bumpTexture, texcoord).xyz * 2.0 - 1.0);
        vec3 bumpNormal = localNormal.z * normalize(f_normalCameraSpace) +
                          localNormal.x * normalize(f_tangentCameraSpace) +
                          localNormal.y * normalize(f_binormalCameraSpace);
        N = normalize(bumpNormal);
    }

    vec3 V = normalize(-f_positionCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffColor = ndotl * rhoDiff;
    vec3 specColor = ndoth * vec3(0.1, 0.1, 0.1);

    out_color = vec4(diffColor + specColor, 1.0);
}
