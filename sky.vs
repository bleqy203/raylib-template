#version 330

in vec3 vertexPosition;

out vec3 fragPos;

uniform mat4 mvp;

void main()
{
    fragPos = vertexPosition;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
