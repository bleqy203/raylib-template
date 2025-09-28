#version 330

in vec3 fragPos;
in vec3 fragNormal;
in vec4 fragColor;

uniform vec3 lightDir;   // direction of the sun, normalized
uniform vec3 lightColor; // usually white or yellow
uniform vec3 ambient;    // ambient light

out vec4 finalColor;

void main()
{
    vec3 norm = normalize(fragNormal);
    float diff = max(dot(norm, -lightDir), 0.0);

    vec3 color = ambient + (diff * lightColor);
    color *= fragColor.rgb;

    finalColor = vec4(color, fragColor.a);
}