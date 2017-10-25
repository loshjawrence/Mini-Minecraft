#version 150

uniform sampler2D u_bokeh;

uniform sampler2D u_SnowTex01;
uniform sampler2D u_SnowTex02;
uniform sampler2D u_SnowTex03;
uniform sampler2D u_SnowTex04;

// hahahahahah
uniform int u_Snow0;
uniform int u_Snow1;
uniform int u_Snow2;
uniform int u_Snow3;
uniform int u_Snow4;
uniform int u_Snow5;
uniform int u_Snow6;
uniform int u_Snow7;
uniform int u_Snow8;
uniform int u_Snow9;
uniform int u_Snow10;
uniform int u_Snow11;
uniform int u_Snow12;
uniform int u_Snow13;
uniform int u_Snow14;
uniform int u_Snow15;
uniform int u_Snow16;
uniform int u_Snow17;
uniform int u_Snow18;
uniform int u_Snow19;
uniform int u_Snow20;
uniform int u_Snow21;
uniform int u_Snow22;
uniform int u_Snow23;
uniform int u_Snow24;
uniform int u_Snow25;
uniform int u_Snow26;
uniform int u_Snow27;
uniform int u_Snow28;
uniform int u_Snow29;
uniform int u_Snow30;
uniform int u_Snow31;
uniform int u_Snow32;
uniform int u_Snow33;
uniform int u_Snow34;
uniform int u_Snow35;
uniform int u_Snow36;
uniform int u_Snow37;
uniform int u_Snow38;
uniform int u_Snow39;

uniform vec4 u_bokehInfo0;
uniform vec4 u_bokehInfo1;
uniform vec4 u_bokehInfo2;
uniform vec4 u_bokehInfo3;
uniform vec4 u_bokehInfo4;
uniform vec4 u_bokehInfo5;
uniform vec4 u_bokehInfo6;
uniform vec4 u_bokehInfo7;
uniform vec4 u_bokehInfo8;
uniform vec4 u_bokehInfo9;
uniform vec4 u_bokehInfo10;
uniform vec4 u_bokehInfo11;
uniform vec4 u_bokehInfo12;
uniform vec4 u_bokehInfo13;
uniform vec4 u_bokehInfo14;
uniform vec4 u_bokehInfo15;
uniform vec4 u_bokehInfo16;
uniform vec4 u_bokehInfo17;
uniform vec4 u_bokehInfo18;
uniform vec4 u_bokehInfo19;
uniform vec4 u_bokehInfo20;
uniform vec4 u_bokehInfo21;
uniform vec4 u_bokehInfo22;
uniform vec4 u_bokehInfo23;
uniform vec4 u_bokehInfo24;
uniform vec4 u_bokehInfo25;
uniform vec4 u_bokehInfo26;
uniform vec4 u_bokehInfo27;
uniform vec4 u_bokehInfo28;
uniform vec4 u_bokehInfo29;
uniform vec4 u_bokehInfo30;
uniform vec4 u_bokehInfo31;
uniform vec4 u_bokehInfo32;
uniform vec4 u_bokehInfo33;
uniform vec4 u_bokehInfo34;
uniform vec4 u_bokehInfo35;
uniform vec4 u_bokehInfo36;
uniform vec4 u_bokehInfo37;
uniform vec4 u_bokehInfo38;
uniform vec4 u_bokehInfo39;

uniform vec4 u_bokehScale0;
uniform vec4 u_bokehScale1;
uniform vec4 u_bokehScale2;
uniform vec4 u_bokehScale3;
uniform vec4 u_bokehScale4;
uniform vec4 u_bokehScale5;
uniform vec4 u_bokehScale6;
uniform vec4 u_bokehScale7;
uniform vec4 u_bokehScale8;
uniform vec4 u_bokehScale9;
uniform vec4 u_bokehScale10;
uniform vec4 u_bokehScale11;
uniform vec4 u_bokehScale12;
uniform vec4 u_bokehScale13;
uniform vec4 u_bokehScale14;
uniform vec4 u_bokehScale15;
uniform vec4 u_bokehScale16;
uniform vec4 u_bokehScale17;
uniform vec4 u_bokehScale18;
uniform vec4 u_bokehScale19;
uniform vec4 u_bokehScale20;
uniform vec4 u_bokehScale21;
uniform vec4 u_bokehScale22;
uniform vec4 u_bokehScale23;
uniform vec4 u_bokehScale24;
uniform vec4 u_bokehScale25;
uniform vec4 u_bokehScale26;
uniform vec4 u_bokehScale27;
uniform vec4 u_bokehScale28;
uniform vec4 u_bokehScale29;
uniform vec4 u_bokehScale30;
uniform vec4 u_bokehScale31;
uniform vec4 u_bokehScale32;
uniform vec4 u_bokehScale33;
uniform vec4 u_bokehScale34;
uniform vec4 u_bokehScale35;
uniform vec4 u_bokehScale36;
uniform vec4 u_bokehScale37;
uniform vec4 u_bokehScale38;
uniform vec4 u_bokehScale39;

