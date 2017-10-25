#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "weathersystem.h"

#define DISTANCE_FROM_SUN 1000

class DirectionalLight :  public Component
{
public:
    DirectionalLight()
    {
        bPause = false;
        daytimescale = 0.04;
        m_BlendAlpha = 0.0;
        bChanging = false;

       m_OldWeatherMode = m_WeatherMode = SUNNY;

       m_direction_0h = glm::vec4(0,1,0,0);
       m_direction_6h = glm::vec4(-1,0,-1,0);
       m_direction_12h = glm::vec4(0,-1,0,0);
       m_direction_18h = glm::vec4(1,0,1,0);

       m_color_0h = glm::vec4(1,1,1,0.2);
       m_color_6h = glm::vec4(0.97255,0.737255,1,0.5);
       m_color_12h = glm::vec4(1,1,1,1);
       m_color_18h = glm::vec4(0.913725,0.478431,0.156863,0.8);
    }

    float Fadefuction(float t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float saturate(float val, float min, float max, bool bFade)
    {
        if(val < min)
            val = min;
        else if(val > max)
            val = max;

        if(bFade)
           val = Fadefuction(val);

        return val;
    }

    void CalDirection()
    {
        float time = m_daytime;

        //Dir
        if(time >= 18.0)
        {
            float seed = saturate((time - 18.0)/6.0, 0, 1, false);
            m_direction = glm::normalize(glm::normalize(m_direction_18h)*(1-seed) +  glm::normalize(m_direction_0h)*seed);
        }
        else if(time >= 12.0)
        {
            float seed = saturate((time - 12.0)/6.0, 0, 1, false);
            m_direction = glm::normalize(glm::normalize(m_direction_12h)*(1-seed) +  glm::normalize(m_direction_18h)*seed);
        }
        else if(time >= 6.0)
        {
            float seed = saturate((time - 6.0)/6.0, 0, 1, false);
            m_direction = glm::normalize(glm::normalize(m_direction_6h)*(1-seed) +  glm::normalize(m_direction_12h)*seed);
        }
        else if(time >= 0.0)
        {
            float seed = saturate((time - 0.0)/6.0, 0, 1, false);
            m_direction = glm::normalize(glm::normalize(m_direction_0h)*(1-seed) +  glm::normalize(m_direction_6h)*seed);
        }



    }

    void CalColor()
    {

        float time = m_daytime;

        //Color
        if(time >= 18.0)
        {
            float seed = saturate((time - 18.0), 0, 1, true);
            m_color = ((m_color_18h)*(1-seed) +  (m_color_0h)*seed);
        }
        else if(time >= 17.0)
        {
            float seed = saturate((time - 17.0), 0, 1, true);
            m_color = ((m_color_12h)*(1-seed) +  (m_color_18h)*seed);
        }
        else if(time >= 6.0)
        {
            float seed = saturate((time - 6.0), 0, 1, true);
            m_color = ((m_color_6h)*(1-seed) +  (m_color_12h)*seed);
        }
        else if(time >= 5.0)
        {
            float seed = saturate((time - 5.0), 0, 1, true);
            m_color = ((m_color_0h)*(1-seed) +  (m_color_6h)*seed);
        }
        else if(time >= 0.0)
        {
            m_color = m_color_0h;
        }

        if(bChanging)
        {
          m_BlendAlpha += Engine::DeltaTime();


          if((m_WeatherMode == RAINY && m_OldWeatherMode == SUNNY) ||
             (m_WeatherMode == SNOWY && m_OldWeatherMode == SUNNY)  )
          {
              m_color = glm::mix(m_color, glm::vec4(0.3,0.3,0.3,m_color.a), glm::clamp(m_BlendAlpha*0.5,0.0,1.0));
          }
          else if((m_WeatherMode == SUNNY && m_OldWeatherMode == RAINY) ||
                    (m_WeatherMode == SUNNY && m_OldWeatherMode == SNOWY))
          {
              m_color = glm::mix(glm::vec4(0.3,0.3,0.3,m_color.a) ,m_color , glm::clamp(m_BlendAlpha*0.5,0.0,1.0));
          }
          else if((m_WeatherMode == RAINY && m_OldWeatherMode == SNOWY) ||
                  (m_OldWeatherMode == RAINY && m_WeatherMode == SNOWY)
                  )
          {
              m_color = glm::vec4(0.3,0.3,0.3,m_color.a);
          }

          if(m_BlendAlpha >= 2.0)
          {
                  bChanging = false;
                  m_BlendAlpha = 0.0;
          }
        }
        else if(m_OldWeatherMode == RAINY || m_OldWeatherMode == SNOWY)
        {
            m_color = glm::vec4(0.3,0.3,0.3,m_color.a);
        }

    }

    virtual void Update()
    {       
        m_WeatherMode = WeatherSystem::GetWeatherMode();


        if(m_WeatherMode != m_OldWeatherMode)
        {
            bChanging = true;
        }

        //Pause DayTime
        if (!bPause)
        {

        m_Time += Engine::DeltaTime()*daytimescale;

        m_daytime = m_Time;
        m_daytime = m_daytime - ((int)(m_daytime/24))*24;

        }



        CalDirection();
        CalColor();



        if(!bChanging)
         m_OldWeatherMode = m_WeatherMode;
    }

    void Pause()
    {
        bPause = !bPause;
    }

    void Faster()
    {
        daytimescale += 0.005;
    }

    void Slower()
    {
        daytimescale -= 0.005;
        if(daytimescale < 0.00001)
            daytimescale = 0.00001;

    }

    bool bPause;
    float daytimescale;

    glm::vec4 m_direction;


    glm::vec4 m_direction_0h;
    glm::vec4 m_direction_6h;
    glm::vec4 m_direction_12h;
    glm::vec4 m_direction_18h;

    glm::vec4 m_color;

    glm::vec4 m_color_0h;
    glm::vec4 m_color_6h;
    glm::vec4 m_color_12h;
    glm::vec4 m_color_18h;

    float m_daytime;
    float m_Time;

    int m_WeatherMode;
    int m_OldWeatherMode;
    float m_BlendAlpha;
    bool bChanging;

};

#endif // DIRECTIONALLIGHT_H
