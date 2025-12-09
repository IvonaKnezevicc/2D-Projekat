#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform float uAlpha;

void main()
{
    if (uUseTexture)
    {
        vec4 texColor = texture(uTexture, TexCoord);
        FragColor = vec4(texColor.rgb, texColor.a * uAlpha);
    }
    else
    {
        FragColor = vec4(uColor.rgb, uColor.a * uAlpha);
    }
}

