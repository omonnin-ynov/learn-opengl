#version 330 core
out vec4 FragColor;
  
in vec4 vertexPos; // the input variable from the vertex shader (same name and same type)  
in vec2 TexCoord;

uniform float mixValue;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = mix(texture2D(texture1, TexCoord), texture2D(texture2, vec2(TexCoord.x, TexCoord.y)), mixValue);
} 