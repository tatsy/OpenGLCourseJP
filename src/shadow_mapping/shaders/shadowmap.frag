#version 330

in vec4 f_positionLightSpace;

out vec4 out_color;

void main(void) {
    float depth = f_positionLightSpace.z / f_positionLightSpace.w;
    out_color = vec4(depth, depth, depth, 1.0);
}
