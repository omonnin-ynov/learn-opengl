#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 0) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;
  
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
}