#version 430

out vec4 color;

// in vec4 varyingColor;
in vec2 tc; // 纹理坐标(插值过的材质坐标)

layout (binding=0) uniform sampler2D samp;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void) {
    color = texture(samp, tc);
}