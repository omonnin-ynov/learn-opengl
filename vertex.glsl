#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
  
out vec4 vertexPos; // specify a color output to the fragment shader

uniform float HOffset;

void main()
{
    gl_Position = vec4(aPos.x + HOffset, -aPos.y, aPos.z, 1); // see how we directly give a vec3 to vec4's constructor
    vertexPos = gl_Position; // set the output variable to a dark-red color
}