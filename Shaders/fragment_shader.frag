#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 uColor;
uniform bool uUseTexture;
uniform float uAlpha;
uniform sampler2D uTexture;

// Osvetljenje (osnovni Phong model - za sada samo ambient)
uniform bool uUseLighting;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

void main() {
    vec4 finalColor;
    
    if (uUseTexture) {
        finalColor = texture(uTexture, TexCoord);
    } else {
        finalColor = uColor;
    }
    
    // Osvetljenje (za sada samo ambient komponenta)
    if (uUseLighting) {
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * uLightColor;
        vec3 result = ambient * finalColor.rgb;
        FragColor = vec4(result, finalColor.a * uAlpha);
    } else {
        FragColor = vec4(finalColor.rgb, finalColor.a * uAlpha);
    }
}
