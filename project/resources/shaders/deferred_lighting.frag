#version 450

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gDepth;
layout(binding = 2) uniform sampler2D gNormal;

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

void main()
{
vec3 normal = normalize(texture(gNormal, texCoord).rgb);

vec3 direction = normalize(vec3(0, 0, 1.0));

float value = dot(normal, direction);
value = value * 0.5 + 0.5;

outColor = vec4(value, value, value, 1.0);
}