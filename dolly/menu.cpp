#include "menu.h"
#include "motor.h"
#include "configuration.h"
#include "Arduino.h"
#include"timelapse2.h"

#include "conf.h"
#include <string.h>
#include <stdio.h>
#include <LiquidCrystal.h>



int endThis=0;

int mainMenu(int i);
int setupMenu(int state);
int infoScreen(int i);

int (*active_menu)(int index) = &infoScreen;
int (*notification_menu_tmp)(int index) = &infoScreen;

char buffer[17];

LiquidCrystal lcd(LCD_PINS);
 
void initializeMenu()
{
   lcd.begin(LCD_COLUMS, LCS_ROWS);  
   
   lcd.setCursor(0, 0);
   lcd.print("Time-lapse");
   lcd.setCursor(0, 1);
   lcd.print("photography");       
   
   pinMode(LCD_KEY_SELECT, INPUT);
   pinMode(LCD_KEY_UP, INPUT);
   pinMode(LCD_KEY_DOWN, INPUT);
   pinMode(LCD_KEY_BACK, INPUT);

   //digital pullup
   digitalWrite(LCD_KEY_SELECT, HIGH);
   digitalWrite(LCD_KEY_UP, HIGH);
   digitalWrite(LCD_KEY_DOWN, HIGH);
   digitalWrite(LCD_KEY_BACK, HIGH);


   pinMode(LCD_LED_PIN,OUTPUT);   
   digitalWrite(LCD_LED_PIN, HIGH);
}

void lcdPrint(int line, const char *text)
{
    static char lines[LCS_ROWS][LCD_COLUMS+1];

    if(strcmp(lines[line],text)!=0)
    {
        memset(lines[line], ' ', LCD_COLUMS);
        lines[line][LCD_COLUMS]=0;
        
        memcpy(lines[line], text, strlen(text)); 

        lcd.setCursor(0, line);
        lcd.print(lines[line]);
    }
}

int getKey()
{    
    int key = -1;
    
  #if LCD_KEY_HIGH_ACTIVE == 1
    if(digitalRead(LCD_KEY_UP)) key = LCD_KEY_UP;
    if(digitalRead(LCD_KEY_DOWN)) key = LCD_KEY_DOWN;
    if(digitalRead(LCD_KEY_SELECT)) key = LCD_KEY_SELECT;
    if(digitalRead(LCD_KEY_BACK)) key = LCD_KEY_BACK;
  #else
    if(!digitalRead(LCD_KEY_UP)) key = LCD_KEY_UP;
    if(!digitalRead(LCD_KEY_DOWN)) key = LCD_KEY_DOWN;
    if(!digitalRead(LCD_KEY_SELECT)) key = LCD_KEY_SELECT;
    if(!digitalRead(LCD_KEY_BACK)) key = LCD_KEY_BACK;
  #endif  
  

    
    
    static unsigned long time = millis();
    unsigned long ctime = millis();
    
    if( (ctime-time) > LCD_LED_TIMER_OFF)
    {
      digitalWrite(LCD_LED_PIN, LOW);
      active_menu = &infoScreen;
      time = millis();
    }
    
    if(key!=-1)
    {
      digitalWrite(LCD_LED_PIN, HIGH);
      time = millis();
    }
    
    static int last = key;
    
    if(key==last) return -1;
    
    last=key;
    
    return key;
}


void setNewActiveMenu(int (*menu)(int))
{
    active_menu = menu;
    menu(-1);
}


int notificationMenu(int i)
{
    switch(getKey())
    {
            case LCD_KEY_SELECT:
            case LCD_KEY_BACK:
                active_menu = notification_menu_tmp; break;
    };   
    return 0;
}

int notification(const char *line1, const char *line2)
{   
    notification_menu_tmp = active_menu;
    active_menu = &notificationMenu;
    lcdPrint(0, line1);
    lcdPrint(1, line2);
}


