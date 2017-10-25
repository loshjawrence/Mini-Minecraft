#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform mat4 u_Model;

uniform mat4 u_ShadowViewProj;

in vec4 vs_Pos;

out float depth;
void main()
{

    vec4 modelposition = u_Model * vs_Pos;
    vec4 shadowPos = u_ShadowViewProj * modelposition;
    depth = shadowPos.z;
    //built-in things to pass down the pipeline
    gl_Position = shadowPos;

}
