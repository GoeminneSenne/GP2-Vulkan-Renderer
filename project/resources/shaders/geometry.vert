#version 450

layout(set = 1, binding = 0) uniform ViewProj
{
	mat4 view;
	mat4 proj;
} viewProj;

layout(push_constant) uniform constants
{
	mat4 model;
	uint albedoTexIndex;
	uint normalTexIndex;
	uint metallicRoughnessTexIndex;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragTangent;

void main()
{
	gl_Position = viewProj.proj * viewProj.view * pushConstants.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragTexCoord = inTexCoord;
	fragNormal = normalize(mat3(pushConstants.model) * inNormal);
	fragTangent = normalize(mat3(pushConstants.model) * inTangent);
}