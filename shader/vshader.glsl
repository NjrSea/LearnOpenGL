#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec2 textureCoord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec2 tc; // 纹理坐标输出到光栅着色器用于插值

void main(void)
{
	gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
	tc = textureCoord;
} 
