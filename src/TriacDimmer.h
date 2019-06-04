/**
 *	@file	TriacDimmer.h
 *	@brief	Contains header information for the TriacDimmer library.
 *	@author	Anson Mansfield
 *	@date 	2017-02-23
 */

#ifndef TriacDimmer_h
#define TriacDimmer_h

#include <Arduino.h>


/**
 *	@brief	Contains all the library functions.
 *	This namespace contains the library's public API suitable for general use. 
 *	Implementation details and more technical API functions are in TriacDimmer::detail.
 */
namespace TriacDimmer {

	/**
	 *	@brief	Initializes the library.
	 *	@param	pulse_length	How long the trigger pulses should be, in half-microseconds.
	 *	@param	min_trigger	Minimum offset from beginning of phase to end of trigger pulse to ensure triac latches.
	 *	@param	on_thresh	Brightness threshold where the light will be turned on completely. >1 means disabled.
	 *	@param	off_thresh	Brightness threshold where the light will be turned off completely. <0 means disabled.
	 *	This method initializes the library, setting up the timer and enabling the corresponding interrupts.
	 */
	void begin(uint16_t pulse_length = 20, uint16_t min_trigger = 2000, float on_thresh = 2.0, float off_thresh = 0.01);

	/**
	 *	@brief	Stops the library
	 *	This method stops the library, disabling the interrupts and resetting the timer configuration.
	 */
	void end();

	/**
	 *	@brief	Sets the current brightness.
	 *	@param	pin		The pin controlling the desired channel. Only pins 9 and 10 are supported.
	 *	@param	value	The desired brightness, from 0.0 to 1.0.
	 *	This method enables library control of the given output pin and sets the brightness.
	 */
	void setBrightness(uint8_t pin, float value);

	/**
	 *  @brief	Disables and detaches the pin from the library.
	 *	@param	pin		The pin to relenquish control of.
	 *	This method disables control of the given pin and stops the library from controlling it.
	 *	Note that both pins start disabled; in order to enable them you can call `setBrightness`.
	 */
	void disable(uint8_t pin);

	/**
	 *	@brief	Gets the currently-set brightness.
	 *	@param	pin		The pin controlling the desired channel. Only pins 9 and 10 are supported.
	 *	This method retrieves the brightness for the channel controlled by the indicated pin.
	 */
	float getCurrentBrightness(uint8_t pin);


	/**
	 *	@brief	Contains lower-level API functions.
	 *	This namespace contains lower level API functions that can be used to more directly control the
	 *	pulse waveform.
	 */
	namespace detail {
		/**
		 *	@brief	Sets channel A phase angle.
		 *	@param	value	The phase angle, 0.0 fires immediately, 1.0 fires one period later.
		 *	This method directly sets the phase angle used to control brightness on channel A (pin 9).
		 *	Note that this method does not enable the pin if it is disabled.
		 */
		void setChannelA(float value);

		/**
		 *	@brief	Sets channel B phase angle.
		 *	@param	value	The phase angle, 0.0 fires immediately, 1.0 fires one period later.
		 *	This method directly sets the phase angle used to control brightness on channel B (pin 10).
		 *	Note that this method does not enable the pin if it is disabled.
		 */
		void setChannelB(float value);

		/**
		 *	@brief	Gets channel A phase angle.
		 *	This method directly retrieves the phase angle used to control brightness on channel A (pin 9).
		 */
		float getChannelA();

		/**
		 *	@brief	Gets channel B phase angle.
		 *	This method directly retrieves the phase angle used to control brightness on channel B (pin 10).
		 */
		float getChannelB();

		/**
		 *	@brief	Disables channel A.
		 *	This method disables timer control of channel A (pin 9). Set `ch_A_en` to re-enable.
		 */
		void disableChannelA();

		/**
		 *	@brief	Disables channel B.
		 *	This method disables timer control of channel B (pin 10). Set `ch_A_en` to re-enable.
		 */
		void disableChannelB();


		/**
		 *	@brief	Stores the configured pulse length.
		 */
		extern uint16_t pulse_length;

		/**
		 *	@brief	Stores the configured minimum trigger time.
		 */
		extern uint16_t min_trigger;

		/**
		 *	@brief	Stores the configured minimum trigger time.
		 */
		extern float on_thresh;

		/**
		 *	@brief	Stores the configured minimum trigger time.
		 */
		extern float off_thresh;

		/**
		 *	@brief	Stores whether channel A is enabled.
		 */
		extern volatile bool ch_A_en;

		/**
		 *	@brief	Stores the channel A positive edge delay
		 */
		extern volatile uint16_t ch_A_up;

		/**
		 *	@brief	Stores the channel A negative edge delay
		 */
		extern volatile uint16_t ch_A_dn;

		/**
		 *	@brief	Stores whether channel B is enabled.
		 */
		extern volatile bool ch_B_en;

		/**
		 *	@brief	Stores the channel B positive edge delay
		 */
		extern volatile uint16_t ch_B_up;

		/**
		 *	@brief	Stores the channel B negative edge delay
		 */
		extern volatile uint16_t ch_B_dn;

		/**
		 *	@brief	Stores the computed pulse period.
		 */
		extern volatile uint16_t period;

	}

	namespace util {

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
