#version 150

uniform vec4 u_Color;
uniform sampler2D u_MainTexture;

in vec2 fs_UV;

out vec4 out_Col;

void main()
{
    vec4 tex = texture(u_MainTexture, fs_UV);
    out_Col = tex * u_Color;
}
