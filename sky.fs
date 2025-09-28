#version 330

in vec3 fragPos;
out vec4 finalColor;

uniform float timeOfDay; // [0,1], controlled by C code

vec3 getSkyColor(float t, float y)
{
    // Define base colors
    vec3 midnight = vec3(0.02, 0.02, 0.08);
    vec3 sunrise  = vec3(0.9, 0.4, 0.2);
    vec3 noon     = vec3(0.4, 0.7, 1.0);
    vec3 sunset   = vec3(0.9, 0.3, 0.2);

    vec3 col;

    if (t < 0.25) {
        // Midnight → Sunrise
        float f = t / 0.25;
        col = mix(midnight, sunrise, f);
    }
    else if (t < 0.5) {
        // Sunrise → Noon
        float f = (t - 0.25) / 0.25;
        col = mix(sunrise, noon, f);
    }
    else if (t < 0.75) {
        // Noon → Sunset
        float f = (t - 0.5) / 0.25;
        col = mix(noon, sunset, f);
    }
    else {
        // Sunset → Midnight
        float f = (t - 0.75) / 0.25;
        col = mix(sunset, midnight, f);
    }

    // Horizon brighter, zenith darker
    float grad = clamp(y * 0.5 + 0.5, 0.0, 1.0);
    return mix(vec3(0.1,0.1,0.1), col, grad);
}

void main()
{
    float y = normalize(fragPos).y;
    vec3 sky = getSkyColor(timeOfDay, y);
    finalColor = vec4(sky, 1.0);
}
