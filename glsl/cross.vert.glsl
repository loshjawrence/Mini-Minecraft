#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;

void main()
{
    gl_Position = u_ViewProj * u_Model * vs_Pos;
}
