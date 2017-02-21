#ifndef Morse_h
#define Morse_h

#include <Arduino.h>
#include <avr/pgmspace.h>

namespace TriacDimmer {
	void begin();
	void setBrightness(uint8_t pin, float value);
	float getCurrentBrightness(uint8_t pin);
	void end();

	namespace detail {

		const uint16_t pulse_length = 20;
		volatile uint16_t ch_A_up;
		volatile uint16_t ch_A_dn;
		volatile uint16_t ch_B_up;
		volatile uint16_t ch_B_dn;

		const PROGMEM float phase_lut[] = 
		{ -0.            ,   9.25295293e-08,   2.52559560e-05,   9.66419104e-04,
		   6.17766236e-03,   1.89552895e-02,   4.10991260e-02,   7.40639247e-02,
		   1.15521833e-01,   1.64535545e-01,   2.20903015e-01,   2.80866796e-01,
		   3.43594021e-01,   4.10268098e-01,   4.77649597e-01,   5.43711948e-01,
		   6.07640486e-01,   6.68898386e-01,   7.26232887e-01,   7.78168688e-01,
		   8.24150458e-01,   8.64133390e-01,   8.98217770e-01,   9.26632476e-01,
		   9.49644260e-01,   9.67583008e-01,   9.80871218e-01,   9.90041286e-01,
		   9.95740338e-01,   9.98723761e-01,   9.99838777e-01,   1.00000000e+00};
		const PROGMEM float  brightness_lut[] =
		{ 0.        ,  0.03225806,  0.06451613,  0.09677419,  0.12903226,
		  0.16129032,  0.19354839,  0.22580645,  0.25806452,  0.29032258,
		  0.32258065,  0.35483871,  0.38709677,  0.41935484,  0.4516129 ,
		  0.48387097,  0.51612903,  0.5483871 ,  0.58064516,  0.61290323,
		  0.64516129,  0.67741935,  0.70967742,  0.74193548,  0.77419355,
		  0.80645161,  0.83870968,  0.87096774,  0.90322581,  0.93548387,
		  0.96774194,  1.        };
		const uint8_t lut_length = sizeof(phase_lut) / sizeof(float);

		const float interpolate(const float x,
			const float x_table[],
			const float y_table[],
			uint8_t table_length);
	}
};

ISR(TIMER1_CAPT_vect);
ISR(TIMER1_COMPA_vect);
ISR(TIMER1_COMPB_vect);

#endif
