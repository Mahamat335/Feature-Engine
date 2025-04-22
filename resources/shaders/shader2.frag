 #version 330 core  
uniform float alphaValue; 
out vec4 FragColor;  
in vec3 currentPos;
void main()  
{  
    FragColor = vec4(currentPos * alphaValue, 1.5f - alphaValue);  
}