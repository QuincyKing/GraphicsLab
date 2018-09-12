#version 430 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
   vec3 N = normalize(WorldPos);
   vec3 up = vec3(0.0, 1.0, 0.0);
   vec3 right = cross(up, N);
   up = cross(N, right);
   vec3 irradiance = vec3(0.0, 0.0, 0.0);

   float delta = 0.025;
   int Nr = 0;
   for(float phi = 0.0; phi <  2.0 * PI; phi += delta)
   {
        for(float theta = 0.0; theta < 0.5 * PI; theta += delta)
        {
            vec3 sampleVec = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            sampleVec = sampleVec.x * right + sampleVec.y * up + sampleVec.z * N;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);

            Nr++;
        }
   }

   FragColor = vec4(PI * irradiance / float(Nr), 1.0);
}