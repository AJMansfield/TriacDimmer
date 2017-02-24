/*
  Basic Example.
  
  This sketch alternately fades two lights in and out, using a
  digital mains AC dimmer switch board.

  This sketch is meant to be used with one of these boards, or 
  something similar:
      http://a.co/iy2r6Gq
      http://a.co/30mKbq8

  While this sketch is meant to be used with a dual-channel dimmer,
  it can also control just a single channel (just leave the second
  channel disconnected).
  
  Note that the circuit here uses high-voltage mains AC power - make
  sure you understand the risks and take appropriate precautions.

  The circuit:
  * Pin 8 is connected to the dimmer 'sync' pin.
  * Pin 9 is connected to the dimmer 'ch 1' pin.
  * Pin 10 is connected to the dimmer 'ch 2' pin.
  * The dimmer power input is connected to mains AC power.
  * The dimmer channel 1 and channel 2 outputs are connected 
      to lightbulbs or other devices.

  Created 2017-02-23
  By Anson Mansfield

*/

#include <TriacDimmer.h>

// arduino pins
unsigned char sync = 8; //sync pin
unsigned char channel_1 = 9; // channel 1 pin
unsigned char channel_2 = 10; // channel 2 pin

void setup() {
  //initialize the dimmer library. We want 
  TriacDimmer::begin();
}

void loop() {
  //gradually increase brightness over time
  for(float brightness = 0.01; brightness < 0.99; brightness += 0.01){
  
    //set channel 1 to the brightness value:
    TriacDimmer::setBrightness(channel_1, brightness); 

    //invert brightness for channel 2:
    TriacDimmer::setBrightness(channel_2, 1 - brightness); 
    
    delay(20);
  }
  
  //and back down - decrease brightness over time
  for(float brightness = 0.99; brightness > 0.01; brightness -= 0.01){
      
    //set channel 1 to the brightness value:
    TriacDimmer::setBrightness(channel_1, brightness);

    //invert brightness for channel 2:
    TriacDimmer::setBrightness(channel_2, brightness);
    
    delay(20);
  }

}
