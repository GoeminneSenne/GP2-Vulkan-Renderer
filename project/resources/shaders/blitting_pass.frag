#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(set = 0, binding = 0) uniform sampler2D hdrImage;

layout(push_constant) uniform PhysicalCamera
{
	float aperture;
	float ISO;
	float shutterSpeed;
} physicalCamera;

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 outColor;

float CalculateEV100FromPhysicalCamera(in float aperture, in float shutterTime, in float ISO);
float ConvertEV100ToExposure(in float EV100);
vec3 ReinhardToneMapping(in vec3 color);

//#define SUNNY_16;
//#define INDOOR

void main()
{
	float aperture = physicalCamera.aperture;
	float ISO = physicalCamera.ISO;
	float shutterSpeed = physicalCamera.shutterSpeed;

	#ifdef SUNNY_16
		aperture = 5.f;
		ISO = 100.f;
		shutterSpeed = 1.f / 200.f;
	#endif

	#ifdef INDOOR
		aperture = 1.4f;
		ISO = 1600.f;
		shutterSpeed = 1.f / 60.f;
	#endif

	vec3 hdrColor = texture(hdrImage, uv).rgb;

	const float EV100_HardCoded = 1.f;
	const float EV100_PhysicalCamera = 
		CalculateEV100FromPhysicalCamera(aperture, shutterSpeed, ISO);

	float exposure = ConvertEV100ToExposure(EV100_PhysicalCamera);
	vec3 ldrColor = ReinhardToneMapping(hdrColor * exposure);
	
	outColor = vec4(ldrColor, 1.0f);
}

float CalculateEV100FromPhysicalCamera(in float aperture, in float shutterTime, in float ISO)
{
	return log2(pow(aperture, 2) / shutterTime * 100 / ISO);
}

float ConvertEV100ToExposure(in float EV100)
{
	const float maxLuminance = 1.2f * pow(2.f, EV100);
	return 1.f / max(maxLuminance, 0.0001f);
}

vec3 ReinhardToneMapping(in vec3 color)
{
	return color / (color + vec3(1.f));
}
