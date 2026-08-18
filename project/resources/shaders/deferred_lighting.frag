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

vec3 gLightDirection = -vec3(0.577, -0.577, -0.577);
float gLightIntensity = 2.0f;
vec3 gLightColor = vec3(1, 1, 1);

const float PI = 3.14159265359;

vec3 GetWorldPositionFromDepth(in float depth, in ivec2 fragCoords, in ivec2 resolution,
	in mat4 invProj, in mat4 invView);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{
	const ivec2 texelCoord = ivec2(texCoord * vec2(textureSize(gDepth, 0)));
	const float depth = texelFetch(gDepth, texelCoord, 0).r;

	const vec3 worldPosition = GetWorldPositionFromDepth(depth,
		texelCoord, camData.resolution,
		inverse(camData.proj), inverse(camData.view));

	const vec3 viewVector = normalize(camData.camPos.xyz - worldPosition);


	//Light data
	const float illuminance = gLightIntensity; //Should be lux
	const vec3 irradiance = gLightColor * illuminance;

	//OpenGL tutorial code
	vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	float metallic = texture(gMetallicRougness, texCoord).b;
	float roughness = texture(gMetallicRougness, texCoord).g;

	vec3 Lo = vec3(0.0);

	vec3 N = normal;
	vec3 L = gLightDirection;
	vec3 V = viewVector;
	vec3 H = normalize(V + L);

	vec3 radiance = irradiance;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N,L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	float NdotL = max(dot(N,L), 0.0);
	Lo += (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = vec3(0.03) * albedo;
	vec3 color = ambient + Lo;

	//color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0/2.2));

	outColor = vec4(color, 1.0);
}

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

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness;
	float a2 = a * a;
	float NdotH = max(dot(N,H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}