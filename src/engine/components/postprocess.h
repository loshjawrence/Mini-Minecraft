#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "weathersystem.h"

#define MAXBOKEH 40


struct bokeh
{
    bool bUse;
    float alphaseed;
    float alpharandomseed;

    int snow;

    float WaitTime;

    glm::vec4 scale;

    // xy : UV,
    // z : angle,
    // a : alpha
    glm::vec4 Info;

    glm::vec4 Color;
    glm::vec4 Color2;
};

class PostProcess :  public Component
{
public:
    PostProcess();

    void Initialize(Camera* pMainCamera, std::string shadername)
    {
        bChanging = false;
        for(uint i=0; i<MAXBOKEH; i++)
        {
           m_bokeh[i].alphaseed = 1;
           m_bokeh[i].alpharandomseed = (float)rand()/RAND_MAX * interval;
           m_bokeh[i].WaitTime = 0;
           m_bokeh[i].snow = (int)((float)rand()/RAND_MAX * 4.0);
           m_bokeh[i].bUse = false;
           m_bokeh[i].Info = glm::vec4(0,0,0,0);

        }

        m_pMainCamera = pMainCamera;

        MeshRenderer * renderer = this->gameObject->AddComponent<MeshRenderer>();
        m_pMesh = MeshFactory::BuildQuad(false);

        renderer->SetMesh(m_pMesh);

        // Make sure it's being drawn at the front, and with no depth testing!
        m_Material = new Material(shadername);

        m_Material->SetFeature(GL_DEPTH_TEST, false);
        m_Material->SetFeature(GL_BLEND, true);
        m_Material->SetBlendOperation(Material::BlendOperation(GL_SRC_ALPHA, GL_ONE));
        m_Material->SetRenderingQueue(Material::PostProcess);

        //need to fix
        m_RainBokehTexture = AssetDatabase::GetInstance()->LoadTexture("/cis460-minecraft/textures/lensflare/particle.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);

        m_Snow01Texture = AssetDatabase::GetInstance()->LoadTexture("/cis460-minecraft/textures/lensflare/snow00.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
        m_Snow02Texture = AssetDatabase::GetInstance()->LoadTexture("/cis460-minecraft/textures/lensflare/snow01.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
        m_Snow03Texture = AssetDatabase::GetInstance()->LoadTexture("/cis460-minecraft/textures/lensflare/snow02.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
        m_Snow04Texture = AssetDatabase::GetInstance()->LoadTexture("/cis460-minecraft/textures/lensflare/snow03.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);


        m_Material->SetTexture("bokeh", m_RainBokehTexture);
        m_Material->SetTexture("SnowTex01", m_Snow01Texture);
        m_Material->SetTexture("SnowTex02", m_Snow02Texture);
        m_Material->SetTexture("SnowTex03", m_Snow03Texture);
        m_Material->SetTexture("SnowTex04", m_Snow04Texture);



        renderer->SetMaterial(m_Material);
    }

    virtual void Awake()
    {
        m_OldWeatherMode = m_WeatherMode = SUNNY;
        interval = 7.0;
    }

    //this should happen at rendering time
    virtual void Update()
    {
        //m_WeatherMode = sin( (Engine::Time()) + 1)*1.5;

        m_WeatherMode = WeatherSystem::GetWeatherMode();

        //Stop
        if(m_WeatherMode != m_OldWeatherMode)
        {
            bChanging = true;
        }

        //if(m_WeatherMode != SUNNY)
        //{
            m_pMainCamera->UpdateScreenSize();
            glm::mat4 InvVP = m_pMainCamera->GetViewProjectionMatrix();
            InvVP = glm::inverse(InvVP);
            m_pMesh->UpdateForScreenQaud(InvVP ,Engine::GetScreenSize().x, Engine::GetScreenSize().y);

            UpdateParticles();

            for(uint i=0; i<MAXBOKEH; i++)
            {
                std::string num =  QString::number(i).toStdString();
                std::string name;
                name = "bokehScale" + num;
                m_Material->SetVector(name, m_bokeh[i].scale);
                name = "bokehInfo"  + num;
                m_Material->SetVector(name, m_bokeh[i].Info);

                name = "Color_" + num;
                m_Material->SetVector(name, m_bokeh[i].Color);
                name = "Color2_"  + num;
                m_Material->SetVector(name, m_bokeh[i].Color2);

                name = "Color2_"  + num;
                m_Material->SetVector(name, m_bokeh[i].Color2);

                name = "Snow"  + num;
                m_Material->SetInt(name, m_bokeh[i].snow);
            }

            m_Material->SetInt("WeatherMode", m_OldWeatherMode);

        //}

        if(bChanging)
        {
           if(IsFinish())
               bChanging = false;
        }

        if(!bChanging)
         m_OldWeatherMode = m_WeatherMode;

    }

    bool IsFinish()
    {
        for(uint i=0; i<MAXBOKEH; i++)
        {
            if(m_bokeh[i].bUse == true)
            {
                return false;

            }
        }
        return true;
    }

    void UpdateParticles()
    {
        for(uint i=0; i<MAXBOKEH; i++)
        {
            if(m_bokeh[i].bUse)
            {
                float deltaTime;


                if(bChanging)
                {
                    deltaTime = Engine::DeltaTime()*10.0;
                }
                else
                {
                    deltaTime = Engine::DeltaTime();
                }




                if(m_bokeh[i].alphaseed > 0.6)
                 m_bokeh[i].alphaseed -= deltaTime;
                else
                 m_bokeh[i].alphaseed -= deltaTime*0.15f;

                if(m_bokeh[i].alphaseed < 0.0)
                {
                    m_bokeh[i].Info.a = m_bokeh[i].alphaseed = 0.0;

                    m_bokeh[i].alpharandomseed = (float)rand()/RAND_MAX * interval;
                    m_bokeh[i].snow = (int)((float)rand()/RAND_MAX * 4.0);

                    m_bokeh[i].bUse = false;
                }
                else
                    m_bokeh[i].Info.a = m_bokeh[i].alphaseed /* + sin( (Engine::Time() + m_bokeh[i].alpharandomseed)*2 )*0.1*/;
            }
            else
            {
              if(!bChanging)
              {
                  m_bokeh[i].WaitTime +=  Engine::DeltaTime();

                  if(m_bokeh[i].WaitTime > m_bokeh[i].alpharandomseed)
                  {
                      m_bokeh[i].WaitTime = 0;

                      float width = Engine::GetInstance()->GetContext()->width();
                      float height = Engine::GetInstance()->GetContext()->height();
                      float ratio = height/width;
                      //generate

                      m_bokeh[i].alphaseed = 1.0;

                      m_bokeh[i].bUse = true;

                      float innerscale = ((float)rand()/RAND_MAX)*10 + 20;

                      if((int)m_WeatherMode == SNOWY)
                          innerscale -= 10.0;

                      m_bokeh[i].scale = glm::vec4( innerscale*ratio  , innerscale, 0, 0);
                      m_bokeh[i].Info = glm::vec4((float)rand()/RAND_MAX , (float)rand()/RAND_MAX, ((float)rand()/RAND_MAX)* glm::pi<float>()*2, 1.0);

                      m_bokeh[i].Color = glm::vec4((float)rand()/RAND_MAX * 0.5 + 0.5, (float)rand()/RAND_MAX* 0.5 + 0.5, (float)rand()/RAND_MAX* 0.5 + 0.5, (float)rand()/RAND_MAX);
                      m_bokeh[i].Color2 = glm::vec4((float)rand()/RAND_MAX* 0.5 + 0.5, (float)rand()/RAND_MAX* 0.5 + 0.5, (float)rand()/RAND_MAX* 0.5 + 0.5, (float)rand()/RAND_MAX);
                  }
              }


            }
        }
    }

    bokeh m_bokeh[MAXBOKEH];

    Mesh* m_pMesh;
    Camera* m_pMainCamera;

    Material * m_Material;
    Texture* m_RainBokehTexture;
    Texture* m_Snow01Texture;
    Texture* m_Snow02Texture;
    Texture* m_Snow03Texture;
    Texture* m_Snow04Texture;

    int m_WeatherMode;
    int m_OldWeatherMode;
    float interval;
    bool bChanging;


};

#endif // POSTPROCESS_H
