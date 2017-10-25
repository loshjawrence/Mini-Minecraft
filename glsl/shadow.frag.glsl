#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in float depth;

out vec4 out_Col;

void main()
{
    out_Col = vec4(depth,depth,depth,1);
}
