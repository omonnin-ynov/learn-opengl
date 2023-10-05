#version 330 core
out vec4 FragColor;
  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture2D(ourTexture, TexCoord);
} 