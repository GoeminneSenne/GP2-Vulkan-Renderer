#version 450

layout(constant_id = 0) const uint TEXTURE_ARRAY_SIZE = 25;
layout(set = 0, binding = 0) uniform sampler2D texSamplers[TEXTURE_ARRAY_SIZE];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants
{
	mat4 model;
	uint textureIndex;
} pushConstants;


void main() 
{
    outColor = texture(texSamplers[pushConstants.textureIndex], fragTexCoord);

	const float alphaThreshold = 0.95f;
	if(outColor.a < alphaThreshold)
		discard;
}