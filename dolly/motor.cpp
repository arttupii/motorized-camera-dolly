#include "motor.h"
#include "Arduino.h"
#include "conf.h"
#include "configuration.h"


unsigned long  steps;
unsigned int dir;
unsigned long  stepCount;

char disable_interrupt = 0;

enum motor_tasks{GOHOME, CALIBRATE, MOVE, IDLE};
enum motor_tasks motor_task = IDLE;
int motor_task_helper = 0;

/*
#define STEP_PIN    5
#define DIR_PIN    7
#define ENABLE_PIN  8
*/

void moveMotor();
void run_motor_tasks();

void initializeMotor()
{
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    
    #if INVERT_ENABLE_PIN==0
    digitalWrite(ENABLE_PIN, LOW);
    #else
    digitalWrite(ENABLE_PIN, HIGH);
    #endif
    pinMode(RIGTH_LIMIT_SWITCH_PIN, INPUT);
    pinMode(LEFT_LIMIT_SWITCH_PIN, INPUT);
    digitalWrite(RIGTH_LIMIT_SWITCH_PIN,HIGH);
    digitalWrite(LEFT_LIMIT_SWITCH_PIN,HIGH);
    
    	TIMSK2 &= ~(1<<TOIE2);
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
	TCCR2B &= ~(1<<WGM22);
	ASSR &= ~(1<<AS2);
	TIMSK2 &= ~(1<<OCIE2A);
	
	TCCR2B |= (1<<CS22);
	TCCR2B &= ~((1<<CS21) | (1<<CS20));
	TIMSK2 |= (1<<TOIE2);
   // attachInterrupt(TIMER2, moveMotor, CHANGE);
 
}
char disable_motor_interrupt()
{
  disable_interrupt = 1;
  return 0;
}
char enable_motor_interrupt()
{
  disable_interrupt = 0;
  return 0;
}
ISR(TIMER2_OVF_vect) {
      TCNT2 = 233;
  if(disable_interrupt==0)
  {

  moveMotor();
  run_motor_tasks();
  }
}

char leftLimitSwitch()
{
    #if INVERT_LIMIT_SWITCH == 1
    return !digitalRead(LEFT_LIMIT_SWITCH_PIN);
    #else
    return digitalRead(LEFT_LIMIT_SWITCH_PIN);
    #endif    
}

char rigthLimitSwitch()
{
    #if INVERT_LIMIT_SWITCH == 1
    return !digitalRead(RIGTH_LIMIT_SWITCH_PIN);
    #else
    return digitalRead(RIGTH_LIMIT_SWITCH_PIN);
    #endif    
}

char one_step_motor(char _dir)
{
  static char i=0;
  i++;
  if(leftLimitSwitch() && !_dir)
  {
      return 2;
  }

  if(rigthLimitSwitch() && _dir)
  {
      return 1;
  }

  
  #if INVERT_DIR_PIN==0
  digitalWrite(DIR_PIN, _dir?HIGH:LOW);
  #else
  digitalWrite(DIR_PIN, _dir?LOW:HIGH);
  #endif
  digitalWrite(STEP_PIN, i&0x01?HIGH:LOW);
  digitalWrite(STEP_PIN, i&0x02?HIGH:LOW);
  
  return 0;
}

void motor_enable()
{
  #if INVERT_ENABLE_PIN==0
  digitalWrite(ENABLE_PIN, HIGH);
  #else
  digitalWrite(ENABLE_PIN, LOW);
  #endif   
}

void motor_disable()
{
    #if INVERT_ENABLE_PIN==0
    digitalWrite(ENABLE_PIN, LOW);
    #else
    digitalWrite(ENABLE_PIN, HIGH);
    #endif 
}
char manualStepMotor(char _dir)
{
  motor_task=IDLE; 
  return one_step_motor(_dir); 
}

void moveMotor()
{
  if(motor_task!=IDLE)
  {
    motor_enable();
  }
  
  if(steps>0)
  {
    char ret = one_step_motor(dir); 
    if(ret==1)
    {
      steps=0;
    }
    else if(ret==2)
    {
      steps=0;
    }
    else
    {
      steps--;
      stepCount++;
    }
  }
  
  if(motor_task==IDLE)
  {
    motor_disable();
  }
}




void runMotor()
{

}

int isMotorCalibrated()
{
    return conf.calibrated = 1;
}

char move_motor(unsigned long  _steps, int _direction)
{
  if(motor_task==IDLE)
  {
    motor_task_helper=0;
    motor_task=MOVE;
    dir=_direction;
    steps=_steps; 
    return 1; 
  }
  else
  {
    return 0;
  }
}

void run_motor_tasks()
{
  if(motor_task==GOHOME)
  {
    switch(motor_task_helper)
    {
      case 0:
        dir=1;
        steps=5000;
        motor_task_helper++;
        break;
      case 1:
        if(steps==0)
        {
           dir=0;
           steps=0xffffffff;
           motor_task_helper++;
        }
        break;
      default:
        if(steps==0)
        {
           motor_task=IDLE;
        }
        break;
    }
  }
  if(motor_task==CALIBRATE)
  {
    switch(motor_task_helper)
    {
       case 0:
        dir=1;
        steps=5000;
        motor_task_helper++;
        break;
      case 1:
        if(steps==0)
        {
           dir=0;
           steps=0xffffffff;
           motor_task_helper++;
        }
        break;
      case 2:
        if(steps==0)
        {
           dir=1;
           stepCount=0;
           steps=0xffffffff;
           motor_task_helper++;
        }
        break;     
      case 3:
        if(steps==0)
        {
           dir=0;
           stepCount=0;
           steps=0xffffffff;
           motor_task_helper++;
        }
        break; 
      default:
        if(steps==0)
        {
          conf.stepFromSideToSide = stepCount;
          conf.calibrated = 1;
          motor_task=IDLE;
        }
        break;
    }
  }
  if(motor_task==MOVE)
  {
    if(steps==0)
    {
      motor_task=IDLE;
    }
  }
}

char isMotorIdle()
{
  if(motor_task==IDLE) return 1;
  return 0;
}

void calibrateMotor()
{
  if(motor_task!=IDLE) return;
  motor_task_helper=0;
  motor_task=CALIBRATE;  
}

void goHomeMotor()
{
  if(motor_task!=IDLE) return;
  motor_task_helper=0;
  motor_task=GOHOME;
}
