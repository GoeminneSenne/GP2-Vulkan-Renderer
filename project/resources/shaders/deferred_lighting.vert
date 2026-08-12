#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec2 fragTexCoord;

const vec2 vertices[3] = vec2[](
	vec2(-1.0,-1.0),
	vec2(3.0,-1.0),
	vec2(-1.0,3.0)
);

const vec2 texCoords[3] = vec2[](
	vec2(0.0,0.0),
	vec2(2.0, 0.0),
	vec2(0.0, 2.0)
);

void main()
{
	gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
	fragTexCoord = texCoords[gl_VertexIndex];
}