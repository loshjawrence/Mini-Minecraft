#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

uniform samplerCube u_CubeMap01;
uniform samplerCube u_CubeMap02;
uniform samplerCube u_CubeMap03;
uniform samplerCube u_CubeMap04;

uniform vec4 u_ViewVec;

uniform vec4 u_DLightColor;

uniform float u_DayTime;
uniform float u_Time;

in vec2 fs_UV;
in vec4 fs_Nor;
in vec4 fs_Pos;
out vec4 out_Col;

const vec4 lightPosition = vec4(10,30,40,0);

float PI = 3.141592;

float Fadefuction(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    vec4 ViewVec = normalize(vec4(-u_ViewVec.xyz,0));
    vec4 LightVec = normalize(vec4(lightPosition.xyz,0));
    vec4 WorldNormal = normalize(vec4(fs_Nor.xyz,0));

    //vec4 RelfectionVec = normalize(2*dot(ViewVec, WorldNormal)*WorldNormal - ViewVec);
    vec4 RelfectionVec =  rotationMatrix(vec3(0,1,0) , -u_DayTime*15 *PI/180.0) * vec4(fs_Pos.x, fs_Pos.y, fs_Pos.z, 0);

    vec4 _0hColor = texture(u_CubeMap01, normalize(RelfectionVec.xyz))*2.2;
    vec4 _6hColor = texture(u_CubeMap02, normalize(RelfectionVec.xyz));
    vec4 _12hColor = texture(u_CubeMap03, normalize(RelfectionVec.xyz));
    vec4 _18hColor = texture(u_CubeMap04, normalize(RelfectionVec.xyz));

    vec4 result = vec4(0,0,0,1);
    // Copy the color; there is no shading.
    if(u_DayTime >= 18.0)
    {
        result = (mix(_18hColor ,_0hColor, Fadefuction(clamp((u_DayTime - 18.0)*1,0,1))));
    }
    else if(u_DayTime >= 17.0)
    {
        result = (mix(_12hColor ,_18hColor, Fadefuction(clamp((u_DayTime - 17.0)*1,0,1))));
    }
    else if(u_DayTime >= 6.0)
    {
        result = (mix(_6hColor ,_12hColor, Fadefuction(clamp((u_DayTime - 6.0)*1,0,1))));
    }
    else if(u_DayTime >= 5.0)
    {
        result = (mix(_0hColor ,_6hColor, Fadefuction(clamp((u_DayTime - 5.0)*1,0,1))));
    }
    else if(u_DayTime >= 0.0)
    {
        result = _0hColor;
    }


    out_Col = u_DLightColor * result;
}
