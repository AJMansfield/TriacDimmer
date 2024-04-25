# TriacDimmer
The _high-performance_ arduino triac dimming library.

This library was designed to perform phase-control dimming control on a triac dimming circuit,
  leveraging the ATmega328p's built-in timer peripheral to perform all time-critical functionality directly in hardware,
  without the need to spend CPU time on an expensive control loop.

Note that this library is intended to control mains AC power.
Make sure you understand the risks and take appropriate precautions before working with mains AC.

The phase offsets are calculated based on the _measured_ mains frequency,
  so this code will work regardless of 50/60Hz or any other frequency.
This includes correcting for any inaccuracies in the arduino's oscillator or the mains frequency.

This library was developed specifically for the Krida 2 CH Dimmer
  ([amazon](https://www.amazon.com/Dimmer-Module-Controller-Arduino-Raspberry/dp/B06Y1GVG26),
  [alibaba](https://mdwdz.en.alibaba.com/product/60670737878-804998378/2CH_AC_LED_Light_Dimmer_Module_Controller_Board.html),
  [inmojo](http://www.inmojo.com/store/krida-electronics/item/2-channel-ac-led-bulb-dimmer-module-v2/)),
  and has been tested to work with the RobotDyn AC Dimmer
  ([robotdyn](https://robotdyn.com/ac-light-dimmer-module-1-channel-3-3v-5v-logic-ac-50-60hz-220v-110v.html)),
  and should work fine with other phase-control dimming circuits that output a positive edge on their sync signal.

See [the example](examples/basic_example/basic_example.ino) for an example of how to use the library.
The library methods themselves are documented in [the library header](src/TriacDimmer.h).

This library **requires** the use of certain pins.
Pin 8 _must_ be used as the sync input, and pins 9 and 10 are the only pins that can be used as channel outputs.
This library _will not work_ on any other pins, period.

![fritzing diagram](fritzing.png)

## Flickering, and How to Fix It

If you experience issues with flickering, there are a handful of parameters you can pass to `begin` that can be adjusted depending on what sort of flickering you have.
By default, with no arguments the library uses these values as defaults:

```cpp
TriacDimmer::begin(pulse_length = 20, min_trigger = 2000, on_thresh = 2, off_thresh = 0.01)
```

First off, if you experience flickering regardless of the brightness value you set, increase `pulse_length` from the default `20` to a larger value like `50` or `100` until `TriacDimmer::setBrightness(pin, 0.5);` results in a stable glow.

Once it's stable at 0.5, set the brightness 1.0 (`TriacDimmer::setBrightness(pin, 1.0);`) and check for flickering.
There shouldn't be any, but if there is you can increase `min_trigger` from the default `2000` to perhaps `3000` or `4000` until the flickering stops.
If you're still experiencing flickering no matter how large `min_trigger` is, you can also try setting `on_thresh` to below the highest brightness level that causes flickering. This shouldn't normally be necessary though, as adjusting `min_trigger` should normally be enough.

The last step is to figure out the lowest brightness value can sustain without flickering. 
By default the library is set to cut off completely for brightness values smaller than `0.01`, but if you still see flickering at `0.015` or `0.02` you can try setting `off_thresh` to a value that's larger than that.

If you've tried all of these steps and you still get flickering, consider opening an issue.
Make sure to include as much information about your setup as you can, including the specific dimmer board you're using.
Also, if you have access to an oscilliscope, screenshots are always helpful in trying to diagnose flickering.


## Theory of Operation

For those having trouble with this library, understanding the underlying theory of operation may be helpful.
For others, if you're looking to get into programming microcontrollers at the register level, this library might actually be a good example codebase to study.

Either way, I'd highly recommend looking through the [Atmega328p datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf), specifically chapter 15: 16-bit Timer/Counter1 with PWM. This library uses most of the features of this pherhipheral, and that datasheet is the primary source of truth for how it works.

The library uses Timer/Counter 1 in free-running aka "Normal" mode, where the counter is allowed to count up continuously and overflow from 0xFFFF back to 0x0000. It's set to run from the system clock, with a divide-by-8 prescaler that ensures that the counter's 16 bits are enough to represent the period between zero-crossings and avoid aliasing between overflows.

With the most common 328p Arduino boards with external 16 MHz crystals, (e.g. an Uno, Micro, Nano, Leonardo), each count represents _approximately_ 0.5us, and the timer overflows back to 0 approximately every 33ms. On other boards that use the 328p's own internal 8 MHz oscillator, each count represents a nominal 1us, for an overflow about every 65ms -- but operating under conditions that put it closer to the edges of the ±14% tolerance, each count could represent anything from 0.86us to 1.14us, and the overflows could be anywhere from 57ms to 76ms.
Importantly, it doesn't actually matter how long the timer counts are -- the "timer count" is the fundamental unit of time being measured by this timer for the purposes of this library, and there's no actual need to relate that unit to more conventional units of time like milliseconds or microseconds, as long as the events being measured are more frequent than the overflow.

For comparison, on 50Hz power there are 100 zero-crossings per second, so the time between those events is 10ms; whereas on 60Hz power there are 120 so the time between those events is 8.3ms. Either way, the time between events is enough smaller than the overflow period to avoid aliasing.

The core functionality of this library revolves around the interplay between the counter's Input Capture unit and the Output Compare units when used in free-running normal mode.

The Input Capture unit is a piece of hardware that, when triggered by an external pin change, instantly copies the current value of the counter to its `ICR1` register.
The Output Compare units have a similar function, but in reverse; they continuously wait and check for the counter to reach the value programmed into their `OCR1` register (`OCR1A` for unit A, `OCR1B1` for unit B), and then instantly change the value of the associated pin.

The value captured by the input capture unit acts as something of a timestamp; it can be compared to work out the time between events by subtracting a previous timestamp, or used to calculate a value to set the output compare register to to generate an output after a precise duration.

Both of those concepts -- comparing capture timestamps and computing output timestamps -- are used by this library. Probably the easiest-to-follow examples of this are in the [`TIMER1_CAPT` interrupt service routine](https://github.com/AJMansfield/TriacDimmer/blob/master/src/TriacDimmer.cpp#L147-L173).

The comparison between captured timestamps happens on [lines 163-164](https://github.com/AJMansfield/TriacDimmer/blob/master/src/TriacDimmer.cpp#L163-L164):
```
	TriacDimmer::detail::period = ICR1 - last_icr;
	last_icr = ICR1;
```

`TriacDimmer::detail::period` is a variable that the library uses to store the measured half-wave period, in timer-count units, and communicate that value from the ISR context to what I'll call the "userland" half of the library.

And calculating and setting the output timestamps happens a few lines earlier, [lines 152-153](https://github.com/AJMansfield/TriacDimmer/blob/master/src/TriacDimmer.cpp#L152-L153):
```
	OCR1A = ICR1 + TriacDimmer::detail::ch_A_up;
	OCR1B = ICR1 + TriacDimmer::detail::ch_B_up;
```

`TriacDimmer::detail::ch_A_up` and `ch_B_up` are variables that the library uses to store how long the pause between the zero crossing and start of trigger pulse should be, in timer-count units, and communicate that value from the "userland" half of the library to the ISR.
(The library also has `TriacDimmer::detail:ch_A_dn` and `ch_B_dn` that are used in the output compare service routines, to set up the timer to end the trigger pulse at the appropriate time once the pulse has started; this works almost the same but with some further complications around needing to buffer this value to avoid a data race.)

The "userland" end of that communication can be seen in [`TriacDimmer::detail::setChannelA`](https://github.com/AJMansfield/TriacDimmer/blob/master/src/TriacDimmer.cpp#L91-L103) and it's siblings. The code there might not be super readable, due to the `ATOMIC_BLOCK` parts needed to avoid a data race and the logic for ensuring the pulses always end correctly, but if you were to strip out all of that, fundamentally what it's doing is this:

```
void TriacDimmer::detail::setChannelA(float value){
	TriacDimmer::detail::ch_A_up = TriacDimmer::detail::period * value;
}
```

This operation is the key to the library's ability to compensate for different mains and clock frequencies. And the use of the _measured_ period ensures that the pulse will be proportional to the actual mains frequency, however many timer-counts its period happens to be. And because both the period and pulse offset are measured in timer-counts, variation in the absolute size of each timer-count cancels out.

This operation is also the reason for the split between the ISR and "userland" parts of the library. One thing to note, is that the 328p does not have hardware floating point, so an arithmetic operation like this internally involves making a subroutine call into a software floating point routine. This wouldn't necessarily be _too_ expensive to do in an ISR, but in general it's good practice to do as little work as possible in ISRs, and in specific the library's margins for how close to the start/end of a cycle and how short of a pulse it can generate are directly affected by how long its ISRs take to run. So this arithmetic is done in the "userland" portion.

Because of this, though, this recalibration doesn't happen automatically the way the interrupt-handling and pulse-scheduling parts do; in order to get the library to re-calibrate against any drift in the system clock frequency or mains frequency, these "userland" routines need to be called again, and in theory called regularly in order to _keep_ recalibrating.

In the examples, the main code loops continuously, calling `setBrightness()` regularly (about every 20ms in basic_example.ino, and continuously as it's polling the analog input in potentiometer.ino). Though it doesn't necessarily need to be called continuously like that, it does need to be called at some point _after_ the system has managed to capture and measure a pair of actual zero-crossing events -- i.e. a couple dozen milliseconds after the initial `TriacDimmer::begin()` call.
