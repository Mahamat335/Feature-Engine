#version 330 core
layout (location = 0) in vec3 aPos;
uniform float alphaValue;
out vec3 currentPos;
void main()
{
    gl_Position = vec4(aPos.x + alphaValue*2 -1.5f, aPos.y, aPos.z, 1.0);
    currentPos = aPos;
}