uniform vec4 u_Color_0;
uniform vec4 u_Color_1;
uniform vec4 u_Color_2;
uniform vec4 u_Color_3;
uniform vec4 u_Color_4;
uniform vec4 u_Color_5;
uniform vec4 u_Color_6;
uniform vec4 u_Color_7;
uniform vec4 u_Color_8;
uniform vec4 u_Color_9;
uniform vec4 u_Color_10;
uniform vec4 u_Color_11;
uniform vec4 u_Color_12;
uniform vec4 u_Color_13;
uniform vec4 u_Color_14;
uniform vec4 u_Color_15;
uniform vec4 u_Color_16;
uniform vec4 u_Color_17;
uniform vec4 u_Color_18;
uniform vec4 u_Color_19;
uniform vec4 u_Color_20;
uniform vec4 u_Color_21;
uniform vec4 u_Color_22;
uniform vec4 u_Color_23;
uniform vec4 u_Color_24;
uniform vec4 u_Color_25;
uniform vec4 u_Color_26;
uniform vec4 u_Color_27;
uniform vec4 u_Color_28;
uniform vec4 u_Color_29;
uniform vec4 u_Color_30;
uniform vec4 u_Color_31;
uniform vec4 u_Color_32;
uniform vec4 u_Color_33;
uniform vec4 u_Color_34;
uniform vec4 u_Color_35;
uniform vec4 u_Color_36;
uniform vec4 u_Color_37;
uniform vec4 u_Color_38;
uniform vec4 u_Color_39;

uniform vec4 u_Color2_0;
uniform vec4 u_Color2_1;
uniform vec4 u_Color2_2;
uniform vec4 u_Color2_3;
uniform vec4 u_Color2_4;
uniform vec4 u_Color2_5;
uniform vec4 u_Color2_6;
uniform vec4 u_Color2_7;
uniform vec4 u_Color2_8;
uniform vec4 u_Color2_9;
uniform vec4 u_Color2_10;
uniform vec4 u_Color2_11;
uniform vec4 u_Color2_12;
uniform vec4 u_Color2_13;
uniform vec4 u_Color2_14;
uniform vec4 u_Color2_15;
uniform vec4 u_Color2_16;
uniform vec4 u_Color2_17;
uniform vec4 u_Color2_18;
uniform vec4 u_Color2_19;
uniform vec4 u_Color2_20;
uniform vec4 u_Color2_21;
uniform vec4 u_Color2_22;
uniform vec4 u_Color2_23;
uniform vec4 u_Color2_24;
uniform vec4 u_Color2_25;
uniform vec4 u_Color2_26;
uniform vec4 u_Color2_27;
uniform vec4 u_Color2_28;
uniform vec4 u_Color2_29;
uniform vec4 u_Color2_30;
uniform vec4 u_Color2_31;
uniform vec4 u_Color2_32;
uniform vec4 u_Color2_33;
uniform vec4 u_Color2_34;
uniform vec4 u_Color2_35;
uniform vec4 u_Color2_36;
uniform vec4 u_Color2_37;
uniform vec4 u_Color2_38;
uniform vec4 u_Color2_39;

uniform vec4 u_DLightColor;

uniform int u_WeatherMode;

in vec4 fs_Col;
in vec2 fs_UV;

out vec4 out_Col;



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

