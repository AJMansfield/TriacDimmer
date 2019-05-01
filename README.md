# TriacDimmer
An arduino library for controlling a triac dimmer.

This library was designed to perform phase-control dimming control on a triac dimming circuit,
  leveraging the ATmega328p's built-in timer peripheral to perform all time-critical functionality directly in hardware,
  without the need to spend CPU time on an expensive control loop.

Note that this library is intended to control mains AC power.
Make sure you understand the risks and take appropriate precautions before working with mains AC.

The phase offsets are calculated based on the _measured_ mains frequency,
  so this code will work regardless of 50/60Hz or any other frequency.
This includes correcting for any inaccuracies in the arduino's oscillator or the mains frequency.

This library was developed specifically to control a Krida 2 CH Dimmer
  ([amazon](https://www.amazon.com/Dimmer-Module-Controller-Arduino-Raspberry/dp/B06Y1GVG26),
  [alibaba](https://mdwdz.en.alibaba.com/product/60670737878-804998378/2CH_AC_LED_Light_Dimmer_Module_Controller_Board.html),
  [inmojo](http://www.inmojo.com/store/krida-electronics/item/2-channel-ac-led-bulb-dimmer-module-v2/)),
  but it should work with other phase-control dimming circuits that output a positive edge on their sync signal.

See [the example](examples/basic_example/basic_example.ino) for an example of how to use the library.
The library methods themselves are documented in [the library header](src/TriacDimmer.h).

This library **requires** the use of pins 8, 9, and 10.
Pin 8 _must_ be used as the sync input, and pins 9 and 10 _must_ be used as the channel outputs.
This library _will not work_ on any other pins, period.

![fritzing diagram](fritzing.png)