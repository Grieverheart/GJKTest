#version 330 core

uniform sampler2D DepthMap;
uniform sampler2D NormalMap;
uniform sampler2D noise;

uniform vec2 projAB;
uniform ivec2 noiseScale;
uniform int kernelSize;
uniform vec3 kernel[256];
uniform float RADIUS;
uniform mat4 projectionMatrix;

noperspective in vec2 pass_TexCoord;
smooth in vec3 viewRay;

layout(location = 0) out float out_AO;

vec3 CalcPosition(void){
	float depth = texture(DepthMap, pass_TexCoord).r;
	float linearDepth = projAB.y / (depth - projAB.x);
	vec3 ray = viewRay / viewRay.z;
	return linearDepth * ray;
}

mat3 CalcRMatrix(vec3 normal, vec2 texcoord){
	vec3 rvec = texture(noise, texcoord * noiseScale).xyz;
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	
	return mat3(tangent, bitangent, normal);
}


void main(void){

	vec2 TexCoord = pass_TexCoord;
	vec3 Normal = texture(NormalMap, TexCoord).xyz;
	if(Normal != 0.0){
		vec3 Position = CalcPosition();
		Normal = normalize(Normal);
		mat3 RotationMatrix = CalcRMatrix(Normal, TexCoord);
		
		float occlusion = 0.0;
		
		for(int i = 0; i < kernelSize; i++){
			// Get sample position
			vec3 sample = RotationMatrix * kernel[i];
			sample = sample * RADIUS + Position;
			// Project and bias sample position to get its texture coordinates
			vec4 offset = projectionMatrix * vec4(sample, 1.0);
			offset.xy /= offset.w;
			offset.xy = offset.xy * 0.5 + 0.5;
			// Get sample depth
			float sample_depth = texture(DepthMap, offset.xy).r;
			sample_depth = projAB.y / (sample_depth - projAB.x);
			if(abs(Position.z - sample_depth) < RADIUS){
				occlusion += (sample_depth > sample.z) ? 1.0 : 0.0;
			}
		}
		out_AO = 1.0 - occlusion / kernelSize;
	}
	else discard;
}