vec4 GetColor(vec2 UV,  vec4 BokehInfo,  vec2 BokehScale, vec4 color01, vec4 color02, int snow)
{
    vec2 UV0 = (UV.xy - BokehInfo.xy)*BokehScale.xy;
    vec4 result0 = rotationMatrix(vec3(0,0,1),BokehInfo.z) * vec4(UV0.x, UV0.y, 0, 0);
    UV0 = vec2(result0.x, result0.y);

    vec4 RESULT;

    if(u_WeatherMode == 1)
    {
        RESULT = BokehInfo.a  * texture(u_bokeh, UV0) * mix(color01, color02, BokehInfo.a);
    }
    else if(u_WeatherMode  == 2)
    {
        vec4 textureColor;

        if(snow == 0)
          textureColor = texture(u_SnowTex01, UV0);
        else if(snow == 1)
           textureColor = texture(u_SnowTex02, UV0);
        else if(snow == 2)
           textureColor = texture(u_SnowTex03, UV0);
        else if(snow == 3)
           textureColor = texture(u_SnowTex04, UV0);

        RESULT = BokehInfo.a  * textureColor * vec4(1,1,1,1);
    }

    /*
    vec4 result_Rain = BokehInfo.a  * texture(u_bokeh, UV0) * mix(color01, color02, BokehInfo.a);



    vec4 result_Snow = BokehInfo.a  * textureColor * vec4(1,1,1,1);

    vec4 RESULT;


    if(u_WeatherMode >= 0 && u_WeatherMode <= 1)
    {
      RESULT = u_WeatherMode * result_Rain;
    }
    else if(u_WeatherMode > 1 && u_WeatherMode <= 2)
    {
      RESULT = mix(result_Rain, result_Snow, u_WeatherMode - 1.0);
    }
    else if(u_WeatherMode > 2 && u_WeatherMode <= 3)
    {
      RESULT = (3.0 - u_WeatherMode) * result_Snow;
    }
    */

    return RESULT;
}