int infoScreen(int i)
{
    static char index = 0;
    
    if(isMotorCalibrated()==0)
    {
        lcdPrint(0, "Calibrate device");
        lcdPrint(1, ">>>>>>>>>>>>>>>>");
    }
    else    
    {
        switch(index)
        {
            case 0:
                lcdPrint(0, "Photos");
                sprintf(buffer,"%d/%d", db.photoCount, conf.camera_moving_time*conf.camera_FPM);
                lcdPrint(1, buffer);
                break;
            case 1:
	    {
		int totalTime = conf.camera_moving_time;
		int timeSpend = db.photoCount/conf.camera_FPM;
		int timeleft = totalTime-timeSpend;
		
		sprintf(buffer,"Time [%02d:%02d]", totalTime/60, totalTime%60);
                lcdPrint(0, buffer);
                sprintf(buffer,"%02d:%02d [%d %%]", timeleft/60, timeleft%60, (timeSpend*100)/totalTime);
                lcdPrint(1, buffer);
	    }
                break;
            case 2:
                {
                    lcdPrint(0, "ProjectionTime");
                    unsigned int t = (conf.camera_moving_time*conf.camera_FPM)/conf.projectionFrameRate;

                    sprintf(buffer,"%d:%d [%dFPS]", t/60,t%60, conf.projectionFrameRate);
                    lcdPrint(1, buffer);
                }
                break;
            default:
                break;
        }
    }

    if(index<0)index = 2;
    if(index>2)index = 0;

    switch(getKey())
    {
            case LCD_KEY_UP: index++; break;
            case LCD_KEY_DOWN: index--; break;
            case LCD_KEY_SELECT:
                setNewActiveMenu(&mainMenu); break;
	    case LCD_KEY_BACK:
	      break;
    };   
    return 0;
}

int waitUntilCalibrationHasDoneMenu(int i)
{
    lcdPrint(0, "Calibrating...");
    lcdPrint(1, "Please wait...");
    if(isMotorCalibrated())
    {
        setNewActiveMenu(&mainMenu);
    }
}

int calibrateMenu(int state)
{
    lcdPrint(0, "Calibrate-Menu");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    
    switch(ch)
    {
            case LCD_KEY_UP: index++; break;
            case LCD_KEY_DOWN: index--; break;
            case LCD_KEY_BACK:
                setNewActiveMenu(&setupMenu); break;
                return 0;
    };

    switch(index)
    {
        case 0: 
            lcdPrint(1, " Do it"); 
            if(ch==LCD_KEY_SELECT)
            {
                calibrateMotor();
                setNewActiveMenu(&waitUntilCalibrationHasDoneMenu);
            }
            break;
        case 1: 
            lcdPrint(1, " Exit"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenu); break;
            }
            break;
        default:
        {
            if(index<0)
            {
                index=1;
            }
            if(index>1)
	    {
		index=0;
	    }
        }
    };
    return 0;
}

int setupMenuDistance(int state)
{
    lcdPrint(0, "Setup-distance");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    char tmp[17];

    sprintf(tmp, "%u %%", conf.distance);

    lcdPrint(1, tmp);

    switch(ch)
    {
            case LCD_KEY_UP: conf.distance+=10; break;
            case LCD_KEY_DOWN: conf.distance-=10; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };

    if(conf.distance<0) conf.distance=100;
    if(conf.distance>100) conf.distance=0;
    

    return 0;
}

int setupMenuDirection(int state)
{
    lcdPrint(0, "Setup-direction");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    if(conf.fromLeftToRight==1)
    {
        lcdPrint(1,"FromLeftToRight");
    }
    else
    {
        lcdPrint(1,"FromRigthToLeft");
    }

    switch(ch)
    {
            case LCD_KEY_UP: conf.fromLeftToRight=0; break;
            case LCD_KEY_DOWN: conf.fromLeftToRight=1; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };
    return 0;
}

