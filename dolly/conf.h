
/*
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
#define LCD_PINS 12, 11, 5, 4, 3, 2

#define LCD_LED_PIN 10

#define LCD_LED_TIMER_OFF 10000

// set up the LCD's number of columns and rows: 
#define LCD_COLUMS 16
#define LCS_ROWS 2


//Key pins
#define LCD_KEY_SELECT A2
#define LCD_KEY_UP     A3
#define LCD_KEY_DOWN   A1
#define LCD_KEY_BACK   A4
#define LCD_KEY_HIGH_ACTIVE 0

//Step motor pins
#define STEP_PIN    8
#define DIR_PIN    7
#define ENABLE_PIN  6

#define INVERT_STEP_PIN    0
#define INVERT_DIR_PIN     0
#define INVERT_ENABLE_PIN  0


#define STEP_TIME_US 10000  

//LIMIT SWITCHES
#define LEFT_LIMIT_SWITCH_PIN    A5
#define RIGTH_LIMIT_SWITCH_PIN   13
#define INVERT_LIMIT_SWITCH 1




/*

|-----||                   --------                      ||
|Step ||End                |Camera|                   End||
|motor||-------belt&rail---|      |-----belt&rail--------||
|-----||switch             |      |                switch||
      ||                   --------                      ||
      |   <--------     Camera moving         --------->
      |
 ---------   
 |Control|
 | Board |
 ---------
     |
 ---------   
 |Battery|
 |       |
 ---------
 */