void main()
{
    // Copy the color; there is no shading.
    //out_Col = vec4(fs_UV.x, fs_UV.y , 0, 1.0);

    //Rain or Snow
    if(u_WeatherMode != 0)
    {
        vec4 rC0 = GetColor(fs_UV.xy, u_bokehInfo0, u_bokehScale0.xy, u_Color_0, u_Color2_0, u_Snow0);
        vec4 rC1 = GetColor(fs_UV.xy, u_bokehInfo1, u_bokehScale1.xy, u_Color_1, u_Color2_1, u_Snow1);
        vec4 rC2 = GetColor(fs_UV.xy, u_bokehInfo2, u_bokehScale2.xy, u_Color_2, u_Color2_2, u_Snow2);
        vec4 rC3 = GetColor(fs_UV.xy, u_bokehInfo3, u_bokehScale3.xy, u_Color_3, u_Color2_3, u_Snow3);
        vec4 rC4 = GetColor(fs_UV.xy, u_bokehInfo4, u_bokehScale4.xy, u_Color_4, u_Color2_4, u_Snow4);
        vec4 rC5 = GetColor(fs_UV.xy, u_bokehInfo5, u_bokehScale5.xy, u_Color_5, u_Color2_5, u_Snow5);
        vec4 rC6 = GetColor(fs_UV.xy, u_bokehInfo6, u_bokehScale6.xy, u_Color_6, u_Color2_6, u_Snow6);
        vec4 rC7 = GetColor(fs_UV.xy, u_bokehInfo7, u_bokehScale7.xy, u_Color_7, u_Color2_7, u_Snow7);
        vec4 rC8 = GetColor(fs_UV.xy, u_bokehInfo8, u_bokehScale8.xy, u_Color_8, u_Color2_8, u_Snow8);
        vec4 rC9 = GetColor(fs_UV.xy, u_bokehInfo9, u_bokehScale9.xy, u_Color_9, u_Color2_9, u_Snow9);
        vec4 rC10 = GetColor(fs_UV.xy, u_bokehInfo10, u_bokehScale10.xy, u_Color_10, u_Color2_10, u_Snow10);
        vec4 rC11 = GetColor(fs_UV.xy, u_bokehInfo11, u_bokehScale11.xy, u_Color_11, u_Color2_11, u_Snow11);
        vec4 rC12 = GetColor(fs_UV.xy, u_bokehInfo12, u_bokehScale12.xy, u_Color_12, u_Color2_12, u_Snow12);
        vec4 rC13 = GetColor(fs_UV.xy, u_bokehInfo13, u_bokehScale13.xy, u_Color_13, u_Color2_13, u_Snow13);
        vec4 rC14 = GetColor(fs_UV.xy, u_bokehInfo14, u_bokehScale14.xy, u_Color_14, u_Color2_14, u_Snow14);
        vec4 rC15 = GetColor(fs_UV.xy, u_bokehInfo15, u_bokehScale15.xy, u_Color_15, u_Color2_15, u_Snow15);
        vec4 rC16 = GetColor(fs_UV.xy, u_bokehInfo16, u_bokehScale16.xy, u_Color_16, u_Color2_16, u_Snow16);
        vec4 rC17 = GetColor(fs_UV.xy, u_bokehInfo17, u_bokehScale17.xy, u_Color_17, u_Color2_17, u_Snow17);
        vec4 rC18 = GetColor(fs_UV.xy, u_bokehInfo18, u_bokehScale18.xy, u_Color_18, u_Color2_18, u_Snow18);
        vec4 rC19 = GetColor(fs_UV.xy, u_bokehInfo19, u_bokehScale19.xy, u_Color_19, u_Color2_19, u_Snow19);

        vec4 rC20 = GetColor(fs_UV.xy, u_bokehInfo20, u_bokehScale20.xy, u_Color_20, u_Color2_20, u_Snow20);
        vec4 rC21 = GetColor(fs_UV.xy, u_bokehInfo21, u_bokehScale21.xy, u_Color_21, u_Color2_21, u_Snow21);
        vec4 rC22 = GetColor(fs_UV.xy, u_bokehInfo22, u_bokehScale22.xy, u_Color_22, u_Color2_22, u_Snow22);
        vec4 rC23 = GetColor(fs_UV.xy, u_bokehInfo23, u_bokehScale23.xy, u_Color_23, u_Color2_23, u_Snow23);
        vec4 rC24 = GetColor(fs_UV.xy, u_bokehInfo24, u_bokehScale24.xy, u_Color_24, u_Color2_24, u_Snow24);
        vec4 rC25 = GetColor(fs_UV.xy, u_bokehInfo25, u_bokehScale25.xy, u_Color_25, u_Color2_25, u_Snow25);
        vec4 rC26 = GetColor(fs_UV.xy, u_bokehInfo26, u_bokehScale26.xy, u_Color_26, u_Color2_26, u_Snow26);
        vec4 rC27 = GetColor(fs_UV.xy, u_bokehInfo27, u_bokehScale27.xy, u_Color_27, u_Color2_27, u_Snow27);
        vec4 rC28 = GetColor(fs_UV.xy, u_bokehInfo28, u_bokehScale28.xy, u_Color_28, u_Color2_28, u_Snow28);
        vec4 rC29 = GetColor(fs_UV.xy, u_bokehInfo29, u_bokehScale29.xy, u_Color_29, u_Color2_29, u_Snow29);
        vec4 rC30 = GetColor(fs_UV.xy, u_bokehInfo30, u_bokehScale30.xy, u_Color_30, u_Color2_30, u_Snow30);
        vec4 rC31 = GetColor(fs_UV.xy, u_bokehInfo31, u_bokehScale31.xy, u_Color_31, u_Color2_31, u_Snow31);
        vec4 rC32 = GetColor(fs_UV.xy, u_bokehInfo32, u_bokehScale32.xy, u_Color_32, u_Color2_32, u_Snow32);
        vec4 rC33 = GetColor(fs_UV.xy, u_bokehInfo33, u_bokehScale33.xy, u_Color_33, u_Color2_33, u_Snow33);
        vec4 rC34 = GetColor(fs_UV.xy, u_bokehInfo34, u_bokehScale34.xy, u_Color_34, u_Color2_34, u_Snow34);
        vec4 rC35 = GetColor(fs_UV.xy, u_bokehInfo35, u_bokehScale35.xy, u_Color_35, u_Color2_35, u_Snow35);
        vec4 rC36 = GetColor(fs_UV.xy, u_bokehInfo36, u_bokehScale36.xy, u_Color_36, u_Color2_36, u_Snow36);
        vec4 rC37 = GetColor(fs_UV.xy, u_bokehInfo37, u_bokehScale37.xy, u_Color_37, u_Color2_37, u_Snow37);
        vec4 rC38 = GetColor(fs_UV.xy, u_bokehInfo38, u_bokehScale38.xy, u_Color_38, u_Color2_38, u_Snow38);
        vec4 rC39 = GetColor(fs_UV.xy, u_bokehInfo39, u_bokehScale39.xy, u_Color_39, u_Color2_39, u_Snow39);

        vec4 part01 = rC0 + rC1 + rC2 + rC3+ rC4 + rC5 + rC6 + rC7 +rC8 + rC9 + rC10 + rC11 + rC12 + rC13+ rC14 + rC15 + rC16 + rC17 +rC18 + rC19;
        vec4 part02 = rC20 + rC21 + rC22 + rC23+ rC24 + rC25 + rC26 + rC27 +rC28 + rC29 + rC30 + rC31 + rC32 + rC33+ rC34 + rC35 + rC36 + rC37 +rC38 + rC39;

        out_Col = part01 + part02;
    }
    else
        out_Col = vec4(0,0,0,0);


}
