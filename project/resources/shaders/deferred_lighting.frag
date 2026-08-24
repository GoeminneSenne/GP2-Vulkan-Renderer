#version 450

struct PointLight
{
	vec3 position;
	float lumen;
	vec3 color;
	float _padding;
};

struct DirectionalLight
{
	vec3 direction;
	float lux;
	vec3 color;
	float _padding;
};

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

layout(set = 2, binding = 0) readonly buffer PointLights
{
	PointLight[] lights;
} pointLights;
layout(set = 2, binding = 1) readonly buffer DirectionalLights
{
	DirectionalLight[] lights;
} dirLights;

layout(push_constant) uniform PC
{
	uint pointLightCount;
	uint dirLightCount;
} pc;


layout(location = 0) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

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

	vec3 albedo = texture(gAlbedo, texCoord).rgb;
	float metallic = texture(gMetallicRougness, texCoord).b;
	float roughness = texture(gMetallicRougness, texCoord).g;
	
	vec3 N = normalize(texture(gNormal, texCoord).rgb);
	vec3 V = normalize(camData.camPos.xyz - worldPosition);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);

	//Point Light loop
	for(int i = 0; i < pc.pointLightCount; ++i)
	{
		PointLight light = pointLights.lights[i];

		const float lumen = light.lumen;
		const float luminousIntensity = lumen / (4.0 * PI);
		const float DistanceToLight = length(light.position - worldPosition);

		const float attentuation = 1.0 / max((DistanceToLight * DistanceToLight), 0.0001f);
		const float illuminance = luminousIntensity * attentuation;
		const vec3 irradiance = light.color * illuminance;
	
		//vec3 L = gLightDirection;
		vec3 L = normalize(light.position - worldPosition);
		vec3 H = normalize(V + L);
	
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);
	
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N,L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float NdotL = max(dot(N,L), 0.0);
		Lo += (kD * albedo / PI + specular) * irradiance * NdotL;
	}

	//Directional Light Loop
	for(int i = 0; i < pc.dirLightCount; ++i)
	{
		DirectionalLight light = dirLights.lights[i];
		
		const float illuminance = light.lux;
		const vec3 irradiance = light.color * illuminance;
	
		vec3 L = -light.direction;
		vec3 H = normalize(V + L);
	
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);
	
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N,L), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		float NdotL = max(dot(N,L), 0.0);
		Lo += (kD * albedo / PI + specular) * irradiance * NdotL;
	}


	vec3 ambient = vec3(0.025f) * albedo;
	ambient = vec3(0);
	vec3 color = ambient + Lo;

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