/*
Simple high resolution dimming example

The function pwmWriteHR(PIN, brightness(0-65535) replaces analogWrite()
Download "PWM_lib_resolution_example.ino" to the arduino and look at the serial monitor to see examples of frequency/resolution combinations that you can use.
For LED applications to get better than 8 bits of resolution, I've found that a frequency of 240hz that gives 15.1 bits works very well.
Note that pwmWriteHR takes an unsigned integer from 0-65535, you won't see a change for each increment depending on the resolution.
I.E at a frequency of 240hz which translates to a resolution of 15.1bits this is 35119 steps, so at most you'll only see a difference every other increment(i.e 15000 to 15002)

*/

#include <PWM.h>

//use pin 11 on the Mega instead, otherwise there is a frequency cap at 31 Hz
int led = 9;                // the pin that the LED is attached to
unsigned int brightness;             // how bright the LED is
int fadeAmount = 32;         // how many points to fade the LED by.  32 was choosen as this gives a fade over about 12 seconds on an arduino uno uC
unsigned int frequency = 240;    //frequency (in Hz), gives approx 15.1 bits of PWM resolution
float Max = 65535.0;        //Maximum of a 16bit integer which is what pwmWriteHR wants
float gammaCorrection = 2.2;//

							//============================================================================
void setup()
{
	Serial.begin(115200);

	//initialize all timers except for 0, to save time keeping functions
	InitTimersSafe();

	//sets the frequency for the specified pin
	bool success = SetPinFrequencySafe(led, frequency);

	//if the pin frequency was set successfully, turn pin 13 on
	if (!success) {
		Serial.println("Failed to set frequency!");
	}
}
//============================================================================
void loop()
{

	brightness = brightness + fadeAmount;

	if (brightness == 0 || brightness == 65535) {
		fadeAmount = -fadeAmount;
	}

	float GammaCorrectedBrightness = GammaCorrection((float)brightness);

	//print out the current brightness
	Serial.print("Raw brightness="); Serial.print(brightness); Serial.print("(0-65536)        Gamma corrected brightness="); Serial.print((uint16_t)GammaCorrectedBrightness); Serial.println("(0-65535)");

	pwmWriteHR(led, (uint16_t)brightness);  //use this functions instead of analogWrite on 'initialized' pins
}
//============================================================================
float GammaCorrection(float sp) {
	//Perform a gamma correction.  LEDs do nto have a linear brightness percieved by human vision.  This helps to simulate that.
	//Pass in a value between 0 and Max, and a gamma corrected value will be returned
	return constrain(pow(sp / float(Max), gammaCorrection) * Max + 0.49, 0.0, Max); //The 0.49 is there to move the first value up a little
}