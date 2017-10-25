#ifndef WEATHERSYSTEM_H
#define WEATHERSYSTEM_H

#include "../gameobject.h"
#include <openglcontext.h>
#include "../../engine/common.h"

enum WeahterMode
{
    SUNNY = 0, RAINY = 1, SNOWY = 2
};



class WeatherSystem :  public Component
{
public:
    WeatherSystem();
    ~WeatherSystem()
    {
        delete Instance;
    }

    static WeatherSystem* Instance;

    virtual void Awake()
    {
            Instance = new WeatherSystem();
    }

    //this should happen at rendering time
    virtual void Update()
    {
        //TODO
        //Procedurally create weather such as rain, snow, and sun. Allow these weather patches to move throughout the world.

        //int seed = (int)(Engine::Time()*0.1);
        //Instance->WEATHER_MODE = (int)seed%3;
    }

    static int GetWeatherMode()
    {
        return Instance->WEATHER_MODE;
    }

    int WEATHER_MODE;


};

#endif // WEATHERSYSTEM_H
