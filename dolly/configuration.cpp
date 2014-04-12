#include"configuration.h"
#include <EEPROM.h>

struct configuration conf;
struct runtime_database db;

void loadDefaults()
{
    conf.version = VERSION;
     conf.stepFromSideToSide=0;

    conf.distance=100; // The maximum distance as percentage
    conf.fromLeftToRight=1; //if 1, Move camere from left to Rigth. If 0, Move camera from rigth to left
    
    conf.camera_FPM=10;    //Frames per minutes
    conf.camera_moving_time=60; //camera moving time from side to side

    conf.projectionFrameRate=24; //frame per second    
    
    conf.calibrated = 0;
 
    conf.dolly_time = 10;
 
    db.isStarted = 0; //Timelapse is not started yet
    db.photoCount = 0;
     
}
void initializeConfiguration()
{
    loadConfig();
    if(conf.version!=VERSION)
    {
      loadDefaults();
      saveConfig();
    }
}


void saveConfig()
{
  for (int i = 0; i < sizeof(conf); i++)
  {
    char *p = (char*)&conf;
    EEPROM.write(i, p[i]);
  }
}
void loadConfig()
{
  for (int i = 0; i < sizeof(conf); i++)
  {
    char *p = (char*)&conf;
    p[i] = EEPROM.read(i);
  }
}