int setupMenuCameraFPM(int state)
{
    lcdPrint(0, "Setup-Camera FPM");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    char tmp[17];

    sprintf(tmp, "%u FPM", conf.camera_FPM);

    lcdPrint(1, tmp);

    switch(ch)
    {
            case LCD_KEY_UP: conf.camera_FPM+=1; break;
            case LCD_KEY_DOWN: conf.camera_FPM-=1; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };

    if(conf.camera_FPM<1) conf.camera_FPM=60;
    if(conf.camera_FPM>60) conf.camera_FPM=1;
    

    return 0;
}

int setupMenuCameraMovingTimeMinute(int state)
{
    lcdPrint(0, "Setup-Moving time");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    char tmp[17];

    sprintf(tmp, "%u min (%dh)", conf.camera_moving_time, conf.camera_moving_time/60);

    lcdPrint(1, tmp);

    switch(ch)
    {
            case LCD_KEY_UP: conf.camera_moving_time+=1; break;
            case LCD_KEY_DOWN: conf.camera_moving_time-=1; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };

    if(conf.camera_FPM<0) conf.camera_moving_time=0;

    return 0;
}

int setupMenuCameraMovingTimeHour(int state)
{
    lcdPrint(0, "Setup-Moving time");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    char tmp[17];

    sprintf(tmp, "%dh", conf.camera_moving_time/60);

    lcdPrint(1, tmp);

    switch(ch)
    {
            case LCD_KEY_UP: conf.camera_moving_time+=60; break;
            case LCD_KEY_DOWN: conf.camera_moving_time-=60; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };

    //Remove minutes
    conf.camera_moving_time = (conf.camera_moving_time/60)*60;

    if(conf.camera_FPM<0) conf.camera_moving_time=0;

    return 0;
}

int setupMenuProjectionFrameRate(int state)
{
    lcdPrint(0, "Setup-Projection");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    

    char tmp[17];

    sprintf(tmp, "%d FPS", conf.projectionFrameRate);

    lcdPrint(1, tmp);

    switch(ch)
    {
            case LCD_KEY_UP: conf.projectionFrameRate+=1; break;
            case LCD_KEY_DOWN: conf.projectionFrameRate-=1; break;
            case LCD_KEY_BACK:
            case LCD_KEY_SELECT:
                active_menu = &setupMenu;
            return 0;
    };

    if(conf.projectionFrameRate<1) conf.projectionFrameRate=60;
    if(conf.projectionFrameRate>60) conf.projectionFrameRate=1;

    return 0;
}

int setupMenu(int state)
{
    lcdPrint(0, "Setup-Menu");

    static char index = 0;
    if(state==-1) index = 0;

    int ch = getKey();    
    switch(ch)
    {
            case LCD_KEY_UP: index++; break;
            case LCD_KEY_DOWN: index--; break;
            case LCD_KEY_BACK:
                setNewActiveMenu(&mainMenu); break;
                return 0;
    };

    switch(index)
    {
        case 0: 
            lcdPrint(1, " Set distance"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuDistance);
            }
            break;
        case 1: 
            lcdPrint(1, " Set direction"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuDirection);
            }
            break;
        case 2: 
            lcdPrint(1, " Camera FPM"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuCameraFPM);
            }
            break;
        case 3: 
            lcdPrint(1, " Time (minute)"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuCameraMovingTimeMinute);
            }
            break;
        case 4: 
            lcdPrint(1, " Time (hour)"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuCameraMovingTimeHour);
            }
            break;

        case 5: 
            lcdPrint(1, " ProjectionFPS"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenuProjectionFrameRate);
            }
            break;
        case 6: 
            lcdPrint(1, " Calibrate"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&calibrateMenu); 
            }
            break;
        case 7:     
            lcdPrint(1, " Exit"); 
            if(ch==LCD_KEY_SELECT)
            {
               setNewActiveMenu(&mainMenu);
            }
            break;
        default:
        {
            if(index<0)
            {
                index=7;
            }
            if(index>7)
	    {
		index=0;
	    }
        }
    };
    return 0;
}

