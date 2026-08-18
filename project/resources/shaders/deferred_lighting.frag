#version 450

layout(binding = 0) uniform sampler2D gAlbedo;
layout(binding = 1) uniform sampler2D gDepth;
layout(binding = 2) uniform sampler2D gNormal;
layout(binding = 3) uniform sampler2D gMetallicRougness;

layout(set = 1, binding = 0) uniform CamData
{
	mat4 view;
	mat4 proj;
	vec4 camPos;
	ivec2 resolution;
} camData;

layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

vec3 gLightDirection = vec3(0.577, -0.577, -0.577);
float gLightIntensity = 1.0f;

vec3 GetWorldPositionFromDepth(in float depth,
	in ivec2 fragCoords, in ivec2 resolution,
	in mat4 invProj, in mat4 invView)
{
	// Convert from fragment coordinate system to NDC and
	// reconstruct clip space coordinate position
	vec2 ndc = vec2(
		(float(fragCoords.x) / resolution.x) * 2.0f - 1.0f,
		(float(fragCoords.y) / resolution.y) * 2.0f - 1.0f);
	ndc.y *= -1.f;
	const vec4 clipPos = vec4(ndc, depth, 1.0f);

	//Inverse projection to view space
	vec4 viewPos = invProj * clipPos;
	viewPos /= viewPos.w;

	//Inverse view to world space
	vec4 worldPos = invView * viewPos;
	return worldPos.xyz;
}

void main()
{
	const ivec2 texelCoord = ivec2(texCoord * vec2(textureSize(gDepth, 0)));
	const float depth = texelFetch(gDepth, texelCoord, 0).r;

	const vec3 worldPosition = GetWorldPositionFromDepth(depth,
		texelCoord, camData.resolution,
		inverse(camData.proj), inverse(camData.view));

	const vec3 viewVector = normalize(camData.camPos.xyz - worldPosition);


	vec3 normal = (texture(gNormal, texCoord).rgb);
	vec3 normalColor = normal * 0.5 + 0.5;
	
	outColor = texture(gAlbedo, texCoord);
	//outColor = vec4(pow(normalColor, vec3(2.2)), 1.0); //normal debugging


	//vec4 metalRoughness = texture(gMetallicRougness, texCoord);
	//outColor = metalRoughness;
}