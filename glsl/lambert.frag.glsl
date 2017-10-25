#version 150
// ^ Change this to version 130 if you have compatibility issues

uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform vec4 u_ViewVec;
uniform float u_Time;
uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;

uniform sampler2D u_ShadowMap01;
uniform sampler2D u_ShadowMap02;
uniform sampler2D u_ShadowMap03;

uniform vec4 u_DLightDirVec;
uniform vec4 u_DLightColor;
uniform mat4 u_ShadowViewProj;

uniform mat4 u_ShadowViewProjArray01;
uniform mat4 u_ShadowViewProjArray02;
uniform mat4 u_ShadowViewProjArray03;

uniform float u_DebugMode;

in vec4 fs_Nor;
in vec4 fs_Tan;
in vec4 fs_Bi;

in vec4 fs_LightVec;
in vec4 fs_Col;
in vec2 fs_UV;

in vec4 fs_position;

flat in int fs_Kind;
in float fogFactor;

out vec4 out_Col;

void main()
{

        float Seed = u_Time;
        float emissive = 0;
        vec4 diffuseColor = texture(u_DiffuseMap, fs_UV);


        //Still Lava Still Water
        if(fs_Kind == 9 || fs_Kind == 10)
        {
            vec4 CurrentLava = texture(u_DiffuseMap, vec2(fs_UV.x + floor(Seed*1.5)*0.0625 , fs_UV.y));
            vec4 NextLAva = texture(u_DiffuseMap, vec2(fs_UV.x + (floor(Seed*1.5) + 1)*0.0625, fs_UV.y));
            diffuseColor = mix(CurrentLava ,NextLAva, Seed*1.5 -  floor(Seed*1.5) );

            float fkind = fs_Kind;

            emissive = mix(1, 0, fkind - 9);
        }
        //Flow Lava Flow Water
        else if(fs_Kind == 11 || fs_Kind == 12 )
        {
            diffuseColor = texture(u_DiffuseMap, vec2(fs_UV.x + (Seed - floor(Seed))*0.0625, fs_UV.y));

            float fkind = fs_Kind;

            emissive = mix(1, 0, fkind - 11);
        }

        //Blinn-Phong

        vec4 normalColor = texture(u_NormalMap, fs_UV);

        normalColor = (normalColor-0.5)*2.0;
        normalColor.a = 0.0;

        mat3 TBN = mat3(vec3(fs_Tan), vec3(fs_Bi), vec3(fs_Nor));

        vec4 worldnormal = vec4(TBN * normalColor.rgb  , 0);
        worldnormal = normalize(worldnormal);

        float diffuseTerm = dot(normalize(worldnormal), normalize(-u_DLightDirVec));
        diffuseTerm = clamp(diffuseTerm, 0, 1);
        diffuseTerm = mix(diffuseTerm , 1, emissive);

        float diffuseTermforMoon = dot(normalize(worldnormal), normalize(u_DLightDirVec));
        diffuseTermforMoon = clamp(diffuseTermforMoon, 0, 0.2);
        diffuseTermforMoon = mix(diffuseTermforMoon , 1, emissive);


        vec4 ViewVec = normalize(vec4(-u_ViewVec.xyz,0));
        vec4 LightVec = normalize(vec4(-u_DLightDirVec.xyz,0));
        vec4 LightVecforMoon = normalize(vec4(u_DLightDirVec.xyz,0));

        vec4 halfvec = normalize(ViewVec + LightVec);
        vec4 halfvecforMoon = normalize(ViewVec + LightVecforMoon);

        float CosPow = clamp(100.0f * diffuseColor.a, 1, 100);
        float SpecIntnesity = pow( clamp(dot(normalize(worldnormal), normalize(halfvec)),0 ,1) , CosPow) * diffuseColor.a;
        float SpecIntnesityforMoon = pow( clamp(dot(normalize(worldnormal), normalize(halfvecforMoon)),0 ,1) , CosPow) * diffuseColor.a;

        vec4 SpecColor = vec4(1,1,1,1);
        SpecColor = mix(SpecColor , vec4(0,0,0,0), emissive);

        float ambientTerm = 0.2;
        float lightIntensity = clamp(diffuseTerm * u_DLightColor.a + ambientTerm, 0 ,1);
        lightIntensity = mix(lightIntensity , 1.0, emissive);
        float lightIntensityforMoon = clamp(diffuseTermforMoon * u_DLightColor.a + ambientTerm, 0 ,1);
        lightIntensityforMoon = mix(lightIntensityforMoon , 1.0, emissive);

        // Fog
        vec4 fogColor = u_DLightColor*0.2;
        float fog = clamp((fogFactor - 30)*0.02 , 0.0, 1.0); // Simple fog for now!
        fog =  clamp(1 - fog + u_DLightColor.a*0.3, 0, 1);

        //Cascade Shadow
        vec4 PosH01 = u_ShadowViewProjArray01 * fs_position;
        vec4 PosH02 = u_ShadowViewProjArray02 * fs_position;
        vec4 PosH03 = u_ShadowViewProjArray03 * fs_position;

        vec2 UVforShadow01 = vec2((PosH01.x + 1) * 0.5, ((PosH01.y + 1) * 0.5));
        vec2 UVforShadow02 = vec2((PosH02.x + 1) * 0.5, ((PosH02.y + 1) * 0.5));
        vec2 UVforShadow03 = vec2((PosH03.x + 1) * 0.5, ((PosH03.y + 1) * 0.5));

        float ShadowDepth01 = texture(u_ShadowMap01, UVforShadow01).x;
        float ShadowDepth02 = texture(u_ShadowMap02, UVforShadow02).x;
        float ShadowDepth03 = texture(u_ShadowMap03, UVforShadow03).x;

        float Depth01 = PosH01.z;
        float Depth02 = PosH02.z;
        float Depth03 = PosH03.z;

        vec4 shadowcolor = vec4(0,0,0,1);
        vec4 colorresult = vec4(((diffuseColor.rgb +  SpecColor.rgb*SpecIntnesity)*lightIntensity  + (diffuseColor.rgb + SpecColor.rgb*SpecIntnesityforMoon)* lightIntensityforMoon) * u_DLightColor.xyz , diffuseColor.a);

        if((UVforShadow01.x >= 0 && UVforShadow01.x <= 1  && UVforShadow01.y >= 0 && UVforShadow01.y <= 1))
        {
            if((Depth01 > ShadowDepth01+0.00075 || diffuseTerm == 0.0))
             colorresult =  mix(colorresult, shadowcolor, 0.3);

            colorresult = mix(colorresult, vec4(1,0,0,1), u_DebugMode);
        }
        else if((UVforShadow02.x >= 0 && UVforShadow02.x <= 1  && UVforShadow02.y >= 0 && UVforShadow02.y <= 1))
        {
            if(Depth02 > ShadowDepth02+0.006 || diffuseTerm == 0.0)
                colorresult =  mix(colorresult, shadowcolor, 0.3);

            colorresult = mix(colorresult, vec4(0,1,0,1), u_DebugMode);

        }
        else if((UVforShadow03.x >= 0 && UVforShadow03.x <= 1  && UVforShadow03.y >= 0 && UVforShadow03.y <= 1))
        {
            if(Depth03 > ShadowDepth03+0.007 || diffuseTerm == 0.0)
                colorresult =  mix(colorresult, shadowcolor, 0.3);

            colorresult = mix(colorresult, vec4(0,0,1,1), u_DebugMode);
        }

        out_Col = mix(fogColor, colorresult, fog);
}
