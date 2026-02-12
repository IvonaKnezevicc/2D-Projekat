#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 uColor;
uniform bool uUseTexture;
uniform float uAlpha;
uniform sampler2D uTexture;

uniform bool uUseLighting;
uniform bool uHallLightEnabled;
uniform bool uScreenLightEnabled;
uniform vec3 uHallLightPos;
uniform vec3 uHallLightColor;
uniform vec3 uScreenLightPos;
uniform vec3 uScreenLightColor;
uniform vec3 uViewPos;
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;

vec3 calculateLight(vec3 lightPos, vec3 lightColor, vec3 norm, vec3 viewDir, vec3 baseRgb) {
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = pow(max(dot(norm, lightDir), 0.0), 0.75);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess) * uSpecularStrength;

    vec3 ambient = uAmbientStrength * lightColor * baseRgb;
    vec3 diffuse = diff * lightColor * baseRgb;
    vec3 specular = spec * lightColor;
    return ambient + diffuse + specular;
}

void main() {
    vec4 baseColor;
    
    if (uUseTexture) {
        baseColor = texture(uTexture, TexCoord) * uColor;
    } else {
        baseColor = uColor;
    }
    
    if (!uUseLighting) {
        FragColor = vec4(baseColor.rgb, baseColor.a * uAlpha);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 litColor = vec3(0.0);

    if (uHallLightEnabled) {
        litColor += calculateLight(uHallLightPos, uHallLightColor, norm, viewDir, baseColor.rgb);
    }

    if (uScreenLightEnabled) {
        litColor += calculateLight(uScreenLightPos, uScreenLightColor, norm, viewDir, baseColor.rgb);
    }

    FragColor = vec4(litColor, baseColor.a * uAlpha);
}
