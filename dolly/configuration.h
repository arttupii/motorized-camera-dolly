#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#define VERSION 3

struct configuration{
    int version;
    
    unsigned long stepFromSideToSide;

    int distance; // The maximum distance as percentage
    int fromLeftToRight; //if 1, Move camere from left to Rigth. If 0, Move camera from rigth to left
    
    int camera_FPM;    //Frames per minutes
    unsigned long camera_moving_time; //camera moving time from side to side minutes

    int projectionFrameRate; //frame per second 
    
    char calibrated;
};

extern struct configuration conf;


struct runtime_database{
    char isStarted; //1 when time lapse is started, otherwise 0
    int photoCount;
};

extern struct configuration conf;
extern struct runtime_database db;

void initializeConfiguration();

void saveConfig();
void loadConfig();

#endif


