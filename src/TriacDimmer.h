/**
 *	@file	TriacDimmer.h
 *	@brief	Contains header information for the TriacDimmer library.
 *	@author	Anson Mansfield
 *	@date 	2017-02-23
 */

#ifndef TriacDimmer_h
#define TriacDimmer_h

#include <Arduino.h>
#include <avr/pgmspace.h>

/**
 *	@brief	Contains all the library functions.
 *	This namespace contains the library's public API suitable for general use. 
 *	Implementation details and more technical API functions are in TriacDimmer::detail.
 */
namespace TriacDimmer {

	/**
	 *	@brief	Initializes the library.
	 *	@param	pulse_length	How long the trigger pulses should be, in half-microseconds.
	 *	This method initializes the library, setting up the timer and enabling the corresponding interrupts.
	 */
	void begin(uint16_t pulse_length = 20);

	/**
	 *	@brief	Stops the library
	 *	This method stops the library, disabling the interrupts and resetting the timer configuration.
	 */
	void end();

	// /**
	//  *	@brief	Sets the current brightness.
	//  *	@param	pin		The pin controlling the desired channel. Only pins 9 and 10 are supported.
	//  *	@param	value	The desired brightness, from 0.0 to 1.0.
	//  *	This method sets the brightness for the channel controlled by the indicated pin.
	//  */
	// void setBrightness(uint8_t pin, float value);

	// /**
	//  *	@brief	Gets the currently-set brightness.
	//  *	@param	pin		The pin controlling the desired channel. Only pins 9 and 10 are supported.
	//  *	This method retrieves the brightness for the channel controlled by the indicated pin.
	//  */	
	// float getCurrentBrightness(uint8_t pin);

	namespace detail {
		// void setChannelA(float value);
		// void setChannelB(float value);
		// float getChannelA();
		// float getChannelB();


		extern volatile uint16_t pulse_length;
		extern volatile uint16_t ch_A_up;
		extern volatile uint16_t ch_A_dn;
		extern volatile uint16_t ch_B_up;
		extern volatile uint16_t ch_B_dn;
		extern volatile uint16_t period;

	}
};

/* @breif This interrupt sets the output pulses to start at the appropriate time.
 * 
 * This interrupt sets the output pulses to begin at the appropriate time, sets up the
 * interrupts that will configure the pulse end, and computes the time since the last pulse.
 */
ISR(TIMER1_CAPT_vect);

/* @breif This interrupt sets the output A pulse to end at the appropriate time.
 * 
 * This interrupt sets the output A pulse to end at the appropriate time, and manually triggers the
 * end in case the interrupt ran late.
 */
ISR(TIMER1_COMPA_vect);

/* @breif This interrupt sets the output B pulse to end at the appropriate time.
 * 
 * This interrupt sets the output B pulse to end at the appropriate time, and manually triggers the
 * end in case the interrupt ran late.
 */
ISR(TIMER1_COMPB_vect);

#endif
