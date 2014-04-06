#include"timelapse2.h"
#include"motor.h"
#include "configuration.h"
#include "Arduino.h"

enum timelapse_tasks {TIMELAPSE_STARTED, TIMELAPSE_IDLE};
enum timelapse_tasks timelapse_task = TIMELAPSE_IDLE;
int timelapse_taskhelper=0;

unsigned long time_between_two_step;
unsigned long steps_per_photo;
unsigned long photo_count;


void shoot()
{
   db.photoCount++; 
}

void run_timelapse()
{
  static unsigned long last = millis();
  unsigned long  currtime = millis();
  unsigned long  diff = currtime-last;
  char shootAndMove = 0;
  
  if(diff>=time_between_two_step)
  {
    last=currtime - (diff-time_between_two_step);
    shootAndMove=1;
  }
  
  
  if(timelapse_task==TIMELAPSE_STARTED)
  {
          
    db.isStarted=1;
    switch(timelapse_taskhelper)
    {
      case 0:
        if(isMotorIdle())
        {
          if(conf.fromLeftToRight)
          {
             move_motor(0xffffffff, 0);     
          }
          else
          {
            move_motor(0xffffffff, 1);
          }
          timelapse_taskhelper++;
        }
        break;
      case 1:
        if(isMotorIdle())
        {

          if(shootAndMove)
          {
            
            shoot();
            
            if(conf.fromLeftToRight)
            {
               move_motor(steps_per_photo, 1);     
            }
            else
            {
              move_motor(steps_per_photo, 0);
            }
            photo_count--;
            if(photo_count==0)
            {
              timelapse_task = TIMELAPSE_IDLE;
            }
          }
        }
     
        break;
    }
  }
  else
  {
    db.isStarted=0;
  }
}

void start_timelapse()
{
  photo_count = conf.camera_moving_time*conf.camera_FPM;
  steps_per_photo = ((conf.stepFromSideToSide*conf.distance)/photo_count)/100;
  
  timelapse_taskhelper=0;
  timelapse_task = TIMELAPSE_STARTED;
  
  time_between_two_step = (conf.camera_moving_time*60*1000)/photo_count;
  
  db.photoCount=0;
}
void stop_timelapse()
{
  timelapse_task = TIMELAPSE_IDLE;
}
