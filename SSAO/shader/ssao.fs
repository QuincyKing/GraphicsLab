#version 430 core
out float FragColor;

in vec2 Tex;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

uniform mat4 projection;

const vec2 noiseScale = vec2(1000.0/4.0, 800.0/4.0);
const float radius = 0.5;
float bias = 0.025;

void main()
{
    vec3 fragPos = texture(gPosition, Tex).xyz;
    vec3 normal = normalize(texture(gNormal, Tex).rgb);
    vec3 randomVec = normalize(texture(texNoise, Tex * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

     float occlusion = 0.0;
     for(int i = 0; i < 64; i++)
     {
        vec3 sample1 = TBN * samples[i];
        sample1 = fragPos + sample1 * radius; 

        vec4 offset = vec4(sample1, 1.0);
        offset = projection * offset;
        offset.xyz /=  offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float depth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - depth + 0.00000001));
        occlusion += (depth >= sample1.z ? 1.0 : 0.0);
     }

     occlusion = 1.0 - occlusion / 64;

     FragColor = occlusion;
}