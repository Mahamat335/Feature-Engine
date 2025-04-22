#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;


out vec3 Normal;
out vec3 FragPos;  
out vec3 Color;
out vec4 fragPosLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightProjection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    //vec3 objectPos = vec3(model[0][3], model[1][3], model[2][3]);
    //FragPos = objectPos * aPos;
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    Color = vec3(1.0, 0.5, 0.31);
    fragPosLight = lightProjection * gl_Position;
}