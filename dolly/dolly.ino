#include "menu.h"
#include "motor.h"
#include "configuration.h"
#include "timelapse2.h"

#include <LiquidCrystal.h>
#include <EEPROM.h>
// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  initializeConfiguration();
  //initialize menu,lcd and buttons
  initializeMenu();
  initializeMotor();
  
}

void loop() {

  runMenu();
  runMotor();
  run_timelapse();
}

