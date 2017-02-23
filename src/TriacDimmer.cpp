#include "TriacDimmer.h"
#include <Arduino.h>
#include <assert.h>

volatile uint16_t TriacDimmer::detail::pulse_length;
volatile uint16_t TriacDimmer::detail::period = 16667;
volatile uint16_t TriacDimmer::detail::ch_A_up;
volatile uint16_t TriacDimmer::detail::ch_A_dn;
volatile uint16_t TriacDimmer::detail::ch_B_up;
volatile uint16_t TriacDimmer::detail::ch_B_dn;

void TriacDimmer::begin(uint16_t pulse_length){
	TriacDimmer::detail::pulse_length = pulse_length;

	TCCR1A = 0;
	TCCR1B = _BV(ICNC1)  //input capture noise cancel
			| _BV(ICES1) //positive edge
			| _BV(CS11); // /8 prescaler

	pinMode(8, INPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);

	TIFR1 = _BV(ICF1); //clear IC interrupt flag
	TIMSK1 = _BV(ICIE1); //enable input capture interrupt
}

void TriacDimmer::end(){
	TIMSK1 = 0; //disable the interrupts first!
	TIFR1 = 0xFF; //clear all flags
	TCCR1A = 0; //clear to reset state
	TCCR1B = 0;
}

void TriacDimmer::setBrightness(uint8_t pin, float value){
	assert(pin == 9 || pin == 10);

	if (pin & 0x01 == 0x01){ // if (pin == 9){
		TriacDimmer::detail::setChannelA(1 - value);
	} else { // if (pin == 10){
		TriacDimmer::detail::setChannelB(1 - value);
	}
}

float TriacDimmer::getCurrentBrightness(uint8_t pin){
	assert(pin == 9 || pin == 10);
	
	if (pin & 0x01 == 0x01){ // if (pin == 9){
		1 - TriacDimmer::detail::getChannelA();
	} else { // if (pin == 10){
		1 - TriacDimmer::detail::getChannelB();
	}
}

void TriacDimmer::detail::setChannelA(float value){
	TriacDimmer::detail::ch_A_up = TriacDimmer::detail::period * value;
	TriacDimmer::detail::ch_A_dn = TriacDimmer::detail::ch_A_up + TriacDimmer::detail::pulse_length;
}

void TriacDimmer::detail::setChannelB(float value){
	TriacDimmer::detail::ch_B_up = TriacDimmer::detail::period * value;
	TriacDimmer::detail::ch_B_dn = TriacDimmer::detail::ch_B_up + TriacDimmer::detail::pulse_length;
}
float TriacDimmer::detail::getChannelA(){
	return (float)TriacDimmer::detail::ch_A_up / TriacDimmer::detail::period;
}
float TriacDimmer::detail::getChannelB(){
	return (float)TriacDimmer::detail::ch_B_up / TriacDimmer::detail::period;
}


ISR(TIMER1_CAPT_vect){
	TIMSK1 &=~ (_BV(OCIE1A) | _BV(OCIE1B)); //clear interrupts, in case they haven't run yet
	TCCR1A &=~ (_BV(COM1A1) | _BV(COM1B1));
	TCCR1C = _BV(FOC1A) | _BV(FOC1B); //ensure outputs are properly cleared

	OCR1A = ICR1 + TriacDimmer::detail::ch_A_up;
	OCR1B = ICR1 + TriacDimmer::detail::ch_B_up;

	TCCR1A |= _BV(COM1A0) | _BV(COM1A1) | _BV(COM1B0) | _BV(COM1B1); //set OC1x on compare match
	TIFR1 = _BV(OCF1A) | _BV(OCF1B); //clear compare match flags
	TIMSK1 |= _BV(OCIE1A) | _BV(OCIE1B); //enable input capture and compare match interrupts


	static uint16_t last_icr = 0;
	TriacDimmer::detail::period = ICR1 - last_icr;
	last_icr = ICR1;
}


ISR(TIMER1_COMPA_vect){
	TIMSK1 &=~ _BV(OCIE1A); //disable match interrupt
	TCCR1A |= _BV(COM1A1); //clear OC1x on compare match

	OCR1A = ICR1 + TriacDimmer::detail::ch_A_dn;

	if((signed)(TCNT1 - OCR1A) >= 0){
		TCCR1C = _BV(FOC1A); //interrupt ran late, trigger match manually
	}
}


ISR(TIMER1_COMPB_vect){
	TIMSK1 &=~ _BV(OCIE1B); //disable match interrupt
	TCCR1A |= _BV(COM1B1); //clear OC1x on compare match

	OCR1B = ICR1 + TriacDimmer::detail::ch_B_dn;

	if((signed)(TCNT1 - OCR1B) >= 0){ 
		TCCR1C = _BV(FOC1B); //interrupt ran late, trigger match manually
	}
}

