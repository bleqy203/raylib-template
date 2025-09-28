#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;      // model-view-projection
uniform mat4 matModel; // model transform

out vec3 fragPos;
out vec3 fragNormal;
out vec4 fragColor;

void main()
{
    fragPos = vec3(matModel * vec4(vertexPosition, 1.0));
    fragNormal = mat3(transpose(inverse(matModel))) * vertexNormal;
    fragColor = vertexColor;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
