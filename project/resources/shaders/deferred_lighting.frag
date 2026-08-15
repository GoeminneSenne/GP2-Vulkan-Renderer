#version 450

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gDepth;
layout(binding = 2) uniform sampler2D gNormal;

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec3 normal = (texture(gNormal, texCoord).rgb);
	vec3 normalColor = normal * 0.5 + 0.5;
	
	//outColor = vec4(normalColor, 1.0);
	outColor = vec4(pow(normalColor, vec3(2.2)), 1.0);
}