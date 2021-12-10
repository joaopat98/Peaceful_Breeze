#version 430 core
out float cocBlurred;
  
in vec2 TexCoord;
  
uniform sampler2D Texture;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(Texture, 0));
    float result = 0.0;
    for (int x = -4; x < 4; ++x) 
    {
        for (int y = -4; y < 4; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(Texture, TexCoord + offset).r;
        }
    }
    cocBlurred = result / (8.0 * 8.0);
}