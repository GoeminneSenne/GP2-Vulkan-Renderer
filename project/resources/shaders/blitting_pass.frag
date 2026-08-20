#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform sampler2D hdrImage;

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

void main()
{
	vec3 hdrColor = texture(hdrImage, uv).rgb;

	vec3 ldrColor = hdrColor / (hdrColor + vec3(1.0));
	outColor = vec4(ldrColor, 1.0f);
}

