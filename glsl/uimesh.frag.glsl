#version 150

uniform vec4 u_Color;
uniform sampler2D u_MainTexture;

in vec2 fs_UV;
in vec4 fs_Nor;
in vec4 fs_LightVec;

out vec4 out_Col;

// UI Meshes are just simple lamberts with a fixed light
void main()
{
    vec3 lightDir = normalize(fs_LightVec.xyz);
    vec3  n = normalize(fs_Nor.xyz);
    float diffuse = dot(lightDir, n) * .5 + .5;

    vec4 tex = texture(u_MainTexture, fs_UV);
    out_Col =  tex * u_Color * diffuse;
}
