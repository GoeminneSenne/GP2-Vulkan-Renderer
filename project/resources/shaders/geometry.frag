#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(early_fragment_tests) in;

layout(constant_id = 0) const uint TEXTURE_ARRAY_SIZE = 69;
layout(set = 0, binding = 0) uniform sampler2D texSamplers[TEXTURE_ARRAY_SIZE];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outMetallicRoughness;

layout(push_constant) uniform constants
{
	mat4 model;
	uint albedoTexIndex;
	uint normalTexIndex;
	uint metallicRoughnessTexIndex;
} pushConstants;


void main() 
{
	vec4 baseColor = texture(texSamplers[pushConstants.albedoTexIndex], fragTexCoord);

	//Alpha Clipping
	const float alphaThreshold = 0.95f;
	if(baseColor.a < alphaThreshold)
		discard;

	outColor = baseColor;

	//Create Tangent Space Axis
	vec3 binormal = cross(fragNormal, fragTangent);
	binormal = normalize(binormal);
	mat3 tangentSpaceAxis = mat3(fragTangent, binormal, fragNormal);

	//Sample normal map
	vec3 sampledNormal = texture(texSamplers[pushConstants.normalTexIndex], fragTexCoord).rgb;
	
	//Remap sampled normal to correct range
	sampledNormal = sampledNormal * 2.0 - 1.0;

	//Transform sampled normal with tangentSpaceAxis
	sampledNormal = tangentSpaceAxis * sampledNormal;
	sampledNormal = normalize(sampledNormal);

	outNormal = sampledNormal;
	
	//Metallic and Roughness
	vec4 metalRoughTex = texture(texSamplers[pushConstants.metallicRoughnessTexIndex], fragTexCoord);

	float roughness = metalRoughTex.g;
	float metallic = metalRoughTex.b;

	outMetallicRoughness.g = roughness;
	outMetallicRoughness.b = metallic;
}