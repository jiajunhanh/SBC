#version 460 compatibility
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoord);
}