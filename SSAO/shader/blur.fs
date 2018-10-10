#version 430 core
out float FragColor;

in vec2 Tex;

uniform sampler2D ssao;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssao, 0));
    float res = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            res += texture(ssao, Tex + offset).r;
        }
    }

    FragColor = res / 16;
}