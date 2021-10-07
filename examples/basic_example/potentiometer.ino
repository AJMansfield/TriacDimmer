/*
*Code to control brightness via a potentiometer.  In this example the pot is plugged into A0
*Tested on a Robotdyn AC Dimmer
*Code and tutorial to use floating point values with the map function here: https://arduinogetstarted.com/tutorials/arduino-potentiometer
*/

#include <TriacDimmer.h>

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned char sync = 8; //sync pin
unsigned char channel_1 = 9; // channel 1 pin

void setup() {
  // initialize the dimmer library.
  TriacDimmer::begin();
}

void loop() {
  // read the input on analog pin A0:
  int analogValue = analogRead(A0);
  float brightness = floatMap(analogValue, 0, 1023, 0, 1);
  // set channel 1 to the brightness value:
  TriacDimmer::setBrightness(channel_1, brightness); 
}
