#version 430 core

in vec4 clipSpace;
in vec2 textureCoords;
in vec3 toCamera;
in vec3 fromLight;

out vec4 out_Color;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float moveFactor;
uniform vec3 lightColor;

const float waveStrength = 0.05;
const float shinedamper = 5.0;
const float reflectivity = 0.6;
const float tiling = 2.0;

void main() 
{
	vec2 tex = textureCoords * tiling;

	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractionTexcoords = vec2(ndc.x, ndc.y);
	vec2 reflectionTexcoords = vec2(ndc.x, -ndc.y);

	float near = 0.1;
	float far = 1000.0;
	float depth = texture(depthMap, refractionTexcoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	float waterDepth = floorDistance - waterDistance;

	vec2 distort = texture(dudvMap, vec2(tex.x + moveFactor, tex.y)).rg * 0.1;
	distort = tex + vec2(distort.x, distort.y + moveFactor);
	vec2 totaldistort = (texture(dudvMap, distort).rg * 2.0 - 1.0 ) * waveStrength * clamp(waterDepth/20.0, 0.0, 1.0);

	refractionTexcoords += totaldistort;
	refractionTexcoords = clamp(refractionTexcoords, 0.001, 0.999);
	reflectionTexcoords += totaldistort;
	reflectionTexcoords.x = clamp(reflectionTexcoords.x, 0.001, 0.999);
	reflectionTexcoords.y = clamp(reflectionTexcoords.y, -0.999, -0.001);

	vec4 reflection = texture(reflectionTexture, reflectionTexcoords);
	vec4 refraction = texture(refractionTexture, refractionTexcoords);

	vec4 normalColor = texture(normalMap, distort);
	vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 3.0, normalColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 view = normalize(toCamera);
	float fresnel = dot(view, normal);
	fresnel = pow(fresnel, 5.0);

	vec3 reflectedLight = reflect(normalize(fromLight), normal);
	float specular = max(dot(reflectedLight, view), 0.0);
	specular = pow(specular, shinedamper);
	float distance = length(fromLight);
	float atten = 5 * 1.0 / distance;
	vec3 specularHightlight = atten * lightColor * specular * reflectivity ;

	out_Color = mix(reflection, refraction, fresnel);
	out_Color = mix(out_Color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHightlight, 0.0);
}