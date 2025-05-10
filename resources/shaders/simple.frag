 #version 330 core  
 
out vec4 FragColor;  
in vec3 currentPos;

void main()  
{  
    FragColor = vec4(currentPos, 1.0);  
}