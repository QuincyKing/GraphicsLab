#version 430 core
in vec3 WorldPos;
in vec2 Tex;
in vec3 Normal;
in vec4 lightSpacePos;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D shadowMap;

//uniform vec3 albedo;
//uniform float metallic;
//uniform float roughness;
//uniform float ao;

uniform vec3 camPos;

out vec4 FragColor;

const float PI = 3.14159265359f;

float ShadowCalculation(vec4 fragPosLightSpace)
{
   // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 49;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, Tex).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(Tex);
    vec2 st2 = dFdy(Tex);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float NDFGGX(vec3 N, vec3 H, float roughness)
{
    roughness *= roughness;
    float elem1 = roughness * roughness;
    float elem2 = max(dot(N, H), 0.0) * max(dot(N, H), 0.0) * (elem1 - 1) + 1;
    float elem22 = elem2 * elem2;

    return elem1 / (PI * max(elem22, 0.001));
}

float GeometrySchlickGGX(vec3 N, vec3 V, float roughness)
{
    float k = (roughness + 1) * (roughness + 1) / 8.0f;
    float nDotV = max(dot(N, V), 0.0f);
    return nDotV / (nDotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float occlusion = GeometrySchlickGGX(N, V, roughness);
    float shadow = GeometrySchlickGGX(N, L, roughness);

    return occlusion * shadow;
}

vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0)
{
    float cosTheta = clamp(dot(H, V), 0.0f, 1.0f);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
   vec3 albedo = pow(texture(albedoMap, Tex).rgb, vec3(2.2));
   float metallic = texture(metallicMap, Tex).r;
   float roughness = texture(roughnessMap, Tex).r;
   float ao = texture(aoMap, Tex).r;

   vec3 N = GetNormalFromMap();
   vec3 V = normalize(camPos - WorldPos);
   vec3 L = normalize(lightPos - WorldPos);
   vec3 H = normalize(V + L);

   vec3 R0 = vec3(0.04f);
   R0 = mix(R0, albedo, metallic);

   vec3 L0 = vec3(0.0f);

   float distance = length(lightPos - WorldPos);
   float atten = 1.0 / (distance * distance);
   vec3 radiance = lightColor * atten;

   float D = NDFGGX(N, H, roughness);
   float G = GeometrySmith(N, V, L, roughness);
   vec3 F = FresnelSchlick(H, V, R0);

   vec3 cooktorrence = D * G * F / max(4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f), 0.0001f );

   vec3 ks = F;
   vec3 kd = vec3(1.0) - ks;
   kd *= 1.0f - metallic;

   vec3 brdf = kd * albedo / PI + cooktorrence;

   L0 += brdf * radiance * max(dot(N, L), 0.0f);

    vec3 ambient = vec3(0.03) * albedo * 1.0;

    float shadow = ShadowCalculation(lightSpacePos);
    vec3 color = ambient + (1.0 - shadow) * L0;

   // HDR tonemapping
   color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0f);
}