#version 430 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float costheta = sqrt( (1.0 - Xi.y) / ((a * a - 1.0) * Xi.y + 1.0));
    float sintheta = sqrt(1.0 - costheta * costheta);

    float phi = 2.0 * PI * Xi.x;

    float x = sintheta * sin(phi);
    float y = sintheta * cos(phi);
    float z = costheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 res = x * tangent + y * bitangent + z * N;
    res = normalize(res);
    return res;
}

void main()
{
     vec3 N = normalize(WorldPos);

     vec3 V = N;
     vec3 R = N;

     uint samplerCount = 1024;
     vec3 ld = vec3(0.0);
     float w = 0.0;
     for(uint i = 0; i < samplerCount; i++)
     {
        vec2 Xi = Hammersley(i, samplerCount);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);

        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float nol = max(dot(N, L), 0.0);
        if(nol > 0.0)
        {
            float D = DistributionGGX(N, H, roughness);
            float noh = max(dot(N, H), 0.0);
            float voh = max(dot(V, H) ,0.0);
            float pdf = D * noh / (4 * voh) + 0.0001;

            float lod = roughness == 0 ? 0 : 0.5 * log2(512 * 512 / (samplerCount * pdf));

            ld += textureLod(environmentMap, L, lod).rgb * voh;
            w += voh;
        }
     }

     FragColor = vec4(ld / w, 1.0);
}