#version 150

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
in vec4 vs_Nor;
in vec2 vs_UV;

out vec4 fs_Nor;
out vec4 fs_LightVec;
out vec2 fs_UV;

const vec4 lightPosition = vec4(10,30,40,0);

void main()
{
    fs_UV = vs_UV;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);

    vec4 worldPosition = u_Model * vs_Pos;
    fs_LightVec = lightPosition - worldPosition;
    gl_Position = u_ViewProj * worldPosition;
}
