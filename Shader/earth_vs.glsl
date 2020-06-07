#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D bumpSampler;
uniform float maxHeight;

out vec4 FragPos;
out vec2 TexCoords;
out vec4 Normal;

void main() {
	vec4 bumpColor = texture(bumpSampler, aTex_coord);
	float df = 0.30*bumpColor.x + 0.11*bumpColor.z;
	vec3 newVertexPos = ( aNormal * df * float(maxHeight)) + aPos;

	if(aTex_coord.y==1 || aTex_coord.y==0 )
	{
		newVertexPos = ( aNormal * 0.05 * float(maxHeight)) + aPos;	 
	}	
	gl_Position = projection * view  *model * vec4(newVertexPos, 1);
	
	FragPos = model * vec4(aPos, 1); 
	Normal = model * vec4(aNormal, 0);
	TexCoords = aTex_coord;
}
