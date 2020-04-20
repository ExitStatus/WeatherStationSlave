#ifndef _SENSORDISPLAY_H
#define _SENSORDISPLAY_H

class SensorDisplay
{
  protected:
    int _width;
    int _height;
    int _capy;
    
  public:
    virtual void InitRender() = 0;
    virtual void BackgroundRender() = 0;
    virtual void Logo() = 0;
    virtual void HasCapability(char *capability);
    virtual void Display() = 0;
    virtual void RenderTemperature(float temperature) = 0;
    virtual void RenderHumidity(float humidity) = 0;
    virtual void RenderMaxMinTemperature(float maxTemperature, float minTemperature);
    virtual void RenderMaxMinHumidity(float maxHumidity, float minHumidity);
};

#endif
