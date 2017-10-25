#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;


uniform sampler2D u_BasicColorTexture;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec4 vs_Tan;
in vec4 vs_Bi;
in vec4 vs_Col;
in vec2 vs_UV;
in int vs_Kind;

out vec4 fs_Nor;
out vec4 fs_Tan;
out vec4 fs_Bi;

out vec4 fs_LightVec;

out vec4 fs_position;

out vec4 fs_Col;
out vec2 fs_UV;
//out vec2 fs_RealUV;
out float fogFactor;
flat out int fs_Kind;
const vec4 lightPosition = vec4(10,30,40,0);

void main()
{
    fs_Col = vs_Col;
    fs_UV = vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);
    fs_Tan = vec4(invTranspose * vec3(vs_Tan), 0);
    fs_Bi   = vec4(invTranspose * vec3(vs_Bi), 0);


    fs_LightVec = lightPosition;

    fs_position = u_Model * vs_Pos;

    vec4 p = u_ViewProj* fs_position;


    fs_Kind = vs_Kind;

    fogFactor = p.z;
    gl_Position = p;
}
