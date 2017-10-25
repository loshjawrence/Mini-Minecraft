#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec2 vs_UV;

out vec4 fs_Nor;
out vec2 fs_UV;
out vec4 fs_Pos;


void main()
{

    fs_UV = vs_UV;
    fs_Nor = vs_Nor;

    vec4 modelposition = u_Model * vs_Pos;
    fs_Pos = modelposition;
    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * modelposition;

}
