#version 330 core

in vec4 FragPos;
in vec2 TexCoords;
in vec4 Normal;

uniform vec4 EyePosW;   // Eye position in world space.
uniform vec4 LightPosW; // Light's position in world space.
uniform vec4 LightColor; // Light's diffuse and specular contribution.

uniform vec4 MaterialEmissive;
uniform vec4 MaterialDiffuse;
uniform float MaterialShininess;

uniform vec4 Ambient; // Global ambient contribution.

uniform sampler2D diffuseSampler;
uniform sampler2D normalSampler;
uniform sampler2D nightSampler;
uniform sampler2D specularSampler;

layout (location=0) out vec4 out_color;

void main()
{
    // Compute the emissive term.
    vec4 Emissive = MaterialEmissive;

    // Compute the diffuse term.

    vec4 N = normalize( texture(normalSampler, TexCoords) );
    vec4 L = normalize( LightPosW - FragPos );

    float NdotL = dot( N, L );

    vec4 Diffuse =  LightColor * MaterialDiffuse;
    
    // Compute the specular term.
    vec4 V = normalize( EyePosW - FragPos );
    vec4 H = normalize( L + V );
    vec4 R = reflect( -L, N );
    float RdotV = max( dot( R, V ), 0 );
    float NdotH = max( dot( N, H ), 0 );
    vec4 Specular = pow( RdotV, MaterialShininess ) * LightColor * texture(specularSampler, TexCoords);

    vec3 t0 = texture(diffuseSampler,TexCoords).rgb; // day
    vec3 t1 = texture(nightSampler,TexCoords).rgb; // night
 
    float y = smoothstep(-0.1, 0.1, NdotL);
    vec3 t = t0*y + t1*(1-y);

    out_color = ( Emissive + Ambient + Diffuse + Specular ) * vec4(t,1.0);
}

















