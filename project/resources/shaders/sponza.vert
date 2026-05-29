#version 450

layout(binding = 0) uniform ViewProj
{
	mat4 view;
	mat4 proj;
} viewProj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
	mat4 model =    mat4(
		0.008, 0, 0, 0,
        0, 0, 0.008, 0,
        0, -0.008, 0, 0,
        0, 0, 0, 1);

	gl_Position = viewProj.proj * viewProj.view * model * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
}