int startMotion5min()
{
  unsigned long time = conf.dolly_time;
  char buffer[17];
  while(1)
  {
    lcdPrint(0, "Dolly-Set time");
    int ch = getKey();    
    switch(ch)
    {
            case LCD_KEY_UP: time+=5; break;
            case LCD_KEY_DOWN: time-=5; break;
            case LCD_KEY_BACK: return 0;
            case LCD_KEY_SELECT:
              {
                lcdPrint(0, "- Dolly -");
              char dir = conf.fromLeftToRight?0:1;
              lcdPrint(1, "To home...");
              move_motor(0xffffffff,dir?0:1);
              while(isMotorIdle()==0);
              
              disable_motor_interrupt();
              motor_enable();
              unsigned long d = (time*1000*1000)/conf.stepFromSideToSide;
              
              
              lcdPrint(1, "Running...");
              unsigned long u = micros();
              
              while(1)
              {
                ch = getKey();
                if(ch==LCD_KEY_BACK) break;
                
                if(u+d<=micros())
                {
                  u=micros();
                  if(dir)
                    {if(manualStepMotor(1)==1) break;}
                  else
                    {if(manualStepMotor(0)==2) break;}
                }
              }
              
              motor_disable();
              enable_motor_interrupt();
              return 0;
              }
    };

    
    sprintf(buffer,"%d s", time);
    lcdPrint(1, buffer);
    
    if(time<10) time=60*5;
    if(time>60*5)time = 10;
  
    conf.dolly_time = time;    
  }
    return 0;  
}
int mainMenu(int i)
{
    lcdPrint(0, "MENU");
    static char index = 0;
    int ch = getKey();

    switch(ch)
    {
            case LCD_KEY_UP: index++; break;
            case LCD_KEY_DOWN: index--; break;
            case LCD_KEY_BACK:
                setNewActiveMenu(&infoScreen); break;
                return 0;
    };

    switch(index)
    {
        case 0: 
	    if(db.isStarted==0)
	      lcdPrint(1, " Start");
	    else
	      lcdPrint(1, " Stop");
	    
            if(ch==LCD_KEY_SELECT)
            {
                if(isMotorCalibrated()!=0)
                {
		  if(db.isStarted==0)
                    start_timelapse();
		    else
                    stop_timelapse();
                }
                else
                {   
                    notification("Motor has not", "been calibrated!");
                    return 0;
                }
            }
            break;
        case 1: 
	    if(db.isStarted==0)
	      lcdPrint(1, " Start as Dolly");
            if(ch==LCD_KEY_SELECT)
            {
                if(isMotorCalibrated()!=0)
                {
		  startMotion5min();
                }
                else
                {   
                    notification("Motor has not", "been calibrated!");
                    return 0;
                }
            }
            break;
        case 2: 
	  if(db.isStarted==0)
	  {
            lcdPrint(1, " Setup"); 
            if(ch==LCD_KEY_SELECT)
            {
                setNewActiveMenu(&setupMenu);
            }
            break;
	  }
	  else
	  {
	    lcdPrint(1, "-------");
	  }
        case 3: 
            lcdPrint(1, " Go Home"); 
            if(ch==LCD_KEY_SELECT)
            {
                goHomeMotor();
            }
            break;
        case 4:     
            lcdPrint(1, " SaveToEEPROM"); 
            if(ch==LCD_KEY_SELECT)
            {
               saveConfig();
               notification("Done","");
            }
            break;
        case 5:     
            lcdPrint(1, " Exit"); 
            if(ch==LCD_KEY_SELECT)
            {
               setNewActiveMenu(&infoScreen);
            }
            break;
        default:
        {
            if(index<0)
            {
                index=5;
            }
            if(index>5)
	    {
	      index=0;
	    }
        }
    };
    return 0;
}

void runMenu()
{
    active_menu(1);
}


